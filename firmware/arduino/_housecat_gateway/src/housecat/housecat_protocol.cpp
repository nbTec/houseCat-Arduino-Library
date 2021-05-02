
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
  m_udpEnabled = false;
}

void housecatProtocol::enableMqtt()
{
  m_modbusEnabled = false;
  m_mqttEnabled = true;
  m_udpEnabled = false;
}

void housecatProtocol::enableUDP()
{
  m_mqttEnabled = false;
  m_modbusEnabled = false;
  m_udpEnabled = true;
}

void housecatProtocol::mqttSetBroker(IPAddress brokerIp, int brokerPort)
{
  m_mqttBrokerIpAddress = brokerIp;
  m_mqttBrokerPort = brokerPort;
}

void housecatProtocol::mqttSetBrokerCredentials(String username)
{
  m_mqttUsername = username;
}

void housecatProtocol::mqttSetBrokerCredentials(String username, String password)
{
  m_mqttUsername = username;
  m_mqttPassword = password;
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
  char mqtt_client_id[m_mqttClientId.length() + 1];
  char mqtt_username[m_mqttUsername.length() + 1];
  char mqtt_password[m_mqttPassword.length() + 1];

  m_mqttClientId.toCharArray(mqtt_client_id, sizeof(mqtt_client_id));
  m_mqttUsername.toCharArray(mqtt_username, sizeof(mqtt_username));
  m_mqttPassword.toCharArray(mqtt_password, sizeof(mqtt_password));

  if(m_mqttUsername.isEmpty())
    m_mqttClient.connect(mqtt_client_id, false);
  else if(m_mqttPassword.isEmpty())
    m_mqttClient.connect(mqtt_client_id, mqtt_username, false);
  else
    m_mqttClient.connect(mqtt_client_id, mqtt_username, mqtt_password, false);
    
  
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

bool housecatProtocol::addInputButton(uint8_t input)
{
  bool ret = false;

  if(m_modbusEnabled)
  {
    ret = m_modbusTcp.addIsts(input);
    ret &= m_modbusTcp.addIsts(input + 64);
  }

  return ret;
}

void housecatProtocol::writeInputButtonShort(uint8_t input, bool state)
{
  if(m_modbusEnabled)
  {
    if(state)
      m_modbusTcp.Ists(input, 1);
    else
      m_modbusTcp.Ists(input, 0);
  }

  if(m_mqttEnabled)
  {
    m_mqttInputsShort[input] = state;
    String input_number = String(input);
    String mqtt_topic = m_mqttInputsTopic + input_number + m_mqttInputButtonShortSubTopic;
    m_mqttClient.publish(mqtt_topic, state ? "TRUE" : "FALSE");
  }
}

void housecatProtocol::writeInputButtonLong(uint8_t input, bool state)
{
  if(m_modbusEnabled)
  {
    if(state)
      m_modbusTcp.Ists(input + 64, 1);
    else
      m_modbusTcp.Ists(input + 64, 0);
  }

  if(m_mqttEnabled)
  {
    m_mqttInputsLong[input] = state;
    String input_number = String(input);
    String mqtt_topic = m_mqttInputsTopic + input_number + m_mqttInputButtonLongSubTopic;
    m_mqttClient.publish(mqtt_topic, state ? "TRUE" : "FALSE");
  }
}

bool housecatProtocol::addOutput(uint8_t output)
{
  if(m_modbusEnabled)
  {
    return m_modbusTcp.addCoil(output);
  }

  /*
  if(m_mqttEnabled)
  {
    String output_number = String(output);
    String mqtt_topic = m_mqttOutputsTopic + output_number;
    return m_mqttClient.subscribe(mqtt_topic);
  }
  */

  return false;
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

bool housecatProtocol::addBlind(uint8_t output)
{
  if(m_modbusEnabled)
  {
    return m_modbusTcp.addCoil(output);
    return m_modbusTcp.addCoil(output + 1);
  }
  return false;
}

enumProtocolBlindsState housecatProtocol::readBlind(uint8_t output)
{
  if(m_modbusEnabled)
  {
    if(m_modbusTcp.Coil(output))
    {
      return blind_down;
    }
    else
    {
      return blind_up;
    }
  }
  if(m_mqttEnabled)
  {
    return static_cast<enumProtocolBlindsState>(m_mqttOutputs[output]);
  }
  return static_cast<enumProtocolBlindsState>(0);
}

void housecatProtocol::writeBlind(uint8_t output, enumProtocolBlindsState state)
{
  if (m_modbusEnabled)
  {
    switch(state)
    {
      case blind_stop:
        m_modbusTcp.Coil(output, true);
      break;
      case blind_up:

      break;
      case blind_down:
 
      break;
      case blind_open:

      break;
      case blind_closed:

      break;
      default:
      break;
    }
  }
  
  if(m_mqttEnabled)
  {
    m_mqttOutputs[output] = static_cast<uint8_t>(state);
    String output_number = String(output);
    String mqtt_topic = m_mqttOutputsTopic + output_number;
    String blind_state;
    switch(state)
    {
      case blind_stop:
        blind_state = "STOP";
      break;
      case blind_up:
        blind_state = "UP";
      break;
      case blind_down:
        blind_state = "DOWN";
      break;
      case blind_open:
        blind_state = "OPEN";
      break;
      case blind_closed:
        blind_state = "CLOSED";
      break;
      default:
      break;
    }
    m_mqttClient.publish(mqtt_topic, blind_state);
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
  Serial.print("MQTT: ");
  Serial.print(m_mqttReceivedTopic);
  Serial.print(", ");
  Serial.println(m_mqttReceivedPayload);

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
      else if(m_mqttReceivedPayload == "STOP")
      {
        m_mqttOutputs[output_number] = blind_stop;
      }
      else if(m_mqttReceivedPayload == "UP")
      {
        m_mqttOutputs[output_number] = blind_up;
      }
      else if(m_mqttReceivedPayload == "DOWN")
      {
        m_mqttOutputs[output_number] = blind_down;
      }
    }
  }
    m_mqttCallback = false;
  }
}
