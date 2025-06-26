#include <ota_update.h>


OTAupdate::OTAupdate() {}

OTAupdate::~OTAupdate() {}

void OTAupdate::begin(const char *hostname, const char *password) {
	_ArduinoOTA.setHostname(hostname);
	_ArduinoOTA.setPassword(password);
	_ArduinoOTA.begin();
}

void OTAupdate::handle() {
	_ArduinoOTA.handle();
}

void OTAupdate::enable_callbacks() {
	_ArduinoOTA.onStart([]() {
		Serial.println("Start of OTA update");
	});

	_ArduinoOTA.onEnd([]() {
		Serial.println("End of OTA update");
	});

	_ArduinoOTA.onProgress([](unsigned int progess, unsigned int total) {
		Serial.printf("Progess: %u%%\r", (progess / (total / 100)));
	});

	_ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		switch (error) {
			case OTA_AUTH_ERROR:
				Serial.println("Auth Failed");
				break;
			case OTA_BEGIN_ERROR:
				Serial.println("Begin Failed");
				break;
			case OTA_CONNECT_ERROR:
				Serial.println("Connect Failed");
				break;
			case OTA_RECEIVE_ERROR:
				Serial.println("Receive Error");
				break;
			case OTA_END_ERROR:
				Serial.println("End Failed");
				break;
		}
	});

}