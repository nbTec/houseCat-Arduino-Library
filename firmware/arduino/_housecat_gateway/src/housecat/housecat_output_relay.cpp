
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_output_relay.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatOutputRelay::housecatOutputRelay(housecatProtocol &protocol, housecatOutputs &outputs, uint8_t outputNumber)
: m_protocol(protocol), m_outputs(outputs), m_outputNumber(outputNumber)
{
  
}

unsigned long housecatOutputRelay::readTimeSec()
{
  return (millis() / 1000);
}

void housecatOutputRelay::enableAutoOff(unsigned long time)
{
  m_autoOff = true;
  m_autoOffTime = time;
}

void housecatOutputRelay::disableAutoOff()
{
  m_autoOff = false;
}

void housecatOutputRelay::enableMotion(unsigned long time)
{
  m_motion = true;
  m_motionTime = time;
}

void housecatOutputRelay::disableMotion()
{
  m_motion = false;
}

void housecatOutputRelay::poll(bool toggleInput)
{
  uint8_t toggle_pressed = toggleInput && (!m_toggleInputPrv);
    
  if (m_firstPoll)
  {
    m_outputState = m_outputs.read(m_outputNumber); //Update output state
    m_protocol.addOutput(m_outputNumber);
    m_protocol.writeOutput(m_outputNumber, m_outputState);
    m_firstPoll = false;
  }

  if (toggle_pressed)
  {
    m_outputState = !m_outputState;
    m_outputs.write(m_outputNumber, m_outputState);
    m_protocol.writeOutput(m_outputNumber, m_outputState);
    m_autoOffStartTime = readTimeSec();
  }

  uint8_t protocol_state = m_protocol.readOutput(m_outputNumber);
  if(protocol_state != m_outputState)
  {
    m_outputState = protocol_state;
    m_outputs.write(m_outputNumber, m_outputState);
    m_autoOffStartTime = readTimeSec();
  }

  if(m_outputState)
  {
    if(m_autoOff && ((readTimeSec() - m_autoOffStartTime) >= m_autoOffTime))
    {
      m_outputState = false;
      m_outputs.write(m_outputNumber, m_outputState);
      m_protocol.writeOutput(m_outputNumber, m_outputState);
    }
  }

  m_toggleInputPrv = toggleInput;
}

void housecatOutputRelay::poll(bool toggleInput, bool resetInput)
{
  uint8_t reset_pressed = resetInput && (!m_resetInputPrv);
  
  if(reset_pressed)
  {
    m_outputState = false;
    m_outputs.write(m_outputNumber, m_outputState);
    m_protocol.writeOutput(m_outputNumber, m_outputState);
  }

  m_resetInputPrv = resetInput;

  poll(toggleInput);
}

void housecatOutputRelay::poll(bool toggleInput, bool resetInput, bool panicInput)
{
  uint8_t panic_pressed = panicInput && (!m_panicInputPrv);

  if(panic_pressed)
  {
    m_outputState = true;
    m_outputs.write(m_outputNumber, m_outputState);
    m_protocol.writeOutput(m_outputNumber, m_outputState);
  }
  
  m_panicInputPrv = panicInput;

  poll(toggleInput, resetInput);
}

void housecatOutputRelay::poll(bool toggleInput, bool resetInput, bool panicInput, bool motionInput)
{
  uint8_t motion_pulse = motionInput && (!m_motionInputPrv);

  if(motion_pulse)
  {
    m_motionActive = true;
    m_outputState = true;
    m_outputs.write(m_outputNumber, m_outputState);
    m_protocol.writeOutput(m_outputNumber, m_outputState);
    m_motionStartTime = readTimeSec();
  }

  if(m_motionActive && ((readTimeSec() - m_motionStartTime) >= m_motionTime))
  {
    m_outputState = false;
    m_outputs.write(m_outputNumber, m_outputState);
    m_protocol.writeOutput(m_outputNumber, m_outputState);
    m_motionActive = false;
  }
  
  m_motionInputPrv = motionInput;

  poll(toggleInput, resetInput, panicInput);
}
