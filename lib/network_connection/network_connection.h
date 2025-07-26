#ifndef NETWORK_ESP8266_H
#define NETWORK_ESP8266_H

#include <network_interface.h>
#include <ESP8266WiFi.h>

class NetworkConnection : public NetworkInterface
{
public:
    NetworkConnection();
    ~NetworkConnection();

    void connect(const char *ssid, const char *password) override;

    bool isConnected() override;

    Client &getClient() override;

private:
    WiFiClient _wifiClient;
};

#endif