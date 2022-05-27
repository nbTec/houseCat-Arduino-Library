
#include "housecat_device_dac7678.h"

unsigned housecatDeviceDac7678::createMask(unsigned a, unsigned b)
{
   unsigned r = 0;
   for (unsigned i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}


housecatDeviceDac7678::housecatDeviceDac7678()
{
}

void housecatDeviceDac7678::begin(uint8_t newDeviceAddress, bool enableInternalReference)
{
  m_deviceAddress = newDeviceAddress;
  m_enableInternalReference = enableInternalReference;
  
  //Configure internal reference (enable)
  Wire.beginTransmission(m_deviceAddress);
  Wire.write(m_referenceConfig);
  Wire.write(0x00);
  Wire.write(m_enableInternalReference << 4);
  Wire.endTransmission();
}


void housecatDeviceDac7678::writeDAC(uint16_t value, uint8_t DAC){
  uint16_t MSDB2, LSDB2;
  
  unsigned r = createMask(0, 3);
  LSDB2 = value & r;
  LSDB2 = LSDB2 << 4;
  
  unsigned x = createMask(3, 11);
  MSDB2 = value & x;
  MSDB2 = MSDB2 >> 4;
  
  //Wire.begin();
  Wire.beginTransmission(m_deviceAddress); //hex 48, B1001000
  Wire.write(m_CA + DAC); //CA byte
  Wire.write(MSDB2); //MSDB
  Wire.write(LSDB2); //LSDB
  Wire.endTransmission();
}
