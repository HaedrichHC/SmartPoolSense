#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "calib_storage.h"
#include "interfaces/state_machine_interface.h"

class WebServer {
	public:
		explicit WebServer(StateMachineInterface& state_machine);
		~WebServer();

		void begin();
		void handleClient();

	private:
		ESP8266WebServer _server;
		StateMachineInterface& _state_machine;

		void handleRoot();
		void handleData();
		void handleRawData();
		void handleCSS();
		void handleJS();
		void handleImage();
		void handleResetTempCalib();
		void handleResetPHCalib();
		void handleSetTempCalib();
		void handleSetPHCalib();
};

#endif