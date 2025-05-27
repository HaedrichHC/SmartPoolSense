#ifndef NETWORK_ESP8266_H
#define NETWORK_ESP8266_H

#include <ESP8266WiFi.h>
#include "interfaces/network_interface.h"

class NetworkESP8266 : public NetworkInterface {
    public:

        NetworkESP8266();
        ~NetworkESP8266();

        void connect(const char* ssid, const char* password) override;

        bool isConnected() override;

        Client* getClient() override;
    
    private:
        WiFiClient _wifiClient;

};

#endif