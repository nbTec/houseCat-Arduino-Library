
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_input_sensor.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatInputSensor::housecatInputSensor(uint8_t inputNumber, bool activeState)
: m_inputNumber(inputNumber), m_activeState(activeState)
{

}

unsigned long housecatInputSensor::readTimeMs()
{
  return millis();
}

void housecatInputSensor::poll()
{
  bool input_state = false;

  if(m_firstPoll)
  {
    g_housecat_protocol.addInputSensor(m_inputNumber);
    m_firstPoll = false;
  }

  if(m_activeState)
    input_state = g_housecat_inputs.read(m_inputNumber);
  else
    input_state = !g_housecat_inputs.read(m_inputNumber);

  switch (m_inputState)
  {
    case rising_edge:
      m_pulse = false;
      if (input_state)
      {
        m_inputState = rising_edge_holdoff;
        m_timerPrv = readTimeMs();
      }
      break;

    case rising_edge_holdoff:
      if ((readTimeMs() - m_timerPrv) > m_holdOffTimeMs)
      {
        if (input_state)
        {
          m_pulse = true;
          g_housecat_protocol.writeInputSensor(m_inputNumber, true);
          m_inputState = falling_edge;
        }
      }
      break;

    case falling_edge:
      if (!input_state)
      {
        m_inputState = falling_edge_holdoff;
      }
      break;

    case falling_edge_holdoff:
      if ((readTimeMs() - m_timerPrv) > m_holdOffTimeMs)
      {
        g_housecat_protocol.writeInputSensor(m_inputNumber, false);
        m_inputState = rising_edge;
      }
      break;

    default:
      m_inputState = rising_edge;
      break;
  }
}

bool housecatInputSensor::pulse()
{
  return m_pulse;
}
