
#ifndef _HOUSECAT_ANALOG_OUTPUT_DIMMER_H_
#define _HOUSECAT_ANALOG_OUTPUT_DIMMER_H_

#include "housecat_analog_outputs.h"

class housecatAnalogOutputDimmer
{
  private:
    housecatAnalogOutputs &m_analogOutputs;

    uint8_t m_outputNumber = 0;
    float m_startValue = 0.0;
    bool m_outputState = false;
    float m_outputValue = 0.0;

    bool m_firstPoll = true;
    bool m_toggleInputPrv = false;
    bool m_cycleInputPrv = false;

    const float m_cycleIncrementValue = 1.0;
    const float m_maxValue = 10.0;
    static const unsigned long m_cycleIncrementTimerMs = 1000;
    unsigned long m_prvCycleTimeMs = 0;

    unsigned long readTimeMs();

  public:
    housecatAnalogOutputDimmer(housecatAnalogOutputs &analogOutputs, uint8_t outputNumber, float startValue, float currentValue);

    void poll(bool toggleInput, bool cycleInput);
};

#endif
