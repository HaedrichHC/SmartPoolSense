# SmartPoolSense

SmartPoolSense is a smart home solution for automatic and continuous monitoring of the temperature and pH value of pool/spa water. 

![Overview](./Overview.svg)

*Figure 1: Overview*

## Hardware

![SmartPoolSense](./SmartPoolSense.svg)

*Figure 2: SmartPoolSense*


## Software

### Build

#### Mit PlatformIO (empfohlen)

1. Repository klonen
2. Abhängigkeiten installieren
3. Konfigurationsdateien erstellen
4. Build und Upload

#### Mit Arduino IDE

1. Installiere benötigte Bibliotheken
2. Konfigurationsdateien erstellen
3. Öffne main.cpp in der Arduino IDE
4. Wähle das richtige Board aus
5. Build und Upload

#### Konfiguration

Stelle sicher das folgende Einstellungen in secret.h korrekt sind:

- WLAN-Passwort
- WLAN-SSID
- MQTT-Server
- MQTT-Port

Stelle sicher das folgende Einstellungen in config.h 