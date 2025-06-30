#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <network_interface.h>
#include <config.h>
#include <PubSubClient.h>

class MqttHandler 
{
public:
    MqttHandler();

    ~MqttHandler();

    void begin(NetworkInterface *network, const char *server, uint16_t port, uint32_t chip_id);
        
	bool connect();

    bool connected();

    void publish(const char *topic, const char *payload);

    void loop();

    void setCallback(void (*callback)(char *, byte *, unsigned int));

	void disconnect();

private:
	const char* _client_id = "";
	PubSubClient _mqtt_client;

	const char *generate_client_id(uint32_t chip_id);
};

#endif