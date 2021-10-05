
#include <HardwareSerial.h>

#define ONEWIRE_SLEW_PIN  2
#define ONEWIRE_PIN       5
#define UART1_RTS_PIN     12
#define UART1_TX_PIN      14
#define UART2_TX_PIN      15
#define UART2_RTS_PIN     32
#define LED_PIN           33
#define CHAIN_OUT_PIN     33
#define CHAIN_IN_PIN      34
#define I2C_SCL_PIN       4
#define I2C_SDA_PIN       16
#define INPUT_INT_PIN     35
#define ETH_ENABLE_PIN    13
//#define ETH_POWER_PIN   13
#define UART2_RX_PIN      36
#define UART1_RX_PIN      39

HardwareSerial hcSerial1(1);
HardwareSerial hcSerial2(2);

OneWire  hcOneWire(ONEWIRE_PIN, ONEWIRE_SLEW_PIN);
uint8_t onewire_address[8];

static bool eth_connected = false;
static bool input_interrupt = false;

void housecatInit(void)
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(UART1_RTS_PIN, OUTPUT);
  pinMode(UART2_RTS_PIN, OUTPUT);
  //pinMode(ONEWIRE_SLEW_PIN, OUTPUT);

  digitalWrite(UART1_RTS_PIN, HIGH);

  Serial.begin(115200);
  hcSerial1.begin(9600, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  hcSerial2.begin(9600, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400000);
  delay(50);
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

    //hcSerial1.write("Test");
    //digitalWrite(ONEWIRE_SLEW_PIN, HIGH);
    //delay(1);
    //digitalWrite(ONEWIRE_SLEW_PIN, LOW);
  }
}

void IRAM_ATTR inputs_interrupt_callback()
{
  input_interrupt = true;
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
    g_housecat_inputs.interruptCallback();
    input_interrupt = false;
  }
}



void outputsTest()
{
  static unsigned long timer = 0;
  static bool state = false;

  if ((millis() - timer) > 1000) //run every second;
  {
    timer = millis();
    for (int i = 1; i <= 64; i++)
      g_housecat_outputs.write(i, state);

    state = not state;
  }
}

void analogOutputsTest()
{
  g_housecat_analog_outputs.write(1,1.0);
  g_housecat_analog_outputs.write(2,2.0);
  g_housecat_analog_outputs.write(3,3.0);
  g_housecat_analog_outputs.write(4,4.0);
  g_housecat_analog_outputs.write(5,5.0);
  g_housecat_analog_outputs.write(6,6.0);
  g_housecat_analog_outputs.write(7,7.0);
  g_housecat_analog_outputs.write(8,8.0);
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

void onewireTest(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if ( !hcOneWire.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    hcOneWire.reset_search();
    delay(250);
    return;
  }

  Serial.print("ROM =");
  for ( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }
  Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  }

  hcOneWire.reset();
  hcOneWire.select(addr);
  hcOneWire.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = hcOneWire.reset();
  hcOneWire.select(addr);
  hcOneWire.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = hcOneWire.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
}
