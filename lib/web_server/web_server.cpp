#include <web_server.h>

WebServer::WebServer(StateMachineInterface &state_machine)
	: _server(80), _state_machine(state_machine) {}

WebServer::~WebServer() {}

void WebServer::begin()
{

	_server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
			   { handleRoot(request); });
	_server.on("/css/style.css", HTTP_GET, [this](AsyncWebServerRequest *request)
			   { handleCSS(request); });
	_server.on("/js/script.js", HTTP_GET, [this](AsyncWebServerRequest *request)
			   { handleJS(request); });
	_server.on("/img/Logo.svg", HTTP_GET, [this](AsyncWebServerRequest *request)
			   { handleImage(request); });
	_server.on("/data", HTTP_GET, [this](AsyncWebServerRequest *request)
			   { handleData(request); });
	_server.on("/rawdata", HTTP_GET, [this](AsyncWebServerRequest *request)
			   { handleRawData(request); });
	_server.on("/calibrate/temp/reset", HTTP_GET, [this](AsyncWebServerRequest *request)
			   { handleResetTempCalib(request); });
	_server.on("/calibrate/ph/reset", HTTP_GET, [this](AsyncWebServerRequest *request)
			   { handleResetPhCalib(request); });

	_server.on("/calibrate/temp/set", HTTP_POST, [this](AsyncWebServerRequest *request) {}, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
			   { handleSetTempCalib(request, data, len, index, total); });

	_server.on("/calibrate/ph/set", HTTP_POST, [this](AsyncWebServerRequest *request) {}, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
			   { handleSetPhCalib(request, data, len, index, total); });

	_server.onNotFound([](AsyncWebServerRequest *request)
					   { request->send(404, "text/plain", "Not Found"); });

	_server.begin();
}

void WebServer::handleRoot(AsyncWebServerRequest *request)
{
	if (LittleFS.exists("/index.html"))
	{
		request->send(LittleFS, "/index.html", "text/html");
	}
	else
	{
		request->send(500, "text/plain", "HTML not found");
	}
}

void WebServer::handleData(AsyncWebServerRequest *request)
{
	JsonDocument doc;
	String data;

	SensorData sensor_data = _state_machine.get_sensor_data();

	doc["temp"] = sensor_data.temp_value;
	doc["ph"] = sensor_data.ph_value;

	serializeJson(doc, data);

	request->send(200, "application/json", data);
}

void WebServer::handleRawData(AsyncWebServerRequest *request)
{
	JsonDocument doc;
	String data;

	SensorData sensor_data = _state_machine.get_sensor_data();

	doc["temp_raw"] = sensor_data.temp_raw;
	doc["temp_stable_status"] = sensor_data.temp_stable_status;
	doc["ph_raw"] = sensor_data.ph_raw;
	doc["ph_stable_status"] = sensor_data.ph_stable_status;

	serializeJson(doc, data);

	request->send(200, "application/json", data);
}

void WebServer::handleCSS(AsyncWebServerRequest *request)
{
	if (LittleFS.exists("/css/style.css"))
	{
		request->send(LittleFS, "/css/style.css", "text/css");
	}
	else
	{
		request->send(500, "text/plain", "CSS not found");
	}
}

void WebServer::handleJS(AsyncWebServerRequest *request)
{
	if (LittleFS.exists("/js/script.js"))
	{
		request->send(LittleFS, "/js/script.js", "application/javascript");
	}
	else
	{
		request->send(500, "text/plain", "JavaScript not found");
	}
}

void WebServer::handleImage(AsyncWebServerRequest *request)
{
	if (LittleFS.exists("/img/Logo.svg"))
	{
		request->send(LittleFS, "/img/Logo.svg", "image/svg+xml");
	}
	else
	{
		request->send(500, "text/plain", "Image not found");
	}
}

void WebServer::handleResetTempCalib(AsyncWebServerRequest *request)
{
	_state_machine.set_temp_calibration(10.4, 1640, 49.0, 4400);
	request->send(200, "text/plain", "Temperature calibration reset");
}

void WebServer::handleResetPhCalib(AsyncWebServerRequest *request)
{
	_state_machine.set_ph_calibration(7.0, 2880);
	request->send(200, "text/plain", "pH calibration reset");
}

void WebServer::handleSetTempCalib(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
	if (index == 0)
	{
		String body = String((char *)data).substring(0, len);
		JsonDocument doc;

		DeserializationError error = deserializeJson(doc, body);

		if (error)
		{
			request->send(400, "text/plain", "Invalid JSON");
			return;
		}

		_state_machine.set_temp_calibration(
			doc["tempValue1"], doc["tempRaw1"],
			doc["tempValue2"], doc["tempRaw2"]);

		request->send(200, "text/plain", "OK");
	}
}

void WebServer::handleSetPhCalib(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
	if (index == 0)
	{
		String body = String((char *)data).substring(0, len);
		JsonDocument doc;

		DeserializationError error = deserializeJson(doc, body);

		if (error)
		{
			request->send(400, "text/plain", "Invalid JSON");
			return;
		}

		_state_machine.set_ph_calibration(
			doc["phValue"], doc["phRaw"]);

		request->send(200, "text/plain", "OK");
	}
}