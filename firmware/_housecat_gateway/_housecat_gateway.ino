
#include <ETH.h>
#include "src/housecat/housecat.h"
#include "housecat_settings.h"


//Input buttons
housecatInputButton buttonHallway_1(inputs, 1);
housecatInputButton buttonHallway_2(inputs, 54);

//Output relays
housecatOutputRelay lightHallway(outputs, protocol, 23);

//Analog output dimmers
housecatAnalogOutputDimmer dimmerLivingroom(analog_outputs, 1, 1.0, 7.0);

//Blinds
housecatOutputBlinds blindLivingRoom_1(outputs, 8, 61, 30);


static bool eth_connected = false;
static bool input_interrupt = false;

//ModbusIP modbus_tcp;


void IRAM_ATTR inputs_interrupt_callback()
{
  input_interrupt = true;
}


void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400000);
  //scanI2c();
  delay(50);
  ethernetInit();

  //protocol.enableModbus();
  protocol.enableMqtt();
  protocol.init();
  
  inputs.init();
  outputs.init();
  analog_outputs.init();

  outputs.write(51, true);

  pinMode(INPUT_INT_PIN, INPUT);
  attachInterrupt(INPUT_INT_PIN, inputs_interrupt_callback, FALLING);

  //modbus_tcp.server();              // Act as Modbus TCP server
  //modbus_tcp.addReg(HREG(100));     // Add Holding register #100
  //modbus_tcp.addCoil(0);            // Add Coil #0
}


void loop()
{
  protocol.poll();
  
  inputHandler();
  //modbusHandler();
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
  lightHallway.poll(buttonHallway_1.longPress() or buttonHallway_1.shortPress() or buttonHallway_2.shortPress());
  dimmerLivingroom.poll(buttonHallway_1.shortPress(), buttonHallway_1.longPress());
  //blindLivingRoom_1.poll(buttonHallway_1.shortPress(), buttonHallway_1.longPress());
}



void heartbeatLed()
{
  static unsigned long timer = 0;
  static bool status_led = false;
  if ((millis() - timer) > 1000) //run every second;
  {
    timer = millis();
    digitalWrite(LED_PIN, status_led);
    status_led = !status_led;
  }
}

void inputHandler()
{
  if (input_interrupt || (digitalRead(INPUT_INT_PIN) == LOW))
  {
    //Serial.println("Input interrupt");
    inputs.interruptCallback();
    input_interrupt = false;
  }
}

/*
void modbusHandler()
{
  static unsigned long timer = 0;
  static bool coil = false;

  modbus_tcp.task();
  //outputs.write(8, modbus_tcp.Coil(0));  //Read coil and write to output


  if ((millis() - timer) > 1000) //run every second;
  {
    timer = millis();
    modbus_tcp.Coil(0, coil);
    coil = !coil;
  }
}
*/

void scanI2c()
{
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
}
