#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <state_machine_interface.h>
#include <mqtt_handler.h>
#include <ph_sensor.h>
#include <temp_sensor.h>
#include <config.h>
#include <stability_checker.h>

class StateMachine : public StateMachineInterface
{
public:
	enum class State
	{
		IDLE,
		READ_SENSORS,
		CALCULATE,
		STABLE_CHECK,
		PUBLISH,
		RECONNECT
	};

	StateMachine(MqttHandler &mqtt, PhSensor &ph_sensor, TempSensor &temp_sensor);
	~StateMachine();

	SensorData get_sensor_data() override;

	void run();

	void set_temp_calibration(float temp_1, uint16_t volt_1, float temp_2, uint16_t volt_2) override;

	void set_ph_calibration(float ph_1, uint16_t volt_1) override;

private:
	State _currentState = State::IDLE;

	MqttHandler &_mqtt;
	PhSensor &_ph_sensor;
	TempSensor &_temp_sensor;

	char _mqtt_payload[MQTT_PAYLOAD_SIZE];

	uint32_t _last_check = 0;
	uint32_t _last_publish = 0;
	uint32_t _last_reconnect = 0;
	uint32_t _now = 0;

	void create_mqtt_payload();
};

#endif