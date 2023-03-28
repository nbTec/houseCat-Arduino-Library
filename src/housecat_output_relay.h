
#ifndef _HOUSECAT_OUTPUT_RELAY_H_
#define _HOUSECAT_OUTPUT_RELAY_H_

#include "housecat.h"

class housecatOutputRelay
{
  private:
    
    uint8_t m_outputNumber = 0;
    bool m_outputState = false;

    bool m_autoOff = false;
    unsigned long m_autoOffStartTime = 0;
    unsigned long m_autoOffTime = 60;

    bool m_motion = false;
    unsigned long m_motionStartTime = 0;
    unsigned long m_motionTime = 60;
    unsigned long m_motionHoldoffStartTime = 0;
    unsigned long m_motionHoldoffTime = 5;
    bool m_motionHoldoffActive = false;
    bool m_motionActive = false;
    bool m_motionActivePrv = false;
    bool m_motionRunning = false;

    bool m_stateBeforePanic = false;
    bool m_panicActive = false;

    bool m_firstPoll = true;
    bool m_toggleInputPrv = false;
    bool m_resetInputPrv = false;
    bool m_panicInputPrv = false;
    bool m_motionInputPrv = false;

    unsigned long readTimeSec();
    void handleOutputState();

  public:
    housecatOutputRelay(uint8_t outputNumber);
    
    void enableAutoOff(unsigned long time);
    void disableAutoOff();
    
    void enableMotion(unsigned long time);
    void disableMotion();

    void poll(bool toggleInput);
    void poll(bool toggleInput, bool resetInput);
    void poll(bool toggleInput, bool resetInput, bool panicInput);
    void poll(bool toggleInput, bool resetInput, bool panicInput, bool motionInput);
};

#endif
