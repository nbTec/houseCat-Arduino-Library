#ifndef _HOUSECAT_PROTOCOL_H_
#define _HOUSECAT_PROTOCOL_H_

#include <ModbusIP_ESP8266.h>

#include <WiFi.h>
#include <MQTT.h>

enum enumProtocolBlindsState {blind_stop = 0, blind_up = 1, blind_down = 2, blind_open = 3, blind_closed = 4};

class housecatProtocol
{
  private:
    bool m_modbusEnabled = false;
    bool m_mqttEnabled = false;
    bool m_udpEnabled = false;

    bool m_connected = false;

    ModbusIP m_modbusTcp;


    WiFiClient m_wifiClient;
    MQTTClient m_mqttClient;
    IPAddress m_mqttBrokerIpAddress;
    int m_mqttBrokerPort;
    String m_mqttUsername, m_mqttPassword;

    void mqttConnect();
    static void mqttCallback(String &topic, String &payload);

    static bool m_mqttCallback;
    static String m_mqttReceivedTopic;
    static String m_mqttReceivedPayload;

    String m_mqttClientId = "housecat";
    String m_mqttInputsTopic = "/housecat/input/";
    String m_mqttInputButtonShortSubTopic = "/short/";
    String m_mqttInputButtonLongSubTopic = "/long/";
    String m_mqttOutputsTopic = "/housecat/output/";
    String m_mqttDimmersTopic = "/housecat/dimmer/";
    String m_mqttDimmersStateSubTopic = "/state/";
    String m_mqttDimmersValueSubTopic = "/value/";
    
    bool m_mqttInputsShort[65];
    bool m_mqttInputsLong[sizeof(m_mqttInputsShort)];
    uint8_t m_mqttOutputs[65];
    uint8_t m_mqttDimmerStates[9];
    uint8_t m_mqttDimmerValues[sizeof(m_mqttDimmerStates)];


    WiFiUDP m_udpSend;
    WiFiUDP m_udpReceive;
    int m_udpSendPort = 44444;
    int m_udpReceivePort = 55555;
    IPAddress m_udpSendIpAddress = IPAddress(192, 168, 1, 101);

    String m_udpDeviceName = "housecat";
    int m_udpDeviceAddress = 1;
    String m_udpDigitalInputPrefix = "din";
    String m_udpDigitalOutoutPrefix = "dout";
    String m_udpAnalogOutoutPrefix = "aout";


  public:
    housecatProtocol();

    void enableModbus();

    void enableMqtt();
    void mqttSetBroker(IPAddress brokerIp, int brokerPort);
    void mqttSetBrokerCredentials(String username);
    void mqttSetBrokerCredentials(String username, String password);

    void enableUDP();

    void init();
 
    void writeInput(uint8_t input, bool state);

    bool addInputButton(uint8_t input);
    void writeInputButtonShort(uint8_t input, bool state);
    void writeInputButtonLong(uint8_t input, bool state);

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
