
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_protocol.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

bool housecatProtocol::m_mqttCallback;
String housecatProtocol::m_mqttReceivedTopic;
String housecatProtocol::m_mqttReceivedPayload;

housecatProtocol::housecatProtocol()
{
}

void housecatProtocol::enableModbus()
{
  m_modbusEnabled = true;
  m_mqttEnabled = false;
}

void housecatProtocol::enableMqtt()
{
  m_mqttEnabled = true;
  m_modbusEnabled = false;
}

void housecatProtocol::init()
{
  if(m_modbusEnabled)
  {
    m_modbusTcp.server();
  }
  
  if(m_mqttEnabled)
  {
    m_mqttClient.begin(m_mqttBrokerIpAddress, m_mqttBrokerPort, m_wifiClient);
    m_mqttClient.onMessage(mqttCallback);
    mqttConnect();
  }
}

void housecatProtocol::mqttConnect()
{
  
  m_mqttClient.subscribe("/housecat/status");
  m_mqttClient.publish("/housecat/status", "Hello");

  for(int i = 1; i < sizeof(m_mqttOutputs); i++)
  {
    String output_number = String(i);
    String mqtt_topic = m_mqttOutputsTopic + output_number;
    m_mqttClient.subscribe(mqtt_topic);
  }
}

void housecatProtocol::mqttCallback(String &topic, String &payload)
{
  m_mqttCallback = true;
  m_mqttReceivedTopic = topic;
  m_mqttReceivedPayload = payload;
}

void housecatProtocol::addOutput(uint8_t output)
{
  if(m_modbusEnabled)
  {
    m_modbusTcp.addCoil(output);
  }

  if(m_mqttEnabled)
  {
    String output_number = String(output);
    String mqtt_topic = m_mqttOutputsTopic + output_number;
    m_mqttClient.subscribe(mqtt_topic);
  }
}

bool housecatProtocol::readOutput(uint8_t output)
{
  if(m_modbusEnabled)
  {
    return m_modbusTcp.Coil(output);
  }

  if(m_mqttEnabled)
  {
    return m_mqttOutputs[output];
  }

  return false;
}

void housecatProtocol::writeOutput(uint8_t output, bool state)
{
  if(m_modbusEnabled)
  {
    m_modbusTcp.Coil(output, state);
  }

  if(m_mqttEnabled)
  {
    m_mqttOutputs[output] = state;
    String output_number = String(output);
    String mqtt_topic = m_mqttOutputsTopic + output_number;
    m_mqttClient.publish(mqtt_topic, state ? "TRUE" : "FALSE");
  }
}

void housecatProtocol::poll()
{
  if(m_modbusEnabled)
  {
    m_modbusTcp.task();
  }

  if(m_mqttEnabled)
  {
    m_mqttClient.loop();

    if (!m_mqttClient.connected()) 
    {
      mqttConnect();
    }
  }

  if(m_mqttCallback)
  {
    /*
    Serial.print("MQTT: ");
    Serial.print(m_mqttReceivedTopic);
    Serial.print(", ");
    Serial.println(m_mqttReceivedPayload);
    */

  if(m_mqttReceivedTopic.startsWith(m_mqttOutputsTopic))
  {
    String output_number_substring = m_mqttReceivedTopic.substring(m_mqttOutputsTopic.length());
    int output_number = output_number_substring.toInt();
    if((0 < output_number) && (output_number < sizeof(m_mqttOutputs)))
    {
      if(m_mqttReceivedPayload == "TRUE")
      {
        m_mqttOutputs[output_number] = true;
      }
      else if(m_mqttReceivedPayload == "FALSE")
      {
        m_mqttOutputs[output_number] = false;
      }
    }
  }
    m_mqttCallback = false;
  }
}
