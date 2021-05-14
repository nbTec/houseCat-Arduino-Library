
#define LED_PIN         33

#define I2C_SCL_PIN     4
#define I2C_SDA_PIN     16

#define INPUT_INT_PIN   35

#define ETH_ENABLE_PIN  13
//#define ETH_POWER_PIN   13

static bool eth_connected = false;
static bool input_interrupt = false;

void IRAM_ATTR inputs_interrupt_callback()
{
  input_interrupt = true;
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

void inputInterruptInit()
{
  pinMode(INPUT_INT_PIN, INPUT);
  attachInterrupt(INPUT_INT_PIN, inputs_interrupt_callback, FALLING);
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
