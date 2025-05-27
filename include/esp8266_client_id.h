#ifndef ESP8266_CLIENT_ID_H
#define ESP8266_CLIENT_ID_H

#include "client_id_generator.h"
#include <ESP8266WiFi.h>

class ESP8266ClientID : public ClientIDGenerator {
	public:
		const char* generate() const override{
			static char buffer[50];
			snprintf(buffer, sizeof(buffer), "PoolMonitor_%u", ESP.getChipId());
			return buffer;
		}

};

extern ESP8266ClientID client_id;

ESP8266ClientID client_id;

#endif