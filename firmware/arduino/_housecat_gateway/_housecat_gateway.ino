
#include <ETH.h>
#include "src/housecat/housecat.h"
#include "_housecat_gateway.h"


//Input buttons declaration (add poll to inputPolling function below)
//housecatInputButton button_name(protocol, inputs, input_pin_number);
housecatInputButton buttonLivingroom_1(protocol, inputs, 1);
housecatInputButton buttonLivingroom_2(protocol, inputs, 2);
housecatInputButton buttonHallway_1(protocol, inputs, 3);
housecatInputButton buttonHallway_2(protocol, inputs, 4);
housecatInputButton buttonBlind_1_Up(protocol, inputs, 5);
housecatInputButton buttonBlind_1_Down(protocol, inputs, 6);
housecatInputButton buttonAllOff(protocol, inputs, 7);
housecatInputButton buttonPanic(protocol, inputs, 8);

//Input sensor declaration (add poll to inputPolling function below)
//housecatInputSensor button_name(protocol, inputs, input_pin_number);
housecatInputSensor motionSensorHallway(protocol, inputs, 9);


//Output relays declaration (add poll to outputPolling function below)
//housecatOutputRelay relay_name(protocol, outputs, output_pin_number);
housecatOutputRelay lightLivingRoom(protocol, outputs, 1);
housecatOutputRelay lightHallway(protocol, outputs, 2);

//Blinds declaration (add poll to outputPolling function below)
//housecatOutputBlinds blind_name(protocol, outputs, output_pin_numer_onoff, output_pin_numer_updown, travel_time_s);
housecatOutputBlinds blindLivingRoom_1(protocol, outputs, 3, 4, 30);


//Analog output dimmers declaration for 0-10V outputs (add poll to outputPolling function below)
//housecatAnalogOutputDimmer dimmer_name(protocol, analog_outputs, analog_output_pin_number, start_percentage, default_percentage);
housecatAnalogOutputDimmer dimmerLivingroom(protocol, analog_outputs, 1, 10, 70);


void setup()
{
  housecatInit();
  ethernetInit();

  //Enable one of the following three protocols by uncommenting them (only one can be used at the moment)

  protocol.modbusEnable();

  //protocol.mqttEnable();
  //protocol.mqttSetBroker(IPAddress(192, 168, 1, 101), 1883);
  //protocol.mqttSetBrokerCredentials("user", "password");

  //protocol.udpEnable();
  //protocol.udpSetAddress(1);
  //protocol.udpSetSender(IPAddress(192, 168, 1, 101), 44444);
  //protocol.udpSetReceiver(55555);

  //Initialisation
  protocol.init();
  inputs.init();
  outputs.init();
  analog_outputs.init();
  inputInterruptInit();

  //Optional tests
  //onewireTest();
  analogOutputsTest();

  //Output additional settings
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

  //outputsTest();

  //UDP is used to read/control the raw IO, so the internal IO linkage is disabled when using UDP
  if (!protocol.udpEnabled())
  {
    inputPolling();
    outputPolling();
  }
}

void inputPolling()
{
  buttonLivingroom_1.poll();
  buttonLivingroom_2.poll();
  buttonHallway_1.poll();
  buttonHallway_2.poll();
  buttonBlind_1_Up.poll();
  buttonBlind_1_Down.poll();
  buttonAllOff.poll();
  buttonPanic.poll();

  motionSensorHallway.poll();
}

//Linking of inputs to output functions
void outputPolling()
{
  //relay_name(toggle_input, reset_input, panic_input, motion_input)
  lightLivingRoom.poll(buttonLivingroom_1.shortPress());
  lightHallway.poll(buttonHallway_1.shortPress() or buttonHallway_2.shortPress(), buttonAllOff.longPress(), buttonPanic.longPress(), motionSensorHallway.pulse());
  blindLivingRoom_1.poll(buttonBlind_1_Up.shortPress(), buttonBlind_1_Down.shortPress());
  dimmerLivingroom.poll(buttonLivingroom_2.shortPress(), buttonLivingroom_2.longPress());
}
