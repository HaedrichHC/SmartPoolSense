#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include "interfaces/network_interface.h"

class MqttHandler {
    public:

        MqttHandler();

        ~MqttHandler();

        void begin(NetworkInterface* network, const char* server, uint16_t port, const char* client_id);

        bool connect();

        bool connected();

        void publish(const char* topic, const char* payload);

        void loop();

        void setCallback(void (*callback)(char*, byte*, unsigned int));

		    void disconnect();

    private:
		  const char* _client_id = "";
		  PubSubClient _mqtt_client;
};

#endif