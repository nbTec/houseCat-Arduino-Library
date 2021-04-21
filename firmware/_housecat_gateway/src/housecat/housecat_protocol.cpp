
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_protocol.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatProtocol::housecatProtocol()
{

}

void housecatProtocol::init()
{
  m_modbus_tcp.server();
}

void housecatProtocol::addOutput(uint8_t output)
{
  m_modbus_tcp.addCoil(output);
}

bool housecatProtocol::readOutput(uint8_t output)
{
  return m_modbus_tcp.Coil(output);
}

void housecatProtocol::writeOutput(uint8_t output, bool state)
{
  m_modbus_tcp.Coil(output, state);
}

void housecatProtocol::poll()
{
  m_modbus_tcp.task();
}
