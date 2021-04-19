
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

void housecatOutputBlinds::poll(bool upInput, bool downInput)
{
  uint8_t up_pressed = upInput && (!m_upInputPrv);
  uint8_t down_pressed = downInput && (!m_downInputPrv);
  
  if (m_firstPoll)
  {

    m_firstPoll = false;
  }
  
  switch (m_blindsState)
  {
	  case stop:
		m_outputs.write(m_outputNumber_1, false);
		m_outputs.write(m_outputNumber_2, false);
		if (up_pressed)
		{
			
		}
	  break;
	
	  case up:

	  break;
	  
	  case fully_up:
	  
	  break;
	  
	  case down:
	  
	  break;
	  
	  case fully_down:
	  
	  break;
	  
	  default:
		m_blindsState = stop;
	  break;
  }
  
  m_upInputPrv = up;
  m_downInputPrv = down;
}
