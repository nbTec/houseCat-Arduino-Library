
#ifndef _HOUSECAT_OUTPUT_BLINDS_H_
#define _HOUSECAT_OUTPUT_BLINDS_H_

#include "housecat_outputs.h"

class housecatOutputBlinds
{
  private:
    housecatOutputs &m_outputs;

    uint8_t m_outputNumber_1 = 0;
	uint8_t m_outputNumber_2 = 0;
	
    bool m_outputState = false;

    bool m_firstPoll = true;
	
    bool m_upPrv = false;
	bool m_downPrv = false;

  public:
    housecatOutputBlinds(housecatOutputs &outputs, uint8_t outputNumber_1, uint8_t outputNumber_2);

    void poll(bool up, bool down);
};

#endif
