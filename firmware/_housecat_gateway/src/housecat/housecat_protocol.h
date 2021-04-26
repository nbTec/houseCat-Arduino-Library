
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

    ModbusIP m_modbusTcp;


    WiFiClient m_wifiClient;
    MQTTClient m_mqttClient;

    IPAddress m_mqttBrokerIpAddress = IPAddress(192, 168, 1, 101);
    int m_mqttBrokerPort = 1883;

    void mqttConnect();
    static void mqttCallback(String &topic, String &payload);

    static bool m_mqttCallback;
    static String m_mqttReceivedTopic;
    static String m_mqttReceivedPayload;

    String m_mqttOutputsTopic = "/housecat/output";
    bool m_mqttOutputs[65];


  public:
    housecatProtocol();
    void enableModbus();
    void enableMqtt();
    void init();
    void poll();

    void addInput(uint8_t output);
    bool readInput(uint8_t output);
    void writeInput(uint8_t output, bool state);

    void addOutput(uint8_t output);
    bool readOutput(uint8_t output);
    void writeOutput(uint8_t output, bool state);

    void addRegister(uint8_t output);
    uint16_t readRegister(uint8_t output);
    void writeRegister(uint8_t output, uint16_t state);
};

#endif
