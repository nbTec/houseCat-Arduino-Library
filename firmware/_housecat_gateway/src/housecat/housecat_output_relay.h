
#ifndef _HOUSECAT_OUTPUT_RELAY_H_
#define _HOUSECAT_OUTPUT_RELAY_H_

#include "housecat_outputs.h"

class housecatOutputRelay
{
  private:
    housecatOutputs &m_outputs;

    uint8_t m_outputNumber = 0;
    bool m_outputState = false;

    bool m_firstPoll = true;
    bool m_togglePrv = false;

  public:
    housecatOutputRelay(housecatOutputs &outputs, uint8_t outputNumber);

    void poll(bool toggle);
};

#endif
