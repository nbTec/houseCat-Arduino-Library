
#ifndef _HOUSECAT_OUTPUT_BLINDS_H_
#define _HOUSECAT_OUTPUT_BLINDS_H_

#include "housecat_outputs.h"
#include "housecat_protocol.h"

class housecatOutputBlinds
{
  private:
    housecatProtocol &m_protocol;
    housecatOutputs &m_outputs;
 
    uint8_t m_outputNumber_1 = 0;
	  uint8_t m_outputNumber_2 = 0;
    uint8_t m_type = 0;
    uint16_t m_travelTimeSec = 30; //Travel time in seconds
    uint16_t m_switchDirectionTimeMs = 1000; //Time between a direction switch in milliseconds

    enumProtocolBlindsState m_protocolInternalState = blind_open;
	
	  enum enumBlindsState {stop, paused, start_down, going_down, closed, switch_direction_down_to_up,
                           start_up, going_up, open, switch_direction_up_to_down};
	  enumBlindsState m_blindsState = open;

    bool m_firstPoll = true;
	
    bool m_upInputPrv = false;
	  bool m_downInputPrv = false;

    unsigned long m_prvTimeMs = 0;
    unsigned long m_prvTimeSec = 0;

    unsigned long readTimeMs();
    unsigned long readTimeSec();

    void relaysStop();
    void relaysDown();
    void relaysUp();

  public:
    housecatOutputBlinds(housecatProtocol &protocol, housecatOutputs &outputs, uint8_t outputNumber_1, uint8_t outputNumber_2, uint16_t travelTimeSec);

    void poll(bool upInput, bool downInput);
};

#endif
