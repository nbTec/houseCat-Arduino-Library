
#include "housecat.h"

housecat hc;

//Input buttons declaration (add poll to inputPolling function below)
//housecatInputButton button_name(input_number);
housecatInputButton button_1(1);
housecatInputButton button_2(2);
housecatInputButton button_3(3);
housecatInputButton button_4(4);
housecatInputButton button_5(5);
housecatInputButton button_6(6);
housecatInputButton button_7(7);
housecatInputButton button_8(8);
housecatInputButton button_9(9);
housecatInputButton button_10(10);
housecatInputButton button_11(11);
housecatInputButton button_12(12);
housecatInputButton button_13(13);
housecatInputButton button_14(14);
housecatInputButton button_15(15);
housecatInputButton button_16(16);
housecatInputButton button_17(17);
housecatInputButton button_18(18);
housecatInputButton button_19(19);
housecatInputButton button_20(20);
housecatInputButton button_21(21);
housecatInputButton button_22(22);
housecatInputButton button_23(23);
housecatInputButton button_24(24);
housecatInputButton button_25(25);
housecatInputButton button_26(26);
housecatInputButton button_27(27);
housecatInputButton button_28(28);
housecatInputButton button_29(29);
housecatInputButton button_30(30);
housecatInputButton button_31(31);
housecatInputButton button_32(32);
housecatInputButton button_33(33);
housecatInputButton button_34(34);
housecatInputButton button_35(35);
housecatInputButton button_36(36);
housecatInputButton button_37(37);
housecatInputButton button_38(38);
housecatInputButton button_39(39);
housecatInputButton button_40(40);
housecatInputButton button_41(41);
housecatInputButton button_42(42);
housecatInputButton button_43(43);
housecatInputButton button_44(44);
housecatInputButton button_45(45);
housecatInputButton button_46(46);
housecatInputButton button_47(47);
housecatInputButton button_48(48);
housecatInputButton button_49(49);
housecatInputButton button_50(50);
housecatInputButton button_51(51);
housecatInputButton button_52(52);
housecatInputButton button_53(53);
housecatInputButton button_54(54);
housecatInputButton button_55(55);
housecatInputButton button_56(56);
housecatInputButton button_57(57);
housecatInputButton button_58(58);
housecatInputButton button_59(59);
housecatInputButton button_60(60);
housecatInputButton button_61(61);
housecatInputButton button_62(62);
housecatInputButton button_63(63);
housecatInputButton button_64(64);

//Input sensor declaration (add poll to inputPolling function below)
//housecatInputSensor sensor_name(input_number, active_state);
//housecatInputSensor motionSensorHallway(9, LOW);


//Output relays declaration (add poll to outputPolling function below)
//housecatOutputRelay relay_name(output_number);
housecatOutputRelay lightLivingRoom(1);
housecatOutputRelay lightHallway(2);
housecatOutputRelay lightTest(17);

//Blinds declaration (add poll to outputPolling function below)
//housecatOutputBlinds blind_name(on_off_output_number, up_down_output_number, travel_time_seconds);
housecatOutputBlinds blindLivingRoom_1(18, 19, 15);


//Analog output dimmers declaration for 0-10V outputs (add poll to outputPolling function below)
//housecatAnalogOutputDimmer dimmer_name(analog_output_pin_number, start_percentage, default_percentage);
housecatAnalogOutputDimmer dimmerLivingroom(1, 10, 70);


void setup()
{
  //Enable one of the following three protocols by uncommenting them (only one can be used at the moment)

  hc.modbusEnable();

  //hc.mqttEnable();
  //hc.mqttSetBaseTopic("housecat");
  //hc.mqttSetBroker(IPAddress(192, 168, 1, 101), 1883);
  //hc.mqttSetBrokerCredentials("user", "password");

  //hc.udpEnable();
  //hc.udpSetAddress(1);
  //hc.udpSetReceiver(55555);
  //hc.udpSetSender(IPAddress(192, 168, 1, 101), 44444);

  hc.init();

  //Output additional settings
  lightHallway.enableAutoOff(30);
  lightHallway.enableMotion(10);

  blindLivingRoom_1.invertDirection(true);
}

void loop()
{
  hc.poll();
  
  //UDP is used to read/control the raw IO, so the internal IO linkage is disabled when using UDP
  if (!hc.udpEnabled())
  {
    inputPolling();
    outputPolling();
  }
  hc.outputsTest();
  hc.analogOutputsTest();
}

void inputPolling()
{
	button_1.poll();
	button_2.poll();
	button_3.poll();
	button_4.poll();
	button_5.poll();
	button_6.poll();
	button_7.poll();
	button_8.poll();
	button_9.poll();
	button_10.poll();
	button_11.poll();
	button_12.poll();
	button_13.poll();
	button_14.poll();
	button_15.poll();
	button_16.poll();
	button_17.poll();
	button_18.poll();
	button_19.poll();
	button_20.poll();
	button_21.poll();
	button_22.poll();
	button_23.poll();
	button_24.poll();
	button_25.poll();
	button_26.poll();
	button_27.poll();
	button_28.poll();
	button_29.poll();
	button_30.poll();
	button_31.poll();
	button_32.poll();
	button_33.poll();
	button_34.poll();
	button_35.poll();
	button_36.poll();
	button_37.poll();
	button_38.poll();
	button_39.poll();
	button_40.poll();
	button_41.poll();
	button_42.poll();
	button_43.poll();
	button_44.poll();
	button_45.poll();
	button_46.poll();
	button_47.poll();
	button_48.poll();
	button_49.poll();
	button_50.poll();
	button_51.poll();
	button_52.poll();
	button_53.poll();
	button_54.poll();
	button_55.poll();
	button_56.poll();
	button_57.poll();
	button_58.poll();
	button_59.poll();
	button_60.poll();
	button_61.poll();
	button_62.poll();
	button_63.poll();
	button_64.poll();
}

//Linking of inputs to output functions
void outputPolling()
{
  
}