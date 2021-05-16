
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

housecatInputSensor::housecatInputSensor(housecatProtocol &protocol, housecatInputs &inputs, uint8_t inputNumber)
: m_protocol(protocol), m_inputs(inputs), m_inputNumber(inputNumber)
{

}

unsigned long housecatInputSensor::readTimeMs()
{
  return millis();
}

void housecatInputSensor::poll()
{
  if(m_firstPoll)
  {
    m_protocol.addInputSensor(m_inputNumber);
    m_firstPoll = false;
  }

  switch (m_inputState)
  {
    case rising_edge:
      m_pulse = false;
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
          m_pulse = true;
          m_protocol.writeInputSensor(m_inputNumber, true);
          m_inputState = falling_edge;
        }
      }
      break;

    case falling_edge:
      if (!m_inputs.read(m_inputNumber))
      {
        m_inputState = falling_edge_holdoff;
      }
      break;

    case falling_edge_holdoff:
      if ((readTimeMs() - m_timerPrv) > m_holdOffTimeMs)
      {
        m_protocol.writeInputSensor(m_inputNumber, false);
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
