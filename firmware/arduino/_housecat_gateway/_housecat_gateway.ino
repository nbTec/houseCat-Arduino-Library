
#include <ETH.h>
#include "src/housecat/housecat.h"
#include "housecat_misc.h"


//Input buttons
housecatInputButton buttonHallway_1(protocol, inputs, 1);
housecatInputButton buttonHallway_2(protocol, inputs, 54);

//Output relays
housecatOutputRelay lightHallway(protocol, outputs, 1);

//Analog output dimmers
housecatAnalogOutputDimmer dimmerLivingroom(protocol, analog_outputs, 1, 10, 70);

//Blinds
housecatOutputBlinds blindLivingRoom_1(protocol, outputs, 23, 24, 30);


void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400000);
  delay(50);
  ethernetInit();


  protocol.init();
  inputs.init();
  outputs.init();
  analog_outputs.init();
  inputInterruptInit();



  //Setup output additional function
  lightHallway.enableAutoOff(30);
  lightHallway.enableMotion(10);
}


void loop()
{
  protocol.poll();
  outputs.poll();
  analog_outputs.poll();

  inputHandler();
  heartbeatLed();

  inputPolling();
  outputPolling();
}


void inputPolling()
{
  buttonHallway_1.poll();
  buttonHallway_2.poll();
}

void outputPolling()
{
  lightHallway.poll(buttonHallway_1.shortPress() or buttonHallway_2.shortPress(), false, buttonHallway_1.longPress());
  dimmerLivingroom.poll(buttonHallway_1.shortPress(), buttonHallway_1.longPress());
  blindLivingRoom_1.poll(buttonHallway_1.shortPress(), buttonHallway_1.longPress());
}
