#include <network_connection.h>
#include <ad_converter.h>
#include <state_machine.h>
#include <mqtt_handler.h>
#include <ota_update.h>
#include <ph_sensor.h>
#include <temp_sensor.h>
#include <web_server.h>
#include <calib_storage.h>
#include <stability_checker.h>

#include <config.h>
#include <secret.h>

CalibStorage calib_storage(STORAGE_PATH);
AdConverter ad_converter;
StabilityChecker temp_stability(100, 20), ph_stability(100, 20);
TempSensor temp_sensor(ad_converter, calib_storage, temp_stability);
PhSensor ph_sensor(ad_converter, calib_storage, ph_stability);
NetworkConnection network;
MqttHandler mqtt_handler;
StateMachine state_machine(mqtt_handler, ph_sensor, temp_sensor);
WebServer web_server(state_machine);
OTAupdate ota_update;

void setup()
{
	Serial.begin(115200);
	SPI.begin();

	ad_converter.begin(ADC_CS_PIN);
	network.connect(WIFI_SSID, WIFI_PASSWORD);
	mqtt_handler.begin(&network, MQTT_SERVER, MQTT_PORT, ESP.getChipId());

	ota_update.enable_callbacks();
	ota_update.begin(OTA_HOSTNAME, OTA_PASSWORD);

	LittleFS.begin();
	calib_storage.begin();

	ph_sensor.begin(ADC_PH_CH);
	temp_sensor.begin(ADC_TEMP_CH);

	web_server.begin();
}

void loop()
{
	ota_update.handle();
	state_machine.run();
}