
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_outputs.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatOutputs::housecatOutputs()
{
  m_mapping[0][0] = 0;
  m_mapping[0][1] = 1;
  m_mapping[0][2] = 2;
  m_mapping[0][3] = 3;
  m_mapping[0][4] = 4;
  m_mapping[0][5] = 5;
  m_mapping[0][6] = 6;
  m_mapping[0][7] = 7;
  m_mapping[0][8] = 39;
  m_mapping[0][9] = 38;
  m_mapping[0][10] = 37;
  m_mapping[0][11] = 36;
  m_mapping[0][12] = 35;
  m_mapping[0][13] = 34;
  m_mapping[0][14] = 33;
  m_mapping[0][15] = 32;
  m_mapping[1][0] = 40;
  m_mapping[1][1] = 41;
  m_mapping[1][2] = 42;
  m_mapping[1][3] = 43;
  m_mapping[1][4] = 44;
  m_mapping[1][5] = 45;
  m_mapping[1][6] = 46;
  m_mapping[1][7] = 47;
  m_mapping[1][8] = 15;
  m_mapping[1][9] = 14;
  m_mapping[1][10] = 13;
  m_mapping[1][11] = 12;
  m_mapping[1][12] = 11;
  m_mapping[1][13] = 10;
  m_mapping[1][14] = 9;
  m_mapping[1][15] = 8;
  m_mapping[2][0] = 16;
  m_mapping[2][1] = 17;
  m_mapping[2][2] = 18;
  m_mapping[2][3] = 19;
  m_mapping[2][4] = 20;
  m_mapping[2][5] = 21;
  m_mapping[2][6] = 22;
  m_mapping[2][7] = 23;
  m_mapping[2][8] = 55;
  m_mapping[2][9] = 54;
  m_mapping[2][10] = 53;
  m_mapping[2][11] = 52;
  m_mapping[2][12] = 51;
  m_mapping[2][13] = 50;
  m_mapping[2][14] = 49;
  m_mapping[2][15] = 48;
  m_mapping[3][0] = 56;
  m_mapping[3][1] = 57;
  m_mapping[3][2] = 58;
  m_mapping[3][3] = 59;
  m_mapping[3][4] = 60;
  m_mapping[3][5] = 61;
  m_mapping[3][6] = 62;
  m_mapping[3][7] = 63;
  m_mapping[3][8] = 31;
  m_mapping[3][9] = 30;
  m_mapping[3][10] = 29;
  m_mapping[3][11] = 28;
  m_mapping[3][12] = 27;
  m_mapping[3][13] = 26;
  m_mapping[3][14] = 25;
  m_mapping[3][15] = 24;
}

void housecatOutputs::init()
{
  uint16_t current_value = 0;
  
  for (int i = 0; i < m_ioExpanderQuantity; i++)
  {
    m_ioExpander[i].begin(i + m_i2cBaseAddress);
    current_value = m_ioExpander[i].readOLATAB();
	  //current_value = m_ioExpander[i].readOutputsAB();
    for (int j = 0; j < m_ioExpanderPins; j++)
    {
	  m_output[m_mapping[i][j]] = ((current_value >> j) & 0x01); //Restore current state of outputs
      m_ioExpander[i].pinMode(j, OUTPUT);

    }
  }
}

bool housecatOutputs::read(uint8_t output)
{
  return m_output[output - 1];
}

void housecatOutputs::write(uint8_t output, bool state)
{
  for (int i = 0; i < m_ioExpanderQuantity; i++)
  {
    for (int j = 0; j < m_ioExpanderPins; j++)
    {
      if (m_mapping[i][j] == (output - 1))
      {
        if (m_output[m_mapping[i][j]] != state)
        {
          m_ioExpander[i].digitalWrite(j, state);
          m_output[m_mapping[i][j]] = state;
        }
      }
    }
  }
}

void housecatOutputs::toggle(uint8_t output, bool toggle)
{
  if (toggle)
  {
    for (int i = 0; i < m_ioExpanderQuantity; i++)
    {
      for (int j = 0; j < m_ioExpanderPins; j++)
      {
        if (m_mapping[i][j] == (output - 1))
        {
          m_output[m_mapping[i][j]] = !m_output[m_mapping[i][j]];
          m_ioExpander[i].digitalWrite(j, m_output[m_mapping[i][j]]);
        }
      }
    }
  }
}
