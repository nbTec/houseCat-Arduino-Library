
#ifndef _HOUSECAT_OUTPUT_BLINDS_H_
#define _HOUSECAT_OUTPUT_BLINDS_H_

#include "housecat_outputs.h"

class housecatOutputBlinds
{
  private:
    housecatOutputs &m_outputs;
	
	uint8_t m_type = 0;
	uint16_t m_travelTimer = 30; //Travel time in seconds

  uint8_t m_outputNumber_1 = 0;
	uint8_t m_outputNumber_2 = 0;
	
	enum enumBlindsState {stop, up, fully_up, down, fully_down};
	enumBlindsState m_blindsState = stop;

  bool m_firstPoll = true;
	
  bool m_upInputPrv = false;
	bool m_downInputPrv = false;

  public:
    housecatOutputBlinds(housecatOutputs &outputs, uint8_t outputNumber_1, uint8_t outputNumber_2);

    void poll(bool upInput, bool downInput);
};

#endif
