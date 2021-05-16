
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

void housecatProtocol::modbusEnable()
{
  m_modbusEnabled = true;
  m_mqttEnabled = false;
  m_udpEnabled = false;
}

void housecatProtocol::mqttEnable()
{
  m_modbusEnabled = false;
  m_mqttEnabled = true;
  m_udpEnabled = false;
}

void housecatProtocol::udpEnable()
{
  m_mqttEnabled = false;
  m_modbusEnabled = false;
  m_udpEnabled = true;
}

bool housecatProtocol::udpEnabled()
{
  return m_udpEnabled;
}

void housecatProtocol::udpSetAddress(int address)
{
  m_udpDeviceAddress = address;
}

void housecatProtocol::udpSetReceiver(int receivePort)
{
  m_udpReceivePort = receivePort;
}

void housecatProtocol::udpSetSender(IPAddress sendIp, int sendPort)
{
  m_udpSendIpAddress = sendIp;
  m_udpSendPort = sendPort;
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

  if(m_udpEnabled)
  {
    m_udpDigitalInputString =  m_udpDeviceName + String(m_udpDeviceAddress) + m_udpSeparator + m_udpDigitalInputPrefix;
    m_udpDigitalOutputString =  m_udpDeviceName + String(m_udpDeviceAddress) + m_udpSeparator + m_udpDigitalOutputPrefix;
    m_udpAnalogOutputString =  m_udpDeviceName + String(m_udpDeviceAddress) + m_udpSeparator + m_udpAnalogOutoutPrefix;

    m_udpSend.begin(m_udpSendPort);
    m_udpReceive.begin(m_udpReceivePort);
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
    String mqtt_output_topic = m_mqttOutputsTopic + output_number;
    m_mqttClient.subscribe(mqtt_output_topic);
  }

  for(int i = 1; i < sizeof(m_mqttDimmerStates); i++)
  {
    String dimmer_number = String(i);
    String mqtt_dimmer_topic = m_mqttDimmersTopic + dimmer_number + m_mqttDimmersStateSubTopic;
    m_mqttClient.subscribe(mqtt_dimmer_topic);
    mqtt_dimmer_topic = m_mqttDimmersTopic + dimmer_number + m_mqttDimmersValueSubTopic;
    m_mqttClient.subscribe(mqtt_dimmer_topic);
  }
}

void housecatProtocol::mqttCallback(String &topic, String &payload)
{
  m_mqttCallback = true;
  m_mqttReceivedTopic = topic;
  m_mqttReceivedPayload = payload;
}


void housecatProtocol::writeInputRaw(uint8_t input, bool state)
{
  if(m_udpEnabled)
  {
    String udp_string = m_udpDigitalInputString + String(input) + m_udpSeparator + String(state);
    int string_length = udp_string.length() + 1;
    uint8_t buffer[string_length];
    udp_string.toCharArray((char*) buffer, string_length);

    m_udpSend.beginPacket(m_udpSendIpAddress, m_udpSendPort);
    m_udpSend.write(buffer, string_length);
    m_udpSend.endPacket();
  }
}

bool housecatProtocol::readOutputRaw(uint8_t output)
{
  if(m_udpEnabled)
  {
    return m_udpOutputs[output];
  }

  return false;
}

int housecatProtocol::readAnalogOutputRaw(uint8_t output)
{
  if(m_udpEnabled)
  {
    return m_udpAnalogOutputs[output];
  }

  return 0;
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

bool housecatProtocol::addInputSensor(uint8_t input)
{
  bool ret = false;

  if(m_modbusEnabled)
  {
    ret = m_modbusTcp.addIsts(input);
  }

  return ret;
}

void housecatProtocol::writeInputSensor(uint8_t input, bool state)
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
    String mqtt_topic = m_mqttInputsTopic + input_number + m_mqttInputSensorSubTopic;
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
  bool ret = false;
  
  if(m_modbusEnabled)
  {
    ret = m_modbusTcp.addCoil(output);
    ret &= m_modbusTcp.addCoil(output + 1);
  }

  return ret;
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

bool housecatProtocol::addDimmer(uint8_t dimmer)
{
  bool ret = false;

  if(m_modbusEnabled)
  {
    ret = m_modbusTcp.addCoil(m_digital_outputs + 1 + dimmer);
    ret &= m_modbusTcp.addHreg(dimmer);
  }

  return ret;
}

bool housecatProtocol::readDimmerState(uint8_t dimmer)
{
  if(m_modbusEnabled)
  {
    return (bool) (m_modbusTcp.Coil(m_digital_outputs + 1 + dimmer) & 0x01);
  }

  if(m_mqttEnabled)
  {
    return m_mqttDimmerStates[dimmer];
  }

  return false;
}

void housecatProtocol::writeDimmerState(uint8_t dimmer, bool state)
{
  if(m_modbusEnabled)
  {
    m_modbusTcp.Coil(m_digital_outputs + 1 + dimmer, state);
  }

  if(m_mqttEnabled)
  {
    m_mqttDimmerStates[dimmer] = state;
    String dimmer_number = String(dimmer);
    String mqtt_topic = m_mqttDimmersTopic + dimmer_number + m_mqttDimmersStateSubTopic;
    m_mqttClient.publish(mqtt_topic, state ? "TRUE" : "FALSE");
  }
}

uint8_t housecatProtocol::readDimmerValue(uint8_t dimmer)
{
  if(m_modbusEnabled)
  {
    return (m_modbusTcp.Hreg(dimmer) & 0xFF);
  }

  if(m_mqttEnabled)
  {
    return m_mqttDimmerValues[dimmer];
  }

  return 0;
}

void housecatProtocol::writeDimmerValue(uint8_t dimmer, uint8_t value)
{
  if(m_modbusEnabled)
  {
    m_modbusTcp.Hreg(dimmer, value);
  }

  if(m_mqttEnabled)
  {
    m_mqttDimmerValues[dimmer] = value;
    String dimmer_number = String(dimmer);
    String mqtt_topic = m_mqttDimmersTopic + dimmer_number + m_mqttDimmersValueSubTopic;
    String mqtt_value = String(m_mqttDimmerValues[dimmer]);
    m_mqttClient.publish(mqtt_topic, mqtt_value);
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
  else if(m_mqttReceivedTopic.startsWith(m_mqttDimmersTopic))
  {
    String dimmer_substring = m_mqttReceivedTopic.substring(m_mqttDimmersTopic.length());
    if(m_mqttReceivedTopic.endsWith(m_mqttDimmersStateSubTopic))
    {
      String dimmer_number_substring = dimmer_substring.substring(0, dimmer_substring.length() - m_mqttDimmersStateSubTopic.length());
      int dimmer_number = dimmer_number_substring.toInt();
      if((0 < dimmer_number) && (dimmer_number < sizeof(m_mqttDimmerStates)))
      {
        if(m_mqttReceivedPayload == "TRUE")
        {
          m_mqttDimmerStates[dimmer_number] = true;
        }
        else if(m_mqttReceivedPayload == "FALSE")
        {
          m_mqttDimmerStates[dimmer_number] = false;
        }
      }
    }
    else if(m_mqttReceivedTopic.endsWith(m_mqttDimmersValueSubTopic))
    {
      String dimmer_number_substring = dimmer_substring.substring(0, dimmer_substring.length() - m_mqttDimmersStateSubTopic.length());
      int dimmer_number = dimmer_number_substring.toInt();
      if((0 < dimmer_number) && (dimmer_number < sizeof(m_mqttDimmerStates)))
      {
        uint8_t dimmer_value = m_mqttReceivedPayload.toInt();
        if((dimmer_value > 0) && (dimmer_value <= 100))
        {
          m_mqttDimmerValues[dimmer_number] = dimmer_value;
        }
      }
    }
  }
    m_mqttCallback = false;
  }

  if(m_udpEnabled)
  {
    uint8_t udp_buffer[50];
    String udp_string;
    memset(udp_buffer, 0, 50);
    m_udpReceive.parsePacket();

    if(m_udpReceive.read(udp_buffer, 50) > 0)
    {
      udp_string = (char*) udp_buffer;

      if(udp_string.startsWith(m_udpDigitalOutputString))
      {
        uint8_t string_seperator_index = udp_string.indexOf(m_udpSeparator, m_udpDigitalOutputString.length());
        String output_index_string = udp_string.substring(m_udpDigitalOutputString.length(), string_seperator_index);
        String output_state_string = udp_string.substring(string_seperator_index + 1);

        uint8_t output_index = output_index_string.toInt();
        uint8_t output_state = output_state_string.toInt();

        if((output_index < sizeof(m_udpOutputs)) && (output_state <= 1))
        {
          m_udpOutputs[output_index] = output_state;
          /*Serial.print("UDP: Digital output: ");
          Serial.print(output_index);
          Serial.print(" State: ");
          Serial.println(output_state);*/
        }
      }
      else if(udp_string.startsWith(m_udpAnalogOutputString))
      {
        uint8_t string_seperator_index = udp_string.indexOf(m_udpSeparator, m_udpAnalogOutputString.length());
        String output_index_string = udp_string.substring(m_udpAnalogOutputString.length(), string_seperator_index);
        String output_state_string = udp_string.substring(string_seperator_index + 1);

        uint8_t output_index = output_index_string.toInt();
        uint8_t output_state = output_state_string.toInt();

        if(output_index < sizeof(m_udpAnalogOutputs))
        {
          m_udpAnalogOutputs[output_index] = output_state;
          /*Serial.print("UDP: Analog output: ");
          Serial.print(output_index);
          Serial.print(" State: ");
          Serial.println(output_state);*/
        }
      }
    }
  }
}
