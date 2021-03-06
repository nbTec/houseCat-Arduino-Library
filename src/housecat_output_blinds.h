
#ifndef _HOUSECAT_OUTPUT_BLINDS_H_
#define _HOUSECAT_OUTPUT_BLINDS_H_

#include "housecat.h"

class housecatOutputBlinds
{
  private:
 
    uint8_t m_outputNumber_1 = 0;
	uint8_t m_outputNumber_2 = 0;
    uint8_t m_type = 0;
    uint16_t m_travelTimeSec = 30; //Travel time in seconds
    uint16_t m_switchDirectionTimeMs = 1000; //Time between a direction switch in milliseconds
    uint16_t m_interRelayDelayTimeMs = 50; //Time between a direction switch in milliseconds
	bool m_invertDirection = false;

    enumProtocolBlindsState m_protocolInternalState = blind_open;
	
	enum enumBlindsState {stop, paused, start_down, going_down, closed, switch_direction_down_to_up,
                           start_up, going_up, open, switch_direction_up_to_down};
	enumBlindsState m_blindsState = stop;

    enum enumRelayState {relays_idle, relays_stop, relays_up, relays_down, relays_disable_direction, relays_enable_power};

    enumRelayState m_relayState = relays_stop;

    bool m_firstPoll = true;
	
    bool m_upInputPrv = false;
	bool m_downInputPrv = false;

    unsigned long m_prvRelayTimeMs = 0;
    unsigned long m_prvTimeMs = 0;
    unsigned long m_prvTimeSec = 0;

    unsigned long readTimeMs();
    unsigned long readTimeSec();

    void relaysStop();
    void relaysDown();
    void relaysUp();
    void relayHandler();

  public:
    housecatOutputBlinds(uint8_t outputNumber_1, uint8_t outputNumber_2, uint16_t travelTimeSec);
	
	void invertDirection(bool state);

    void poll(bool upInput, bool downInput);
};

#endif
