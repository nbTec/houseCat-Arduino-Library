
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
    g_housecat_protocol.writeOutput(m_outputNumber, true);
    m_pulseStartTimeMs = readTimeMilliSec();
    m_pulseInProgress = true;
  }
  else if((readTimeMilliSec() - m_pulseStartTimeMs) >= m_pulseTimeMs)
  {
    g_housecat_protocol.writeOutput(m_outputNumber, false);
    m_pulseInProgress = true;
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
    g_housecat_protocol.addOutput(m_outputNumber);
    g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
    m_firstPoll = false;
  }

  if (toggle_pressed)
  {
    m_outputState = !m_outputState;
    g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
    m_autoOffStartTime = readTimeSec();
  }

  uint8_t protocol_state = g_housecat_protocol.readOutput(m_outputNumber);
  if(protocol_state != m_outputState)
  {
    m_outputState = protocol_state;
    m_autoOffStartTime = readTimeSec();
  }

  if(m_outputState)
  {
    if(m_autoOff && ((readTimeSec() - m_autoOffStartTime) >= m_autoOffTime))
    {
      m_outputState = false;
      g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
    }
  }

  pulseHandler();

  m_toggleInputPrv = toggleInput;
}

void housecatOutputTeleruptor::poll(bool toggleInput, bool resetInput)
{
  uint8_t reset_pressed = resetInput && (!m_resetInputPrv);
  
  if(reset_pressed)
  {
    m_outputState = false;
    g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
  }

  m_resetInputPrv = resetInput;

  poll(toggleInput);
}

void housecatOutputTeleruptor::poll(bool toggleInput, bool resetInput, bool panicInput)
{
  uint8_t panic_pressed = panicInput && (!m_panicInputPrv);

  if(panic_pressed)
  {
    m_outputState = true;
    g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
  }
  
  m_panicInputPrv = panicInput;

  poll(toggleInput, resetInput);
}

void housecatOutputTeleruptor::poll(bool toggleInput, bool resetInput, bool panicInput, bool motionInput)
{
  uint8_t motion_pulse = motionInput && (!m_motionInputPrv);

  if(m_motion)
  {
    if(motion_pulse && (m_outputState == false))
    {
      m_motionActive = true;
      m_outputState = true;
      g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
      m_motionStartTime = readTimeSec();
    }

    if(m_motionActive && ((readTimeSec() - m_motionStartTime) >= m_motionTime))
    {
      m_outputState = false;
      g_housecat_protocol.writeOutput(m_outputNumber, m_outputState);
      m_motionActive = false;
    }
  }
  
  m_motionInputPrv = motionInput;

  poll(toggleInput, resetInput, panicInput);
}
