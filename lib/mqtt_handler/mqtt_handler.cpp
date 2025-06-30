#include <mqtt_handler.h>

MqttHandler::MqttHandler() {}

MqttHandler::~MqttHandler() {}

void MqttHandler::begin(NetworkInterface* network, const char* server, uint16_t port, uint32_t chip_id)
{
    _mqtt_client.setServer(server, port);
    _mqtt_client.setClient(*network->getClient());
    _client_id = generate_client_id(chip_id);
}

bool MqttHandler::connect()
{
    Serial.print("Connecting to MQTT-Broker...");
    if (_mqtt_client.connect(_client_id)) 
	{
        Serial.println("connected");
        return true;
    } 
	else 
	{
        Serial.print("Failure, rc= ");
		Serial.println(_mqtt_client.state());
        return false;
    }
}

bool MqttHandler::connected()
{
    return _mqtt_client.connected();
}

void MqttHandler::publish(const char* topic, const char* payload)
{
    if (_mqtt_client.connected()) {
        _mqtt_client.publish(topic, payload);
    }
}

void MqttHandler::loop()
{
    _mqtt_client.loop();
}

void MqttHandler::setCallback(void (*callback)(char*, byte*, unsigned int))
{
    _mqtt_client.setCallback(callback);
}

void MqttHandler::disconnect()
{
	_mqtt_client.disconnect();
}

const char *MqttHandler:: generate_client_id(uint32_t chip_id)
{
	static char buffer[50];
    snprintf(buffer, sizeof(buffer), "%s_%u", MQTT_CLIENT_ID, chip_id);
    return buffer;
}