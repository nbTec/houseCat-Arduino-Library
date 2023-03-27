
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

void housecatOutputBlinds::invertDirection(bool state = true)
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
		}
		if (up_pressed)
		{
			m_blindsState = start_up;
			m_protocolInternalState = blind_up;
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		}
		
		if(!g_housecat_protocol.modbusEnabled()) //Modbus only has binary state
		{
			if(protocol_down)
			{
				m_blindsState = start_down;
				m_protocolInternalState = protocolExternalState;
			}	
			if(protocol_up)
			{
				m_blindsState = start_up;
				m_protocolInternalState = protocolExternalState;
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
	  	break;

	  case going_down:
		if (down_pressed or protocol_stop)
		{
			m_blindsState = stop;
		}
		if (up_pressed or protocol_stop)
		{
			m_blindsState = stop;
		}
		if((readTimeSec() - m_prvTimeSec) >= m_travelTimeSec)
		{
			relaysStop();
			m_protocolInternalState = blind_closed;
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
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
		if(!g_housecat_protocol.modbusEnabled())
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
		m_blindsState = going_up;
	  	break;

	  case going_up:
	  	if (down_pressed or protocol_stop)
		{
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
			g_housecat_protocol.writeBlind(m_outputNumber_1, m_protocolInternalState);
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

  relayHandler();
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
	m_relayState = relays_stop;
}

void housecatOutputBlinds::relaysDown()
{
	m_relayState = relays_down;
}

void housecatOutputBlinds::relaysUp()
{
	m_relayState = relays_up;
}

void housecatOutputBlinds::relayHandler()
{
	switch(m_relayState)
	{
		case relays_idle:
			break;

		case relays_stop:
			g_housecat_outputs.write(m_outputNumber_1, false);
			m_prvRelayTimeMs = readTimeMs();
			m_relayState = relays_disable_direction;
			break;

		case relays_down:
			g_housecat_outputs.write(m_outputNumber_2, !m_invertDirection);
			m_prvRelayTimeMs = readTimeMs();
			m_relayState = relays_enable_power;
			break;

		case relays_up:
			g_housecat_outputs.write(m_outputNumber_2, m_invertDirection);
			m_prvRelayTimeMs = readTimeMs();
			m_relayState = relays_enable_power;
			break;

		case relays_disable_direction:
			if((readTimeMs() - m_prvRelayTimeMs) >= m_interRelayDelayTimeMs)
			{
				g_housecat_outputs.write(m_outputNumber_2, false);
				m_relayState = relays_idle;
			}
			break;

		case relays_enable_power:
			if((readTimeMs() - m_prvRelayTimeMs) >= m_interRelayDelayTimeMs)
			{
				g_housecat_outputs.write(m_outputNumber_1, true);
				m_relayState = relays_idle;
			}
			break;

		default:
			m_relayState = relays_idle;
			break;
	}
}
