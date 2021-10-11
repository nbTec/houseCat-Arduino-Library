
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

class housecat
{
  private:
    void ethernetInit();
    void inputInterruptInit();
    void inputHandler();
    void heartbeatLed();
    
  public:
    housecat();
    void init();
    void poll();
    void outputsTest();
    void analogOutputsTest();
    void scanI2c();
    void onewireTest();
};

#endif