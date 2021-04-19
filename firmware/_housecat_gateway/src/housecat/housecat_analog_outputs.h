
#ifndef _HOUSECAT_ANALOG_OUTPUTS_H_
#define _HOUSECAT_ANALOG_OUTPUTS_H_

#include <Wire.h>
#include "../DAC7678/DAC7678.h"

class housecatAnalogOutputs
{
  private:
    static const uint8_t i2cAddress = 0x4A;
    static const uint8_t m_dacOutputs = 8;
    static const uint16_t m_resolution = 0xFFF;
    const float m_fullScale = 5.00;
    static const uint8_t m_externalGain = 4;

    DAC7678 m_dac;
    uint8_t m_mapping[m_dacOutputs];
    float m_output[m_dacOutputs];

  public:
    housecatAnalogOutputs();
    void init();
    float read(uint8_t output);
    void write(uint8_t output, float value);
};

#endif
