
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_analog_outputs.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatAnalogOutputs::housecatAnalogOutputs(housecatProtocol &protocol): m_protocol(protocol)
{
  m_mapping[0] = 2;
  m_mapping[1] = 0;
  m_mapping[2] = 3;
  m_mapping[3] = 1;
  m_mapping[4] = 7;
  m_mapping[5] = 5;
  m_mapping[6] = 6;
  m_mapping[7] = 4;
}

void housecatAnalogOutputs::init()
{
  m_dac.begin(i2cAddress, true);
}

float housecatAnalogOutputs::readFullScale()
{
  return m_fullScale;
}

float housecatAnalogOutputs::read(uint8_t output)
{
  if((output > 0) && (output <= m_dacOutputs))
    return m_output[output - 1];
  else
    return 0;
}

void housecatAnalogOutputs::write(uint8_t output, float value)
{
 if((output > 0) && (output <= m_dacOutputs))
 {
    output -= 1;
    if(m_output[output] != value)
    {
      uint16_t dac_value = (uint16_t) (((value / (float) m_externalGain) / m_dacFullScale ) * (float) m_dacResolution);
      m_dac.writeDAC(dac_value, m_mapping[output]);
      m_output[output] = value;
    } 
 }
}

void housecatAnalogOutputs::poll()
{
  if(m_protocol.udpEnabled())
  {
    for(uint8_t i = 1; i <= m_dacOutputs; i++)
    {
      uint8_t output_raw_state = m_protocol.readAnalogOutputRaw(i);
      float output_raw_voltage = ((float) output_raw_state / 25.5); //255 -> 10V

      if(read(i) != output_raw_voltage)
      {
        write(i, output_raw_voltage);
      }
    }
  }
}
