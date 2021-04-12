
#include <Wire.h>
#include "Adafruit_MCP23017.h"

class housecat_inputs
{
  private:
    Adafruit_MCP23017 m_io_expander[4];

  public:
    housecat_inputs();
    void init();
    void interrupt_callback();
};
