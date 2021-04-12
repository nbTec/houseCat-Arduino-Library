
#include <Wire.h>
#include "Adafruit_MCP23017.h"

class housecat_outputs
{
  private:
    uint8_t m_state;
    Adafruit_MCP23017 m_io_expander[4];

  public:
    housecat_outputs();
    void init();
};
