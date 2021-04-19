#include "DAC7678.h"

DAC7678 DAC(0x4A);

void setup(void)
{
  Serial.begin(9600);
  delay(1000);

  Serial.println("Arduino Connected");
}

void loop(void)
{
  for(int i=0; i<=4095; i++){
    DAC.writeDAC(i, 0);
    delay(10);
  }
}
