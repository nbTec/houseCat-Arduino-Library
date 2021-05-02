
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_input_button.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatInputButton::housecatInputButton(housecatProtocol &protocol, housecatInputs &inputs, uint8_t inputNumber)
: m_protocol(protocol), m_inputs(inputs), m_inputNumber(inputNumber)
{

}

unsigned long housecatInputButton::readTimeMs()
{
  return millis();
}

void housecatInputButton::poll()
{
  if(m_firstPoll)
  {
    m_protocol.addInputButton(m_inputNumber);
    m_firstPoll = false;
  }

  switch (m_inputState)
  {
    case rising_edge:
      m_shortPress = false;
      m_longPress = false;
      if (m_inputs.read(m_inputNumber))
      {
        m_inputState = rising_edge_holdoff;
        m_timerPrv = readTimeMs();
      }
      break;

    case rising_edge_holdoff:
      if ((readTimeMs() - m_timerPrv) > m_holdOffTimeMs)
      {
        if (m_inputs.read(m_inputNumber))
        {
          m_inputState = falling_edge;
        }
      }
      break;

    case falling_edge:
      if (!m_inputs.read(m_inputNumber))
      {
        if ((readTimeMs() - m_timerPrv) <= m_longPressTimeMs)
        {
          m_shortPress = true;
          m_protocol.writeInputButtonShort(m_inputNumber, true);
          m_timerPrv = readTimeMs();
          m_inputState = falling_edge_holdoff;
        }
      }
      else
      {
        if ((readTimeMs() - m_timerPrv) > m_longPressTimeMs)
        {
          m_longPress = true;
          m_protocol.writeInputButtonLong(m_inputNumber, true);
          m_inputState = long_press_wait;
        }
      }
      break;

    case long_press_wait:
      if (!m_inputs.read(m_inputNumber))
      {
        m_timerPrv = readTimeMs();
        m_inputState = falling_edge_holdoff;
      }
      break;

    case falling_edge_holdoff:
      if ((readTimeMs() - m_timerPrv) > m_holdOffTimeMs)
      {
        if(m_shortPress)
          m_protocol.writeInputButtonShort(m_inputNumber, false);
        if(m_longPress)
          m_protocol.writeInputButtonLong(m_inputNumber, false);

        m_inputState = rising_edge;
      }
      break;

    default:
      m_inputState = rising_edge;
      break;
  }
}

bool housecatInputButton::shortPress()
{
  return m_shortPress;
}

bool housecatInputButton::longPress()
{
  return m_longPress;
}
