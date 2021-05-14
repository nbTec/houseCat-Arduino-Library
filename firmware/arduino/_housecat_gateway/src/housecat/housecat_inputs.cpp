
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_inputs.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatInputs::housecatInputs(housecatProtocol &protocol): m_protocol(protocol)
{
  m_mapping[0][0] = 7;
  m_mapping[0][1] = 39;
  m_mapping[0][2] = 6;
  m_mapping[0][3] = 38;
  m_mapping[0][4] = 5;
  m_mapping[0][5] = 37;
  m_mapping[0][6] = 4;
  m_mapping[0][7] = 36;
  m_mapping[0][8] = 32;
  m_mapping[0][9] = 0;
  m_mapping[0][10] = 33;
  m_mapping[0][11] = 1;
  m_mapping[0][12] = 34;
  m_mapping[0][13] = 2;
  m_mapping[0][14] = 35;
  m_mapping[0][15] = 3;
  m_mapping[1][0] = 15;
  m_mapping[1][1] = 47;
  m_mapping[1][2] = 14;
  m_mapping[1][3] = 46;
  m_mapping[1][4] = 9;
  m_mapping[1][5] = 41;
  m_mapping[1][6] = 8;
  m_mapping[1][7] = 40;
  m_mapping[1][8] = 42;
  m_mapping[1][9] = 10;
  m_mapping[1][10] = 43;
  m_mapping[1][11] = 11;
  m_mapping[1][12] = 44;
  m_mapping[1][13] = 12;
  m_mapping[1][14] = 45;
  m_mapping[1][15] = 13;
  m_mapping[2][0] = 23;
  m_mapping[2][1] = 55;
  m_mapping[2][2] = 22;
  m_mapping[2][3] = 54;
  m_mapping[2][4] = 21;
  m_mapping[2][5] = 53;
  m_mapping[2][6] = 20;
  m_mapping[2][7] = 52;
  m_mapping[2][8] = 48;
  m_mapping[2][9] = 16;
  m_mapping[2][10] = 49;
  m_mapping[2][11] = 17;
  m_mapping[2][12] = 50;
  m_mapping[2][13] = 18;
  m_mapping[2][14] = 51;
  m_mapping[2][15] = 19;
  m_mapping[3][0] = 31;
  m_mapping[3][1] = 63;
  m_mapping[3][2] = 30;
  m_mapping[3][3] = 62;
  m_mapping[3][4] = 25;
  m_mapping[3][5] = 57;
  m_mapping[3][6] = 24;
  m_mapping[3][7] = 56;
  m_mapping[3][8] = 58;
  m_mapping[3][9] = 26;
  m_mapping[3][10] = 59;
  m_mapping[3][11] = 27;
  m_mapping[3][12] = 60;
  m_mapping[3][13] = 28;
  m_mapping[3][14] = 61;
  m_mapping[3][15] = 29;

}

void housecatInputs::init()
{
  for (int i = 0; i < m_ioExpanderQuantity; i++)
  {
    m_ioExpander[i].begin(i + m_i2cBaseAddress);
    m_ioExpander[i].setupInterrupts(true, true, LOW);
    for (int j = 0; j < m_ioExpanderPins; j++)
    {
      m_ioExpander[i].pinMode(j, INPUT);
      m_ioExpander[i].setupInterruptPin(j, CHANGE);
    }
  }
}

void housecatInputs::interruptCallback()
{
  uint8_t interrupt_pin, value;
  for (int i = 0; i < m_ioExpanderQuantity; i++)
  {
    interrupt_pin = m_ioExpander[i].getLastInterruptPin();
    if (interrupt_pin != MCP23017_INT_ERR)
    {
      value = m_ioExpander[i].getLastInterruptPinValue();
      m_input[m_mapping[i][interrupt_pin]] = value;

      m_protocol.writeInputRaw(m_mapping[i][interrupt_pin], (bool) value);

      /*Serial.print("IO Expander: ");
      Serial.print(i);
      Serial.print(", Input: ");
      Serial.print(m_mapping[i][interrupt_pin]);
      Serial.print(", Value: ");
      Serial.println(value);*/
    }
  }
}

bool housecatInputs::read(uint8_t input)
{
  if ((input > 0) && (input <= ((m_ioExpanderQuantity * m_ioExpanderPins) + 1)))
    return m_input[input - 1];
  else
    return false;
}
