
#ifndef _HOUSECAT_ANALOG_OUTPUT_DIMMER_H_
#define _HOUSECAT_ANALOG_OUTPUT_DIMMER_H_

#include "housecat_analog_outputs.h"

class housecatAnalogOutputDimmer
{
  private:
    housecatAnalogOutputs &m_analogOutputs;

    uint8_t m_outputNumber = 0;
    uint8_t m_startValue = 0;
    bool m_outputState = false;
    uint8_t m_outputValue = 0;

    const uint8_t m_cycleIncrementValue = 10;
    const uint8_t m_maxValue = 100;
    static const unsigned long m_cycleIncrementTimerMs = 1000;
    unsigned long m_prvCycleTimeMs = 0;

    bool m_firstPoll = true;
    bool m_toggleInputPrv = false;
    bool m_cycleInputPrv = false;

    unsigned long readTimeMs();

  public:
    housecatAnalogOutputDimmer(housecatAnalogOutputs &analogOutputs, uint8_t outputNumber, uint8_t startValue, uint8_t currentValue);

    void poll(bool toggleInput, bool cycleInput);
};

#endif
