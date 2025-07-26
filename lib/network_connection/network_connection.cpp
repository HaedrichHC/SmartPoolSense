#include <network_connection.h>

NetworkConnection::NetworkConnection() {}

NetworkConnection::~NetworkConnection() {}

void NetworkConnection::connect(const char *ssid, const char *password)
{

    const uint8_t max_tries = 5;

    WiFi.mode(WIFI_STA);

    for (uint8_t tr = 1; tr <= max_tries; tr++)
    {
        WiFi.begin(ssid, password);
        Serial.println("Verbindungsaufbau...");
        if (WiFi.waitForConnectResult() == WL_CONNECTED)
        {
            Serial.println("Verbunden!");
            Serial.print("IP-Adresse: ");
            Serial.println(WiFi.localIP());
            return;
        }
        Serial.println("Verbindung fehlgeschalagen, neuer Versuch in 5s...");
        delay(5000);
    }

    Serial.println("Keine Verbindung konnte hergestellt werden!");
    Serial.println("ESP8266 wird neu gestartet");
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