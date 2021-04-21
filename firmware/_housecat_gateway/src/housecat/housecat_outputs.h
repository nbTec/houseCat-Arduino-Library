
#ifndef _HOUSECAT_OUTPUTS_H_
#define _HOUSECAT_OUTPUTS_H_

#include <Wire.h>
#include "../Adafruit-MCP23017-Arduino-Library/Adafruit_MCP23017.h"

class housecatOutputs
{
  private:
    static const uint8_t m_i2cBaseAddress = 4;
    static const uint8_t m_ioExpanderQuantity = 4;
    static const uint8_t m_ioExpanderPins = 16;

    Adafruit_MCP23017 m_ioExpander[m_ioExpanderQuantity];
    uint8_t m_mapping[m_ioExpanderQuantity][m_ioExpanderPins];
    bool m_output[m_ioExpanderQuantity * m_ioExpanderPins];

  public:
    housecatOutputs();
    void init();
    bool read(uint8_t output);
    void write(uint8_t output, bool state);
    void toggle(uint8_t output, bool toggle);
};

#endif
