
#ifndef _HOUSECAT_ANALOG_OUTPUT_DIMMER_H_
#define _HOUSECAT_ANALOG_OUTPUT_DIMMER_H_

#include "housecat.h"

class housecatAnalogOutputDimmer
{
  private:

    uint8_t m_outputNumber = 0;
    uint8_t m_startValue = 0;
    bool m_outputState = false;
    uint8_t m_outputValue = 0;

    const uint8_t m_cycleIncrementValue = 10;
    const uint8_t m_maxValue = 100;
    static const unsigned long m_cycleIncrementTimerMs = 1000;
    unsigned long m_prvCycleTimeMs = 0;

    const uint8_t m_analogOutputDivider = 10;

    bool m_autoOff = false;
    unsigned long m_autoOffStartTime = 0;
    unsigned long m_autoOffTime = 60;

    bool m_motion = false;
    unsigned long m_motionStartTime = 0;
    unsigned long m_motionTime = 60;
    bool m_motionActive = false;

    bool m_firstPoll = true;
    bool m_toggleInputPrv = false;
    bool m_cycleInputPrv = false;
    bool m_resetInputPrv = false;
    bool m_panicInputPrv = false;
    bool m_motionInputPrv = false;

    unsigned long readTimeMs();
    unsigned long readTimeSec();

  public:
    housecatAnalogOutputDimmer(uint8_t outputNumber, uint8_t startValue, uint8_t defaultValue);

    void poll(bool toggleInput, bool cycleInput);
    void poll(bool toggleInput, bool cycleInput, bool resetInput);
    void poll(bool toggleInput, bool cycleInput, bool resetInput, bool panicInput);
    void poll(bool toggleInput, bool cycleInput, bool resetInput, bool panicInput, bool motionInput);
};

#endif
