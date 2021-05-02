
#ifndef _HOUSECAT_OUTPUT_RELAY_H_
#define _HOUSECAT_OUTPUT_RELAY_H_

#include "housecat_outputs.h"
#include "housecat_protocol.h"

class housecatOutputRelay
{
  private:
    housecatProtocol &m_protocol;
    housecatOutputs &m_outputs;
    
    uint8_t m_outputNumber = 0;
    bool m_outputState = false;

    bool m_autoOff = false;
    unsigned long m_autoOffStartTime = 0;
    unsigned long m_autoOffTime = 60;

    bool m_motion = false;
    unsigned long m_motionStartTime = 0;
    unsigned long m_motionTime = 60;
    bool m_motionActive = false;

    bool m_firstPoll = true;
    bool m_toggleInputPrv = false;
    bool m_resetInputPrv = false;
    bool m_motionInputPrv = false;
    bool m_panicInputPrv = false;

    unsigned long readTimeSec();

  public:
    housecatOutputRelay(housecatProtocol &protocol, housecatOutputs &outputs, uint8_t outputNumber);
    
    void enableAutoOff(unsigned long time);
    void disableAutoOff();
    
    void enableMotion(unsigned long time);
    void disableMotion();

    void poll(bool toggleInput);
    void poll(bool toggleInput, bool resetInput);
    void poll(bool toggleInput, bool resetInput, bool motionInput);
    void poll(bool toggleInput, bool resetInput, bool motionInput, bool panicInput);
};

#endif
