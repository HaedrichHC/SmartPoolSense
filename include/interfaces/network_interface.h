#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include "Client.h"

class NetworkInterface {
    public:
        virtual ~NetworkInterface() = default;

        virtual void connect(const char* ssid, const char* password) = 0;

        virtual bool isConnected() = 0;

        virtual Client* getClient() = 0;
};


#endif