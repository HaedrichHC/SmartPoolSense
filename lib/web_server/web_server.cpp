#include <web_server.h>

WebServer::WebServer(StateMachineInterface& state_machine)
	: _server(80), _state_machine(state_machine) {}

WebServer::~WebServer() {}

void WebServer::begin() {

	_server.on("/", HTTP_GET, [this]() {handleRoot ();});
	_server.on("/css/style.css", HTTP_GET, [this]() {handleCSS();});
	_server.on("/js/script.js", HTTP_GET, [this]() {handleJS();});
	_server.on("/img/Logo.svg", HTTP_GET, [this]() {handleImage();});
	_server.on("/data", HTTP_GET, [this]() {handleData();});
	_server.on("/rawdata", HTTP_GET, [this]() {handleRawData();});
	_server.on("/calibrate/temp/reset", HTTP_GET, [this] {handleResetTempCalib();});
	_server.on("/calibrate/ph/reset", HTTP_GET, [this] {handleResetPHCalib();});
	_server.on("/calibrate/temp/set", HTTP_POST, [this]() {handleSetTempCalib();});
	_server.on("/calibrate/ph/set", HTTP_POST, [this]() {handleSetPHCalib();});

	_server.begin();
}

void WebServer::handleClient() {
	_server.handleClient();
}

void WebServer::handleRoot() {
	File file = LittleFS.open("/index.html", "r");
	if (file) {
		_server.streamFile(file, "text/html");
		file.close();
	} else {
		_server.send(500, "text/plain", "HTML not found");
	}	
}

void WebServer::handleData() {
	JsonDocument doc;
	String data;

	doc["temp"] = _state_machine.get_sensor_data().temp_value;
	doc["ph"] = _state_machine.get_sensor_data().ph_value;
	
	serializeJson(doc, data);

	_server.send(200, "application/json", data);
}

void WebServer::handleRawData() {
	JsonDocument doc;
	String data;

	doc["temp_raw"] = _state_machine.get_sensor_data().temp_raw;
	doc["temp_stable_status"] = _state_machine.get_sensor_data().temp_stable_status;
	doc["ph_raw"] = _state_machine.get_sensor_data().ph_raw;
	doc["ph_stable_status"] = _state_machine.get_sensor_data().ph_stable_status;

	serializeJson(doc, data);

	_server.send(200, "application/json", data);
}

void WebServer::handleCSS() {
	File file = LittleFS.open("/css/style.css", "r");
	if (file) {
		_server.streamFile(file, "text/css");
		file.close();
	} else {
		_server.send(500, "text/plain", "CSS not found");
	}	
}

void WebServer::handleJS() {
	File file = LittleFS.open("/js/script.js", "r");
	if (file) {
		_server.streamFile(file, "application/javascript");
		file.close();
	} else {
		_server.send(500, "text/plain", "JavaScript not found");
	}	
}

void WebServer::handleImage() {
	File file = LittleFS.open("/img/Logo.svg", "r");
	if (file) {
		_server.streamFile(file, "image/svg+xml");
		file.close();
	} else {
		_server.send(500, "text/plain", "Image not found");
	}
}

void WebServer::handleResetTempCalib() {
	_state_machine.set_temp_calibration(10.4, 1640, 49.0, 4400);
	_server.send(200, "text/plain", "OK");
}

void WebServer::handleResetPHCalib() {
	_state_machine.set_ph_calibration(7.0, 2880);
	_server.send(200, "text/plain", "OK");
}

void WebServer::handleSetTempCalib() {
	JsonDocument doc;
	String body = _server.arg("plain");

	if (deserializeJson(doc, body)) {
		_server.send(400, "text/plain", "Invalid JSON");
		return;
	}

	_state_machine.set_temp_calibration(
		doc["tempValue1"], doc["tempRaw1"],
		doc["tempValue2"], doc["tempRaw2"]
	);

	_server.send(200, "text/plain", "OK");
}

void WebServer::handleSetPHCalib() {
	JsonDocument doc;
	String body = _server.arg("plain");

	if (deserializeJson(doc, body)) {
		_server.send(400, "text/plain", "Invalid JSON");
		return;
	}

	_state_machine.set_ph_calibration(
		doc["phValue"], doc["phRaw"]
	);

	_server.send(200, "text/plain", "OK");
}