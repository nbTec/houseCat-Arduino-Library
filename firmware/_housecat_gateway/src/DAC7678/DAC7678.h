
#ifndef DAC7678_h
#define DAC7678_h

#include "Arduino.h"
#include <Wire.h>

class DAC7678
{
    public:
        DAC7678();
        
		void begin(uint8_t newDeviceAddress, bool enableInternalReference);
        void writeDAC(uint16_t value, uint8_t DAC);
		
    private:
        uint8_t m_deviceAddress;
		
		bool m_enableInternalReference = false;
		
		byte m_referenceConfig = 0x80;
        byte m_CA = 0x30;
};

#endif