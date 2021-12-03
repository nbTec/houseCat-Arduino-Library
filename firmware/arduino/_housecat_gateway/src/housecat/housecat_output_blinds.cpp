
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

housecatOutputBlinds::housecatOutputBlinds(uint8_t outputNumber_1, uint8_t outputNumber_2, uint16_t travelTimeSec)
:m_outputNumber_1(outputNumber_1), m_outputNumber_2(outputNumber_2), m_travelTimeSec(travelTimeSec)
{

}

void housecatOutputBlinds::invertDirection(bool state)
{
	m_invertDirection = state;
}

void housecatOutputBlinds::poll(bool upInput, bool downInput)
{
  uint8_t up_pressed = upInput && (!m_upInputPrv);
  uint8_t down_pressed = downInput && (!m_downInputPrv);

  enumProtocolBlindsState protocolExternalState = g_housecat_protocol.readBlind(m_outputNumber_1);
  bool protocol_down = ((m_protocolInternalState != protocolExternalState) && (protocolExternalState == blind_down));
  bool protocol_up = ((m_protocolInternalState != protocolExternalState) && (protocolExternalState == blind_up));
  bool protocol_stop = ((m_protocolInternalState != protocolExternalState) && (protocolExternalState == blind_stop));
  
  if (m_firstPoll)
  {
	g_housecat_protocol.addBlind(m_outputNumber_1);
	g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
    m_firstPoll = false;
  }
  
  switch (m_blindsState)
  {
	  case stop:
		relaysStop();
		m_protocolInternalState = blind_stop;
		g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		m_blindsState = paused;
	  	break;

	  case paused:
		if (down_pressed)
		{
			m_blindsState = start_down;
			m_protocolInternalState = blind_down;
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
			Serial.println("start_down");
		}
		if (up_pressed)
		{
			m_blindsState = start_up;
			m_protocolInternalState = blind_up;
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
			Serial.println("start_up");
		}
		
		if(!g_housecat_protocol.modbusEnabled()) //Modbus only has binary state
		{
			if(protocol_down)
			{
				m_blindsState = start_down;
				m_protocolInternalState = protocolExternalState;
				Serial.println("start_down_protocol");
			}	
			if(protocol_up)
			{
				m_blindsState = start_up;
				m_protocolInternalState = protocolExternalState;
				Serial.println("start_up_protocol");
			}
		}
		
	  	break;

	  case start_down:
		relaysDown();
		m_prvTimeSec = readTimeSec();
		m_protocolInternalState = blind_down;
		if(!g_housecat_protocol.modbusEnabled())
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		m_blindsState = going_down;
		Serial.println("going_down");
	  	break;

	  case going_down:
		if (down_pressed or protocol_stop)
		{
			m_blindsState = stop;
			Serial.println("stop");
		}
		if (up_pressed or protocol_stop)
		{
			//relaysStop();
			//m_prvTimeMs = readTimeMs();
			//m_blindsState = switch_direction_down_to_up;
			m_blindsState = stop;
			Serial.println("stop");
		}
		if((readTimeSec() - m_prvTimeSec) >= m_travelTimeSec)
		{
			relaysStop();
			m_protocolInternalState = blind_closed;
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
			m_blindsState = closed;
			Serial.println("closed");
		}
	  	break;
	  
	  case closed:
	  	if (up_pressed or protocol_up)
		{
			m_blindsState = start_up;
			Serial.println("start_up");
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
		if(!g_housecat_protocol.modbusEnabled())
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		m_blindsState = going_up;
	  	break;

	  case going_up:
	  	if (down_pressed or protocol_stop)
		{
			//relaysStop();
			//m_prvTimeMs = readTimeMs();
			//m_blindsState = switch_direction_up_to_down;
			m_blindsState = stop;
			Serial.println("stop");
		}
		if (up_pressed or protocol_stop)
		{
			m_blindsState = stop;
			Serial.println("stop");
		}
		if((readTimeSec() - m_prvTimeSec) >= m_travelTimeSec)
		{
			relaysStop();
	  		m_protocolInternalState = blind_open;
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
			m_blindsState = open;
			Serial.println("open");
		}
	  	break;

	  case open:
	  	if (down_pressed or protocol_down)
		{
			m_blindsState = start_down;
			Serial.println("start_down");
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
	g_housecat_outputs.write(m_outputNumber_1, false);
	g_housecat_outputs.write(m_outputNumber_2, false);
}

void housecatOutputBlinds::relaysDown()
{
	g_housecat_outputs.write(m_outputNumber_1, true);
	g_housecat_outputs.write(m_outputNumber_2, !m_invertDirection);
}

void housecatOutputBlinds::relaysUp()
{
	g_housecat_outputs.write(m_outputNumber_1, true);
	g_housecat_outputs.write(m_outputNumber_2, m_invertDirection);
}
