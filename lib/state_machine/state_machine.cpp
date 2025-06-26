#include <state_machine.h>


StateMachine::StateMachine(MqttHandler& mqtt, PhSensor& ph_sensor, TempSensor& temp_sensor)
	: _mqtt(mqtt), _ph_sensor(ph_sensor), _temp_sensor(temp_sensor), 
	  _stable_check_ph(100), _stable_check_temp(100), _mqtt_payload("") {}

StateMachine::~StateMachine() {}

SensorData StateMachine::get_sensor_data() {
	return {
		_temp_sensor.get_value(),
		_ph_sensor.get_value(),
		_temp_sensor.get_raw_value(),
		_ph_sensor.get_raw_value(),
		_stable_check_temp.get_stable_status(),
		_stable_check_ph.get_stable_status()
	};
}

void StateMachine::run() {

	_now = millis();

	switch (_currentState) {
		case State::IDLE:
			_mqtt.loop();
			_currentState = State::READ_SENSORS;
			break;
		case State::READ_SENSORS:
			_temp_sensor.read_raw();
			_ph_sensor.read_raw();
			_currentState = State::CALCULATE;
			break;
		case State::CALCULATE:
			_temp_sensor.calculate_value();
			_ph_sensor.calculate_value(_temp_sensor.get_value());
			_currentState = State::STABLE_CHECK;
			break;
		case State::STABLE_CHECK:
			if (_now - _last_check > 500) {
				_last_check = _now;
				_stable_check_temp.is_stable(_temp_sensor.get_raw_value());
				_stable_check_ph.is_stable(_ph_sensor.get_raw_value());
			}
			_currentState = State::PUBLISH;
			break;
		case State::PUBLISH:
			if (_now - _last_publish > MQTT_PUBLISH) {
				Serial.println("State: PUBLISH");
				_last_publish = _now;
				create_mqtt_payload();
				Serial.println(_mqtt_payload);
				_mqtt.publish(MQTT_TOPIC, _mqtt_payload);
			}
			_currentState = State::RECONNECT;
			break;
		case State::RECONNECT:
			if (!_mqtt.connected() && _now - _last_reconnect > MQTT_RECONNECT) {
				Serial.println("State: RECONNECT");
				_last_reconnect = _now;
				_mqtt.connect();
			}
			_currentState = State::IDLE;
			break;
	}
}

void StateMachine::create_mqtt_payload() {
	snprintf(_mqtt_payload, MQTT_PAYLOAD_SIZE,
		"{\"temperature\":%.2f,\"pH\":%.2f,\"raw\":[%d,%d]}",
		_temp_sensor.get_value(), _ph_sensor.get_value(), _temp_sensor.get_raw_value(), _ph_sensor.get_raw_value()
	);
}

void StateMachine::set_temp_calibration(float temp_1, uint16_t volt_1, float temp_2, uint16_t volt_2) {
	_temp_sensor.set_calibration(temp_1, volt_1, temp_2, volt_2);
}

void StateMachine::set_ph_calibration(float ph_1, uint16_t volt_1) {
	_ph_sensor.set_calibration(ph_1, volt_1);
}