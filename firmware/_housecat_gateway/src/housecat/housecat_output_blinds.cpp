
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_output_blinds.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatOutputBlinds::housecatOutputBlinds(housecatOutputs &outputs, uint8_t outputNumber_1, uint8_t outputNumber_2): m_outputs(outputs), m_outputNumber_1(outputNumber_1), m_outputNumber_2(outputNumber_2)
{

}

void housecatOutputBlinds::poll(bool up, bool down)
{
  if (m_firstPoll)
  {

    m_firstPoll = false;
  }

  if (up && (!m_upPrv))
  {

  }
  
  if (down && (!m_downPrv))
  {

  }
  
  m_upPrv = up;
  m_downPrv = down;
}
