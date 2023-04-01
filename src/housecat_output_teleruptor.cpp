
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_output_teleruptor.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatOutputTeleruptor::housecatOutputTeleruptor(uint8_t outputNumber)
: m_outputNumber(outputNumber)
{
  
}

unsigned long housecatOutputTeleruptor::readTimeMilliSec()
{
  return millis();
}

unsigned long housecatOutputTeleruptor::readTimeSec()
{
  return (millis() / 1000);
}

void housecatOutputTeleruptor::pulseHandler()
{
  if((m_outputState != m_outputStatePrevious) & !m_pulseInProgress)
  {
    g_housecat_outputs.write(m_outputNumber, true);
    g_housecat_protocol.writeOutput(m_outputNumber, true);
    m_pulseStartTimeMs = readTimeMilliSec();
    m_pulseInProgress = true;
  }
  else if((readTimeMilliSec() - m_pulseStartTimeMs) >= m_pulseTimeMs)
  {
    g_housecat_outputs.write(m_outputNumber, false);
    g_housecat_protocol.writeOutput(m_outputNumber, false);
    m_outputStatePrevious = m_outputState;
    m_pulseInProgress = false;
  }
}

void housecatOutputTeleruptor::enableAutoOff(unsigned long time)
{
  m_autoOff = true;
  m_autoOffTime = time;
}

void housecatOutputTeleruptor::disableAutoOff()
{
  m_autoOff = false;
}

void housecatOutputTeleruptor::enableMotion(unsigned long time)
{
  m_motion = true;
  m_motionTime = time;
}

void housecatOutputTeleruptor::disableMotion()
{
  m_motion = false;
}

void housecatOutputTeleruptor::poll(bool toggleInput)
{
  uint8_t toggle_pressed = toggleInput && (!m_toggleInputPrv);
    
  if (m_firstPoll)
  {
    m_outputState = g_housecat_outputs.read(m_outputNumber); //Update output state
    m_firstPoll = false;
  }

  if (toggle_pressed)
  {
    m_outputState = !m_outputState;

    if(m_outputState)
    {
      m_motionActive = false;
    }
    else
    {
      m_motionActive = true;
    }
    
    m_autoOffStartTime = readTimeSec();
  }

  uint8_t protocol_state = g_housecat_protocol.readOutput(m_outputNumber);
  if(protocol_state != m_outputState)
  {
    m_outputState = protocol_state;

    if(m_outputState)
    {
      m_motionActive = false;
    }
    else
    {
      m_motionActive = true;
    }

    m_autoOffStartTime = readTimeSec();
  }

  if(m_outputState)
  {
    if(m_autoOff && ((readTimeSec() - m_autoOffStartTime) >= m_autoOffTime))
    {
      m_outputState = false;
    }
  }

  m_toggleInputPrv = toggleInput;
}

void housecatOutputTeleruptor::poll(bool toggleInput, bool resetInput)
{
  uint8_t reset_pressed = resetInput && (!m_resetInputPrv);
  
  if(reset_pressed)
  {
    m_outputState = false;
    m_motionActive = true;
  }

  m_resetInputPrv = resetInput;

  poll(toggleInput);
}

void housecatOutputTeleruptor::poll(bool toggleInput, bool resetInput, bool panicInput)
{
  uint8_t panic_pressed = panicInput && (!m_panicInputPrv);

  if(panic_pressed)
  {
    if(m_panicActive == false)
    {
      m_stateBeforePanic = m_outputState;
      m_outputState = true;
      m_motionActive = false;
      m_panicActive = true;
    }
    else
    {
      m_outputState = m_stateBeforePanic;
      m_motionActive = true;
      m_panicActive = true;
    }
  }
  
  m_panicInputPrv = panicInput;

  poll(toggleInput, resetInput);
}

void housecatOutputTeleruptor::poll(bool toggleInput, bool resetInput, bool panicInput, bool motionInput)
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
            m_motionStartTime = readTimeSec();
          }

          if(m_motionRunning && ((readTimeSec() - m_motionStartTime) >= m_motionTime))
          {
            m_outputState = false;
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
