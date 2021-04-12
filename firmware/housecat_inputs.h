
#include <Wire.h>
#include "Adafruit_MCP23017.h"

class housecatInputs
{
  private:
    static const uint8_t m_i2cBaseAddress = 0;
    static const uint8_t m_ioExpanderQuantity = 4;
    static const uint8_t m_ioExpanderPins = 16;
    
    Adafruit_MCP23017 m_ioExpander[m_ioExpanderQuantity];
    uint8_t m_mapping[m_ioExpanderQuantity][m_ioExpanderPins];
    bool m_input[m_ioExpanderQuantity * m_ioExpanderPins];
    

  public:
    housecatInputs();
    void init();
    void interruptCallback();
};
