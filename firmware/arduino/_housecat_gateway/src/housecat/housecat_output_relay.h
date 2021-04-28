
#ifndef _HOUSECAT_OUTPUT_RELAY_H_
#define _HOUSECAT_OUTPUT_RELAY_H_

#include "housecat_outputs.h"
#include "housecat_protocol.h"

class housecatOutputRelay
{
  private:
    housecatOutputs &m_outputs;
    housecatProtocol &m_protocol;

    uint8_t m_outputNumber = 0;
    bool m_outputState = false;

    bool m_firstPoll = true;
    bool m_toggleInputPrv = false;

  public:
    housecatOutputRelay(housecatOutputs &outputs, housecatProtocol &protocol, uint8_t outputNumber);

    void poll(bool toggleInput);
};

#endif
