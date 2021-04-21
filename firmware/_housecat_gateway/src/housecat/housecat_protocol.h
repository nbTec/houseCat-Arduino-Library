
#ifndef _HOUSECAT_PROTOCOL_H_
#define _HOUSECAT_PROTOCOL_H_

#include <ModbusIP_ESP8266.h>

class housecatProtocol
{
  private:
    ModbusIP m_modbus_tcp;


  public:
    housecatProtocol();
    void init();
    void poll();
    void addOutput(uint8_t output);
    bool readOutput(uint8_t output);
    void writeOutput(uint8_t output, bool state);
};

#endif
