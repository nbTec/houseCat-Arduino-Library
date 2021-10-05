
#include <ETH.h>
#include "src/housecat/housecat.h"
#include "_housecat_gateway.h"


//Input buttons declaration (add poll to inputPolling function below)
//housecatInputButton button_name(protocol, inputs, input_pin_number);
housecatInputButton buttonLivingroom_1(1);
housecatInputButton buttonLivingroom_2(2);
housecatInputButton buttonHallway_1(3);
housecatInputButton buttonHallway_2(4);
housecatInputButton buttonBlind_1_Up(5);
housecatInputButton buttonBlind_1_Down(6);
housecatInputButton buttonAllOff(7);
housecatInputButton buttonPanic(8);

//Input sensor declaration (add poll to inputPolling function below)
//housecatInputSensor button_name(input_pin_number);
housecatInputSensor motionSensorHallway(9);


//Output relays declaration (add poll to outputPolling function below)
//housecatOutputRelay relay_name(output_pin_number);
housecatOutputRelay lightLivingRoom(1);
housecatOutputRelay lightHallway(2);

//Blinds declaration (add poll to outputPolling function below)
//housecatOutputBlinds blind_name(output_pin_numer_onoff, output_pin_numer_updown, travel_time_s);
housecatOutputBlinds blindLivingRoom_1(3, 4, 30);


//Analog output dimmers declaration for 0-10V outputs (add poll to outputPolling function below)
//housecatAnalogOutputDimmer dimmer_name(analog_output_pin_number, start_percentage, default_percentage);
housecatAnalogOutputDimmer dimmerLivingroom(1, 10, 70);


void setup()
{
  housecatInit();
  ethernetInit();

  //Enable one of the following three protocols by uncommenting them (only one can be used at the moment)

  g_housecat_protocol.modbusEnable();

  //g_housecat_protocol.mqttEnable();
  //g_housecat_protocol.mqttSetBroker(IPAddress(192, 168, 1, 101), 1883);
  //g_housecat_protocol.mqttSetBrokerCredentials("user", "password");

  //g_housecat_protocol.udpEnable();
  //g_housecat_protocol.udpSetAddress(1);
  //g_housecat_protocol.udpSetSender(IPAddress(192, 168, 1, 101), 44444);
  //g_housecat_protocol.udpSetReceiver(55555);

  //Initialisation
  g_housecat_protocol.init();
  g_housecat_inputs.init();
  g_housecat_outputs.init();
  g_housecat_analog_outputs.init();
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
  g_housecat_protocol.poll();
  g_housecat_outputs.poll();
  g_housecat_analog_outputs.poll();
  inputHandler();
  heartbeatLed();

  //outputsTest();

  //UDP is used to read/control the raw IO, so the internal IO linkage is disabled when using UDP
  if (!g_housecat_protocol.udpEnabled())
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
