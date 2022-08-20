
#ifndef _HOUSECAT_H_
#define _HOUSECAT_H_

#include <HardwareSerial.h>
#include <ETH.h>

#include "housecat_protocol.h"

#include "housecat_onewire.h"

#include "housecat_inputs.h"
#include "housecat_input_button.h"
#include "housecat_input_sensor.h"

#include "housecat_outputs.h"
#include "housecat_output_relay.h"
#include "housecat_output_teleruptor.h"
#include "housecat_output_blinds.h"

#include "housecat_analog_outputs.h"
#include "housecat_analog_output_dimmer.h"

extern housecatProtocol g_housecat_protocol;

extern housecatInputs g_housecat_inputs;
extern housecatOutputs g_housecat_outputs;
extern housecatAnalogOutputs g_housecat_analog_outputs;

extern HardwareSerial hcSerial1;
extern HardwareSerial hcSerial2;

extern housecatOneWire hcOneWire;

#define housecat_hostname  "housecat"

#define HOUSECAT_ONEWIRE_SLEW_PIN  2
#define HOUSECAT_ONEWIRE_PIN       5
#define HOUSECAT_UART1_RTS_PIN     12
#define HOUSECAT_UART1_TX_PIN      14
#define HOUSECAT_UART2_TX_PIN      15
#define HOUSECAT_UART2_RTS_PIN     32
#define HOUSECAT_LED_PIN           33
#define HOUSECAT_CHAIN_OUT_PIN     33
#define HOUSECAT_CHAIN_IN_PIN      34
#define HOUSECAT_I2C_SCL_PIN       4
#define HOUSECAT_I2C_SDA_PIN       16
#define HOUSECAT_INPUT_INT_PIN     35
#define HOUSECAT_ETH_ENABLE_PIN    13
//#define HOUSECAT_ETH_POWER_PIN   13
#define HOUSECAT_UART2_RX_PIN      36
#define HOUSECAT_UART1_RX_PIN      39

class housecat
{
  private:
    void ethernetInit();
    void inputInterruptInit();
    void inputHandler();
    void heartbeatLed();
    
  public:
    housecat();
	
	void modbusEnable();
    bool modbusEnabled();
	
	void mqttEnable();
	void mqttSetBaseTopic(String baseTopic);
    void mqttSetBroker(IPAddress brokerIp, int brokerPort);
    void mqttSetBrokerCredentials(String username);
    void mqttSetBrokerCredentials(String username, String password);

    void udpEnable();
    bool udpEnabled();
    void udpSetAddress(int address);
    void udpSetReceiver(int receivePort);
    void udpSetSender(IPAddress sendIp, int sendPort);
	
    void init();
    void poll();
    void outputsTest();
    void analogOutputsTest();
    void scanI2c();
    void onewireTest();
};

#endif