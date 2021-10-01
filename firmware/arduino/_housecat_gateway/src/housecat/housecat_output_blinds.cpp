
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

housecatOutputBlinds::housecatOutputBlinds(housecatProtocol &protocol, housecatOutputs &outputs, uint8_t outputNumber_1, uint8_t outputNumber_2, uint16_t travelTimeSec)
:m_protocol(protocol), m_outputs(outputs), m_outputNumber_1(outputNumber_1), m_outputNumber_2(outputNumber_2), m_travelTimeSec(travelTimeSec)
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

  enumProtocolBlindsState m_protocolExternalState = m_protocol.readBlind(m_outputNumber_1);
  bool protocol_up = ((m_protocolInternalState != m_protocolExternalState) && (m_protocolExternalState == blind_up));
  bool protocol_down = ((m_protocolInternalState != m_protocolExternalState) && (m_protocolExternalState == blind_down));
  bool protocol_stop = ((m_protocolInternalState != m_protocolExternalState) && (m_protocolExternalState == blind_stop));
  
  if (m_firstPoll)
  {
	//m_protocol.addBlind(upInput);
	m_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
    m_firstPoll = false;
  }
  
  switch (m_blindsState)
  {
	  case stop:
		relaysStop();
		m_protocolInternalState = blind_stop;
		m_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		m_blindsState = paused;
	  	break;

	  case paused:
		if (down_pressed)
		{
			m_blindsState = start_down;
			m_protocolInternalState = blind_down;
			m_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		}
		if (up_pressed)
		{
			m_blindsState = start_up;
			m_protocolInternalState = blind_up;
			m_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		}
		
		if(m_protocolInternalState != m_protocolExternalState)
		{
			if(m_protocolExternalState == blind_down)
			{
				m_blindsState = start_down;
			}	
			if(m_protocolExternalState == blind_up)
			{
				m_blindsState = start_up;
			}	
			m_protocolInternalState = m_protocolExternalState;
		}
	  	break;

	  case start_down:
		relaysDown();
		m_prvTimeSec = readTimeSec();
		m_protocolInternalState = blind_down;
		m_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		m_blindsState = going_down;
	  	break;

	  case going_down:
		if (down_pressed or protocol_stop)
		{
			m_blindsState = stop;
		}
		if (up_pressed or protocol_up)
		{
			//relaysStop();
			//m_prvTimeMs = readTimeMs();
			//m_blindsState = switch_direction_down_to_up;
			m_blindsState = stop;
		}
		if((readTimeSec() - m_prvTimeSec) >= m_travelTimeSec)
		{
			relaysStop();
			m_protocolInternalState = blind_closed;
			m_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
			m_blindsState = closed;
		}
	  	break;
	  
	  case closed:
	  	if (up_pressed or protocol_up)
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
		m_protocolInternalState = blind_up;
		m_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		m_blindsState = going_up;
	  	break;

	  case going_up:
	  	if (down_pressed or protocol_down)
		{
			//relaysStop();
			//m_prvTimeMs = readTimeMs();
			//m_blindsState = switch_direction_up_to_down;
			m_blindsState = stop;
		}
		if (up_pressed or protocol_stop)
		{
			m_blindsState = stop;
		}
		if((readTimeSec() - m_prvTimeSec) >= m_travelTimeSec)
		{
			relaysStop();
	  		m_protocolInternalState = blind_open;
			m_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
			m_blindsState = open;
		}
	  	break;

	  case open:
	  	if (down_pressed or protocol_down)
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
