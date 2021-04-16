
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

housecatInputButton::housecatInputButton(housecatInputs &inputs, uint8_t inputNumber): m_inputs(inputs), m_inputNumber(inputNumber)
{

}

unsigned long housecatInputButton::readTime()
{
  return millis();
}

void housecatInputButton::poll()
{
    switch (m_inputState)
  {
    case rising_edge:
      m_shortPress = false;
      m_longPress = false;
      if (m_inputs.read(m_inputNumber))
      {
        m_inputState = rising_edge_holdoff;
        m_timerPrv = readTime();
      }
      break;

    case rising_edge_holdoff:
      if ((readTime() - m_timerPrv) > m_holdOffTimeMs)
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
        if ((readTime() - m_timerPrv) <= m_longPressTimeMs)
        {
          m_shortPress = true;
          m_timerPrv = readTime();
          m_inputState = falling_edge_holdoff;
        }
      }
      else
      {
        if ((readTime() - m_timerPrv) > m_longPressTimeMs)
        {
          m_longPress = true;
          m_inputState = long_press_wait;
        }
      }
      break;

    case long_press_wait:
      if (!m_inputs.read(m_inputNumber))
      {
        m_timerPrv = readTime();
        m_inputState = falling_edge_holdoff;
      }
      break;

    case falling_edge_holdoff:
      if ((readTime() - m_timerPrv) > m_holdOffTimeMs)
      {
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
