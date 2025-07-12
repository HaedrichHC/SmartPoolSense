#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include <calib_storage.h>
#include <state_machine_interface.h>

class WebServer
{
public:
	explicit WebServer(StateMachineInterface &state_machine);
	~WebServer();

	void begin();
	void handleClient();

private:
	AsyncWebServer _server;
	StateMachineInterface &_state_machine;

	void handleRoot(AsyncWebServerRequest *request);
	void handleData(AsyncWebServerRequest *request);
	void handleRawData(AsyncWebServerRequest *request);
	void handleCSS(AsyncWebServerRequest *request);
	void handleJS(AsyncWebServerRequest *request);
	void handleImage(AsyncWebServerRequest *request);
	void handleResetTempCalib(AsyncWebServerRequest *request);
	void handleResetPhCalib(AsyncWebServerRequest *request);
	void handleSetTempCalib(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
	void handleSetPhCalib(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif