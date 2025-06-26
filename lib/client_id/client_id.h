#ifndef CLIENT_ID_H
#define CLIENT_ID_H

#include <client_id_interface.h>
#include <ESP8266WiFi.h>

class ClientId : public ClientIdInterface {
	public:
		ClientId();
		~ClientId();
		
		void generate(const char* id_name) override;

		const char* get_id() const override;

	private:
		const char* _client_id = "";
};

#endif