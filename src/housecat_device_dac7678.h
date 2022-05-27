
#ifndef HOUSECAT_DEVICE_DAC7678_H
#define HOUSECAT_DEVICE_DAC7678_H

#include "Arduino.h"
#include <Wire.h>

class housecatDeviceDac7678
{
    public:
        housecatDeviceDac7678();
        
		void begin(uint8_t newDeviceAddress, bool enableInternalReference);
        void writeDAC(uint16_t value, uint8_t DAC);
		
    private:
        uint8_t m_deviceAddress;
		
		bool m_enableInternalReference = false;
		
		byte m_referenceConfig = 0x80;
        byte m_CA = 0x30;

        static unsigned createMask(unsigned a, unsigned b);
};

#endif