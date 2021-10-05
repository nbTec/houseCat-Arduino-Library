
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
        g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / 10);
    }
    else
    {
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

        g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / 10);
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
        g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / 10);
    }
    else
    {
        g_housecat_analog_outputs.write(m_outputNumber, 0.0);
    }
  }

  uint8_t protocol_value = g_housecat_protocol.readDimmerValue(m_outputNumber);
  if(m_outputValue != protocol_value)
  {
    m_outputValue = protocol_value;
    if(m_outputState == true)
    {
        g_housecat_analog_outputs.write(m_outputNumber, m_outputValue / 10);
    }
  }

  m_toggleInputPrv = toggleInput;
  //m_cycleInputPrv = cycleInput;
}
