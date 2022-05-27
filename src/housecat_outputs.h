
#ifndef _HOUSECAT_OUTPUTS_H_
#define _HOUSECAT_OUTPUTS_H_

#include <Wire.h>
#include "housecat_device_mcp23017.h"

#include "housecat_protocol.h"

class housecatOutputs
{
  private:
    static const uint8_t m_i2cBaseAddress = 4;
    static const uint8_t m_ioExpanderQuantity = 4;
    static const uint8_t m_ioExpanderPins = 16;

    housecatDeviceMcp23017 m_ioExpander[m_ioExpanderQuantity];
    uint8_t m_mapping[m_ioExpanderQuantity][m_ioExpanderPins];
    bool m_output[m_ioExpanderQuantity * m_ioExpanderPins];

    housecatProtocol &m_protocol;

  public:
    housecatOutputs(housecatProtocol &protocol);
    void init();
    bool read(uint8_t output);
    void write(uint8_t output, bool state);
    void toggle(uint8_t output, bool toggle);
    void poll();
};

#endif
