#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <mqtt_interface.h>
#include <network_interface.h>
#include <PubSubClient.h>

class MqttHandler : public MqttInterface
{
public:
    MqttHandler(NetworkInterface &network);

    ~MqttHandler();

    void begin(const char *server, uint16_t port, uint32_t chip_id, const char *client_name) override;

    bool connect() override;

    bool connected() override;

    void publish(const char *topic, const char *payload) override;

    void loop() override;

    void set_callback(void (*callback)(char *, uint8_t *, unsigned int)) override;

    void disconnect() override;

private:
    NetworkInterface &_wifi_client;
    PubSubClient _mqtt_client;
    const char *_client_id = "";
    const char *generate_client_id(uint32_t chip_id, const char *client_name);
};

#endif