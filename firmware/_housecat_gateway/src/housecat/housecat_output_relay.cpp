
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

housecatOutputRelay::housecatOutputRelay(housecatOutputs &outputs, uint8_t outputNumber): m_outputs(outputs), m_outputNumber(outputNumber)
{

}

void housecatOutputRelay::poll(bool toggleInput)
{
  uint8_t toggle_pressed = toggleInput && (!m_toggleInputPrv);
  if (m_firstPoll)
  {
    m_outputState = m_outputs.read(m_outputNumber); //Update output state
    m_firstPoll = false;
  }

  if (toggle_pressed)
  {
    m_outputState = !m_outputState;
    m_outputs.write(m_outputNumber, m_outputState);
  }
  m_toggleInputPrv = toggleInput;
}
