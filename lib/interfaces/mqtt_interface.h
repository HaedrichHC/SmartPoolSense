#ifndef MQTT_INTERFACE_H
#define MQTT_INTERFACE_H

#include <cstdint>

class MqttInterface
{
public:
	virtual ~MqttInterface() = default;

	virtual void begin(const char *server, uint16_t port, uint32_t chip_id, const char *client_name) = 0;
	virtual bool connect() = 0;
	virtual bool connected() = 0;
	virtual void publish(const char *topic, const char *payload) = 0;
	virtual void loop() = 0;
	virtual void set_callback(void (*callback)(char *, uint8_t *, unsigned int)) = 0;
	virtual void disconnect() = 0;
};

#endif