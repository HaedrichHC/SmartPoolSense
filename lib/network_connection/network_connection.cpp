#include <network_connection.h>

NetworkConnection::NetworkConnection() {}

NetworkConnection::~NetworkConnection() {}

void NetworkConnection::connect(const char *ssid, const char *password)
{
	WiFi.mode(WIFI_STA);

	for (uint8_t tr = 1; tr <= MAX_TRIES; tr++)
	{
		WiFi.begin(ssid, password);
		Serial.println("Connecting to wifi...");
		if (WiFi.waitForConnectResult() == WL_CONNECTED)
		{
			Serial.println("Successful connected!");
			Serial.print("IP-Adresse: ");
			Serial.println(WiFi.localIP());
			return;
		}
		Serial.printf("Attempt %d/%d failed. Retrying in %d seconds...\n", tr, MAX_TRIES, WAIT);
		delay(WAIT * 1000);
	}

	Serial.println("No connection could be established to wifi");
	Serial.println("ESP8266 restart");
	ESP.restart();
}

bool NetworkConnection::isConnected()
{
	return WiFi.status() == WL_CONNECTED;
}

Client &NetworkConnection::getClient()
{
	return _wifiClient;
}