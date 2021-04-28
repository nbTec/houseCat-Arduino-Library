
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

housecatOutputBlinds::housecatOutputBlinds(housecatOutputs &outputs, uint8_t outputNumber_1, uint8_t outputNumber_2, uint16_t travelTimeSec)
:m_outputs(outputs), m_outputNumber_1(outputNumber_1), m_outputNumber_2(outputNumber_2), m_travelTimeSec(travelTimeSec)
{

}

unsigned long housecatOutputBlinds::readTimeMs()
{
  return millis();
}

unsigned long housecatOutputBlinds::readTimeSec()
{
  return (millis() / 1000);
}

void housecatOutputBlinds::relaysStop()
{
	m_outputs.write(m_outputNumber_1, false);
	m_outputs.write(m_outputNumber_2, false);
}

void housecatOutputBlinds::relaysDown()
{
	m_outputs.write(m_outputNumber_1, true);
	m_outputs.write(m_outputNumber_2, true);	
}

void housecatOutputBlinds::relaysUp()
{
	m_outputs.write(m_outputNumber_1, true);
	m_outputs.write(m_outputNumber_2, false);
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
		relaysStop();
		m_blindsState = paused;
	  	break;

	  case paused:
		if (down_pressed)
		{
			m_blindsState = start_down;
		}
		if (up_pressed)
		{
			m_blindsState = start_up;
		}
	  	break;

	  case start_down:
		relaysDown();
		m_prvTimeSec = readTimeSec();
		m_blindsState = going_down;
	  	break;

	  case going_down:
		if (down_pressed)
		{
			m_blindsState = stop;
		}
		if (up_pressed)
		{
			relaysStop();
			m_prvTimeMs = readTimeMs();
			m_blindsState = switch_direction_down_to_up;
		}
		if((readTimeSec() - m_prvTimeSec) >= m_travelTimeSec)
		{
			relaysStop();
			m_blindsState = fully_down;
		}
	  	break;
	  
	  case fully_down:
	  	if (up_pressed)
		{
			m_blindsState = start_up;
		}
	  	break;
	
	  case switch_direction_down_to_up:
	  	if ((readTimeMs() - m_prvTimeMs) >= m_switchDirectionTimeMs)
		{
			m_blindsState = start_up;
		}
	  	break;
	
	  case start_up:
	  	relaysUp();
		m_prvTimeSec = readTimeSec();
		m_blindsState = going_up;
	  	break;

	  case going_up:
	  	if (down_pressed)
		{
			relaysStop();
			m_prvTimeMs = readTimeMs();
			m_blindsState = switch_direction_up_to_down;
		}
		if (up_pressed)
		{
			m_blindsState = stop;

		}
		if((readTimeSec() - m_prvTimeSec) >= m_travelTimeSec)
		{
			relaysStop();
			m_blindsState = fully_up;
		}
	  	break;

	  case fully_up:
	  	if (down_pressed)
		{
			m_blindsState = start_down;
		}
	  	break;

	  case switch_direction_up_to_down:
	  	if ((readTimeMs() - m_prvTimeMs) >= m_switchDirectionTimeMs)
		{
			m_blindsState = start_down;
		}
	  	break;
	  
	  default:
		m_blindsState = stop;
	  	break;
  }
  
  m_upInputPrv = upInput;
  m_downInputPrv = downInput;
}
