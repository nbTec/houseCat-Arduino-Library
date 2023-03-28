
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_analog_output_dimmer.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatAnalogOutputDimmer::housecatAnalogOutputDimmer(uint8_t outputNumber, uint8_t startValue, uint8_t defaultValue)
: m_outputNumber(outputNumber), m_startValue(startValue), m_outputValue(defaultValue)
{
}

unsigned long housecatAnalogOutputDimmer::readTimeMs()
{
  return millis();
}

unsigned long housecatAnalogOutputDimmer::readTimeSec()
{
  return (millis() / 1000);
}

void housecatAnalogOutputDimmer::analogOutputHandler()
{
  
}

void housecatAnalogOutputDimmer::enableAutoOff(unsigned long time)
{
  m_autoOff = true;
  m_autoOffTime = time;
}

void housecatAnalogOutputDimmer::disableAutoOff()
{
  m_autoOff = false;
}

void housecatAnalogOutputDimmer::enableMotion(unsigned long time)
{
  m_motion = true;
  m_motionTime = time;
}

void housecatAnalogOutputDimmer::disableMotion()
{
  m_motion = false;
}

void housecatAnalogOutputDimmer::poll(bool toggleInput, bool cycleInput)
{
  uint8_t toggle_pressed = toggleInput && (!m_toggleInputPrv);

  if (m_firstPoll)
  {
    g_housecat_protocol.addDimmer(m_outputNumber);
    g_housecat_protocol.writeDimmerState(m_outputNumber, m_outputState);
    g_housecat_protocol.writeDimmerValue(m_outputNumber, m_outputValue);
    m_firstPoll = false;
  }

  if (toggle_pressed)
  {
    m_outputState = !m_outputState;
    
    if(m_outputState == true)
    {
      m_motionActive = false;
      g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / m_analogOutputDivider);
      m_autoOffStartTime = readTimeSec();
    }
    else
    {
      m_motionActive = true;
      g_housecat_analog_outputs.write(m_outputNumber, 0.0);
    }
    g_housecat_protocol.writeDimmerState(m_outputNumber, m_outputState);
  }

  if (cycleInput && m_outputState)
  {
      if((readTimeMs() - m_prvCycleTimeMs) >= m_cycleIncrementTimerMs)
      {
        m_outputValue += m_cycleIncrementValue;

        if(m_outputValue > m_maxValue)
        {
          m_outputValue = m_startValue;
        }

        g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / m_analogOutputDivider);
        g_housecat_protocol.writeDimmerValue(m_outputNumber, m_outputValue);
        m_prvCycleTimeMs = readTimeMs();
      }
  }

  bool protocol_state = g_housecat_protocol.readDimmerState(m_outputNumber);
  if(m_outputState != protocol_state)
  {
    m_outputState = protocol_state;
    if(m_outputState == true)
    {
      m_motionActive = false;
      g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / m_analogOutputDivider);
    }
    else
    {
      m_motionActive = true;
      g_housecat_analog_outputs.write(m_outputNumber, 0.0);
    }
  }

  uint8_t protocol_value = g_housecat_protocol.readDimmerValue(m_outputNumber);
  if(m_outputValue != protocol_value)
  {
    m_outputValue = protocol_value;
    if(m_outputState == true)
    {
      g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / m_analogOutputDivider);
    }
  }

  if(m_outputState)
  {
    if(m_autoOff && ((readTimeSec() - m_autoOffStartTime) >= m_autoOffTime))
    {
      m_outputState = false;
      g_housecat_analog_outputs.write(m_outputNumber, 0.0);
      g_housecat_protocol.writeDimmerState(m_outputNumber, m_outputState);
    }
  }

  m_toggleInputPrv = toggleInput;
  //m_cycleInputPrv = cycleInput;
}

void housecatAnalogOutputDimmer::poll(bool toggleInput, bool cycleInput, bool resetInput)
{
  uint8_t reset_pressed = resetInput && (!m_resetInputPrv);

  if(reset_pressed)
  {
    m_outputState = false;
    g_housecat_analog_outputs.write(m_outputNumber, 0.0);
    g_housecat_protocol.writeDimmerState(m_outputNumber, m_outputState);
  }

  m_resetInputPrv = resetInput;

  poll(toggleInput, cycleInput);
}

void housecatAnalogOutputDimmer::poll(bool toggleInput, bool cycleInput, bool resetInput, bool panicInput)
{
  uint8_t panic_pressed = panicInput && (!m_panicInputPrv);

  if(panic_pressed)
  {
    if(m_panicActive == false)
    {
      m_stateBeforePanic = m_outputState;
      m_outputState = true;
      g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / m_analogOutputDivider);
      m_panicActive = true;
    }
    else
    {
      m_outputState = m_stateBeforePanic;

      if(m_outputState)
      {
        g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / m_analogOutputDivider);
      }
      else
      {
        g_housecat_analog_outputs.write(m_outputNumber, 0.0);
      }

      m_panicActive = false;
    }
    g_housecat_protocol.writeDimmerState(m_outputNumber, m_outputState);
  }

  m_panicInputPrv = panicInput;

  poll(toggleInput, cycleInput);
}

void housecatAnalogOutputDimmer::poll(bool toggleInput, bool cycleInput, bool resetInput, bool panicInput, bool motionInput)
{
  bool motion_pulse = motionInput && (!m_motionInputPrv);
  bool motion_active_enabled = m_motionActive && (!m_motionActivePrv);

  if(m_motion)
  {
    if(m_motionActive == true)
    {
      if(motion_active_enabled)
      {
        m_motionHoldoffStartTime = readTimeSec();
        m_motionHoldoffActive = true;
      }
      else
      {
        if(m_motionHoldoffActive == true)
        {
          if((readTimeSec() - m_motionHoldoffStartTime) >= m_motionHoldoffTime)
          {
            m_motionHoldoffActive = false;
          }
        }
        else
        {
          if(motion_pulse && (m_outputState == false))
          {
            m_motionRunning = true;
            m_outputState = true;
            g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / m_analogOutputDivider);
            g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
            m_motionStartTime = readTimeSec();
          }

          if(m_motionRunning && ((readTimeSec() - m_motionStartTime) >= m_motionTime))
          {
            m_outputState = false;
            g_housecat_analog_outputs.write(m_outputNumber, 0.0);
            g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
            m_motionRunning = false;
          }
        }
      }
    }
  }
  
  m_motionActivePrv = m_motionActive;
  m_motionInputPrv = motionInput;

  poll(toggleInput, resetInput, panicInput);
}
