
#include "housecat.h"

housecatProtocol g_housecat_protocol;

housecatInputs g_housecat_inputs(g_housecat_protocol);
housecatOutputs g_housecat_outputs(g_housecat_protocol);
housecatAnalogOutputs g_housecat_analog_outputs(g_housecat_protocol);

HardwareSerial hcSerial1(1);
HardwareSerial hcSerial2(2);

housecatOneWire hcOneWire(HOUSECAT_ONEWIRE_PIN, HOUSECAT_ONEWIRE_SLEW_PIN);

static bool g_housecat_input_interrupt = false;


void IRAM_ATTR housecatInputsInterruptCallback()
{
  g_housecat_input_interrupt = true;
}

void housecatNetworkEvent(WiFiEvent_t event)
{
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      ETH.setHostname(HOUSECAT_HOSTNAME);
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      break;
    default:
      break;
  }
}




housecat::housecat()
{}

void housecat::ethernetInit()
{
  pinMode(HOUSECAT_ETH_ENABLE_PIN, OUTPUT);
  
  digitalWrite(HOUSECAT_ETH_ENABLE_PIN, LOW);
  delay(100);
  digitalWrite(HOUSECAT_ETH_ENABLE_PIN, HIGH);
  
  WiFi.onEvent(housecatNetworkEvent);
  ETH.begin();
  //ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_LAN87XX, ETH_CLOCK_GPIO0_IN);
}

void housecat::inputInterruptInit()
{
  pinMode(HOUSECAT_INPUT_INT_PIN, INPUT);
  attachInterrupt(HOUSECAT_INPUT_INT_PIN, housecatInputsInterruptCallback, FALLING);
}

void housecat::inputHandler()
{
  if (g_housecat_input_interrupt || (digitalRead(HOUSECAT_INPUT_INT_PIN) == LOW))
  {
    g_housecat_inputs.interruptCallback();
    g_housecat_input_interrupt = false;
  }
}

void housecat::heartbeatLed()
{
  static unsigned long timer = 0;
  static bool status_led = false;
  if ((millis() - timer) > 1000) //run every second;
  {
    timer = millis();
    digitalWrite(HOUSECAT_LED_PIN, status_led);
    status_led = !status_led;
  }
}

	
void housecat::modbusEnable()
{
	g_housecat_protocol.modbusEnable();
}
	
bool housecat::modbusEnabled()
{
	return g_housecat_protocol.modbusEnabled();
}


void housecat::mqttEnable()
{
	g_housecat_protocol.mqttEnable();
}

void housecat::mqttSetBaseTopic(String baseTopic)
{
	g_housecat_protocol.mqttSetBaseTopic(baseTopic);
}
	
void housecat::mqttSetBroker(IPAddress brokerIp, int brokerPort)
{
	g_housecat_protocol.mqttSetBroker(brokerIp, brokerPort);
}
	
void housecat::mqttSetBrokerCredentials(String username)
{
	g_housecat_protocol.mqttSetBrokerCredentials(username);
}
	
void housecat::mqttSetBrokerCredentials(String username, String password)
{
	g_housecat_protocol.mqttSetBrokerCredentials(username, password);
}

void housecat::udpEnable()
{
	g_housecat_protocol.udpEnable();
}
	
bool housecat::udpEnabled()
{
	return g_housecat_protocol.udpEnabled();
}
	
void housecat::udpSetAddress(int address)
{
	g_housecat_protocol.udpSetAddress(address);
}
	
void housecat::udpSetReceiver(int receivePort)
{
	g_housecat_protocol.udpSetReceiver(receivePort);
}

void housecat::udpSetSender(IPAddress sendIp, int sendPort)
{
	g_housecat_protocol.udpSetSender(sendIp, sendPort);
}

void housecat::init()
{
  Serial.begin(115200);
  
  ethernetInit();

  m_housecatOta.init();
  
  pinMode(HOUSECAT_LED_PIN, OUTPUT);
  pinMode(HOUSECAT_UART1_RTS_PIN, OUTPUT);
  pinMode(HOUSECAT_UART2_RTS_PIN, OUTPUT);
  //pinMode(HOUSECAT_ONEWIRE_SLEW_PIN, OUTPUT);

  digitalWrite(HOUSECAT_UART1_RTS_PIN, HIGH);

  hcSerial1.begin(9600, SERIAL_8N1, HOUSECAT_UART1_RX_PIN, HOUSECAT_UART1_TX_PIN);
  hcSerial2.begin(9600, SERIAL_8N1, HOUSECAT_UART2_RX_PIN, HOUSECAT_UART2_TX_PIN);

  Wire.begin((int) HOUSECAT_I2C_SDA_PIN, (int) HOUSECAT_I2C_SCL_PIN, (uint32_t) 400000);

  inputInterruptInit();

  delay(50);

  g_housecat_protocol.init();
  g_housecat_inputs.init();
  g_housecat_outputs.init();
  g_housecat_analog_outputs.init();

  Serial.println("Housecat: Started");
}

void housecat::poll()
{
  m_housecatOta.poll();

  heartbeatLed();
  inputHandler();

  g_housecat_protocol.poll();
	g_housecat_inputs.poll();
  g_housecat_outputs.poll();
  g_housecat_analog_outputs.poll();
}

void housecat::outputsTest()
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

void housecat::analogOutputsTest()
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

uint8_t onewire_address[8];



void housecat::scanI2c()
{
  byte error, address;
  int nDevices;
  Serial.println("I2C Scan...");
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
      Serial.print("Unknown error at address 0x");
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

void housecat::onewireTest() {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if (!hcOneWire.search(addr)) {
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

  if (housecatOneWire::crc8(addr, 7) != addr[7]) {
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
  Serial.print(housecatOneWire::crc8(data, 8), HEX);
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
