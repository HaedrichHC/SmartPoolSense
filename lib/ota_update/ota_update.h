#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <ArduinoOTA.h>

class OTAupdate {
	public:

		OTAupdate();

		~OTAupdate();

		void begin(const char *hostname, const char *password);

		void handle();

		void enable_callbacks();

	private:
		ArduinoOTAClass _ArduinoOTA;
};

#endif