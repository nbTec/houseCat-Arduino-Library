
#ifndef _HOUSECAT_ANALOG_OUTPUTS_H_
#define _HOUSECAT_ANALOG_OUTPUTS_H_

#include <Wire.h>
#include "housecat_device_dac7678.h"

#include "housecat_protocol.h"

class housecatAnalogOutputs
{
  private:
    static const uint8_t i2cAddress = 0x4A;
    static const uint8_t m_dacOutputs = 8;
    static const uint16_t m_dacResolution = 0xFFF;
    const float m_dacFullScale = 5.00;
    static const uint8_t m_externalGain = 4;
    const float m_fullScale = 10.0;

    housecatDeviceDac7678 m_dac;
    uint8_t m_mapping[m_dacOutputs];
    float m_output[m_dacOutputs];

    housecatProtocol &m_protocol;

  public:
    housecatAnalogOutputs(housecatProtocol &protocol);
    void init();
    float readFullScale();
    float read(uint8_t output);
    void write(uint8_t output, float value);
    void poll();
};

#endif
