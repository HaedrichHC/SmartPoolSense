#include "mqtt_handler.h"

MqttHandler::MqttHandler(NetworkInterface &network)
    : _wifi_client(network), _mqtt_client(_wifi_client.getClient()) {}

MqttHandler::~MqttHandler() {}

void MqttHandler::begin(const char *server, uint16_t port, uint32_t chip_id, const char *client_name)
{
    _mqtt_client.setServer(server, port);
    _client_id = generate_client_id(chip_id, client_name);
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

void MqttHandler::publish(const char *topic, const char *payload)
{
    if (_mqtt_client.connected())
    {
        _mqtt_client.publish(topic, payload);
    }
}

void MqttHandler::loop()
{
    _mqtt_client.loop();
}

void MqttHandler::set_callback(void (*callback)(char *, uint8_t *, unsigned int))
{
    _mqtt_client.setCallback(callback);
}

void MqttHandler::disconnect()
{
    _mqtt_client.disconnect();
}

const char *MqttHandler::generate_client_id(uint32_t chip_id, const char *client_name)
{
    static char buffer[50];
    snprintf(buffer, sizeof(buffer), "%s_%u", client_name, chip_id);
    return buffer;
}