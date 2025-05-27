#ifndef CONFIG_H
#define CONFIG_H

// MQTT-Configuartion
#define MQTT_TOPIC "Garden/Pool"
#define MQTT_PAYLOAD_SIZE (128)
#define MQTT_RECONNECT 5000
#define MQTT_PUBLISH 15000

// Default Temperature Sensor calibration values
#define TEMP_VALUE_1 10.4
#define TEMP_VOLT_1 1640
#define TEMP_VALUE_2 49.0
#define TEMP_VOLT_2 4400

// Default pH-level Sensor calibration values
#define PH_VALUE_1 7.0
#define PH_VOLT_1 2880

// ADC-Configuartion
#define ADC_CS_PIN 15
#define ADC_TEMP_CH 0
#define ADC_PH_CH 1
#define ADC_V_REF 5

// Calibration-storage-Configuration
#define STORAGE_PATH "calib_data"

#endif