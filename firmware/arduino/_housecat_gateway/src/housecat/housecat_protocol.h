
#ifndef _HOUSECAT_PROTOCOL_H_
#define _HOUSECAT_PROTOCOL_H_

#include <ModbusIP_ESP8266.h>

#include <WiFi.h>
#include <MQTT.h>


class housecatProtocol
{
  private:
    bool m_modbusEnabled = false;
    bool m_mqttEnabled = false;
    bool m_UDPEnabled = false;

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
    String m_mqttDimmerTopic = "/housecat/dimmer/";
    
    bool m_mqttInputsShort[65];
    bool m_mqttInputsLong[65];
    uint8_t m_mqttOutputs[65];
    uint8_t m_mqttDimmerValues[65];
    uint8_t m_mqttDimmerStates[65];


  public:
    housecatProtocol();

    void enableModbus();

    void enableMqtt();
    void mqttSetBroker(IPAddress brokerIp, int brokerPort);
    void mqttSetBrokerCredentials(String username);
    void mqttSetBrokerCredentials(String username, String password);

    void enableUDP();

    void init();
    void poll();

    void addInputButton(uint8_t input);
    void writeInputButtonShort(uint8_t input, bool state);
    void writeInputButtonLong(uint8_t input, bool state);

    void addOutput(uint8_t output);
    bool readOutput(uint8_t output);
    void writeOutput(uint8_t output, bool state);

    void addRegister(uint8_t output);
    uint16_t readRegister(uint8_t output);
    void writeRegister(uint8_t output, uint16_t state);
};

#endif
