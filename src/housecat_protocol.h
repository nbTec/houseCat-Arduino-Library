#ifndef _HOUSECAT_PROTOCOL_H_
#define _HOUSECAT_PROTOCOL_H_

#include <ModbusIP_ESP8266.h>

#include <WiFi.h>
#include <MQTT.h>

enum enumProtocolBlindsState {blind_stop = 0, blind_up = 1, blind_down = 2, blind_open = 3, blind_closed = 4};

class housecatProtocol
{
  private:
    static const bool m_debug = false;

    bool m_modbusEnabled = false;
    bool m_mqttEnabled = false;
    bool m_udpEnabled = false;

    bool m_connected = false;

    ModbusIP m_modbusTcp;

    WiFiClient m_wifiClient;
    MQTTClient m_mqttClient;
    IPAddress m_mqttBrokerIpAddress = IPAddress(192, 168, 0, 1);
    int m_mqttBrokerPort = 1883;
    String m_mqttUsername, m_mqttPassword;

    void mqttConnect();
    static void mqttCallback(String &topic, String &payload);

    static bool m_mqttCallback;
    static String m_mqttReceivedTopic;
    static String m_mqttReceivedPayload;

    String m_mqttClientId = "housecat";
	String m_mqttBaseTopic = "/housecat";
    String m_mqttInputsTopic = "/input/";
    String m_mqttInputButtonShortSubTopic = "/button/short/";
    String m_mqttInputButtonLongSubTopic = "/button/long/";
    String m_mqttInputSensorSubTopic = "/sensor/";
    String m_mqttOutputsTopic = "/output/";
    String m_mqttDimmersTopic = "/dimmer/";
    String m_mqttDimmersStateSubTopic = "/state/";
    String m_mqttDimmersValueSubTopic = "/value/";

    static const uint8_t m_digital_inputs = 64;
    static const uint8_t m_digital_outputs = 64;
    static const uint8_t m_analog_outputs = 8;

    bool m_mqttInputsShort[m_digital_inputs + 1];
    bool m_mqttInputsLong[sizeof(m_mqttInputsShort)];
    uint8_t m_mqttOutputs[m_digital_outputs + 1];
    uint8_t m_mqttDimmerStates[m_analog_outputs + 1];
    uint8_t m_mqttDimmerValues[sizeof(m_mqttDimmerStates)];

    WiFiUDP m_udpSend;
    WiFiUDP m_udpReceive;
    int m_udpSendPort = 44444;
    IPAddress m_udpSendIpAddress = IPAddress(192, 168, 0, 1);
    int m_udpReceivePort = 55555;

    String m_udpDeviceName = "housecat";
    int m_udpDeviceAddress = 1;
    String m_udpSeparator = " ";
    String m_udpDigitalInputPrefix = "din";
    String m_udpDigitalOutputPrefix = "dout";
    String m_udpAnalogOutputPrefix = "aout";
    String m_udpDigitalInputString;
    String m_udpDigitalOutputString;
    String m_udpAnalogOutputString;
    bool m_udpOutputs[m_digital_outputs + 1];
    uint8_t m_udpAnalogOutputs[m_analog_outputs + 1];

  public:
    housecatProtocol();

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
 
    void writeInputRaw(uint8_t input, bool state);
    bool readOutputRaw(uint8_t output);
    int readAnalogOutputRaw(uint8_t output);

    bool addInputButton(uint8_t input);
    void writeInputButtonShort(uint8_t input, bool state);
    void writeInputButtonLong(uint8_t input, bool state);

    bool addInputSensor(uint8_t input);
    void writeInputSensor(uint8_t input, bool state);

    bool addOutput(uint8_t output);
    bool readOutput(uint8_t output);
    void writeOutput(uint8_t output, bool state);

    bool addBlind(uint8_t output);
    enumProtocolBlindsState readBlind(uint8_t output);
    void writeBlind(uint8_t output, enumProtocolBlindsState state);

    bool addDimmer(uint8_t dimmer);
    bool readDimmerState(uint8_t dimmer);
    void writeDimmerState(uint8_t dimmer, bool state);
    uint8_t readDimmerValue(uint8_t dimmer);
    void writeDimmerValue(uint8_t dimmer, uint8_t value);

    void poll();
};

#endif
