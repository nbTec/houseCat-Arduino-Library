#include <ETH.h>
#include <ModbusIP_ESP8266.h>

#define LED_PIN         33

#define I2C_SCL_PIN     4
#define I2C_SDA_PIN     16

#define INPUT_INT_PIN   35

#include "housecat_outputs.h"
#include "housecat_inputs.h"
#include "housecat_input_button.h"

housecatOutputs outputs;
housecatInputs inputs;

static bool eth_connected = false;
static bool input_interrupt = false;

ModbusIP modbus_tcp;


void IRAM_ATTR inputs_interrupt_callback()
{
  input_interrupt = true;
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400000);
  //scan_i2c();
  delay(50);
  ethernetInit();

  outputs.init();
  inputs.init();
  pinMode(INPUT_INT_PIN, INPUT);
  attachInterrupt(INPUT_INT_PIN, inputs_interrupt_callback, FALLING);
  outputs.set(24, HIGH);
  
  modbus_tcp.server();              // Act as Modbus TCP server
  modbus_tcp.addReg(HREG(100));     // Add Holding register #100
  modbus_tcp.addCoil(0);            // Add Coil #0
}

void loop()
{
  static unsigned long timer = 0;
  static bool status_led = false;
  
  modbus_tcp.task();

  if (input_interrupt || (digitalRead(INPUT_INT_PIN) == LOW))
  {
    Serial.println("Input interrupt");
    inputs.interruptCallback();
    input_interrupt = false;
  }

  outputs.set(8, modbus_tcp.Coil(0));  //Read coil

  if((millis() - timer) > 1000) //run every second;
  {
    timer = millis();
    digitalWrite(LED_PIN, status_led);
    //modbus_tcp.Coil(0, status_led); //Write coil
    status_led = !status_led;
  }
}




void scan_i2c()
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
