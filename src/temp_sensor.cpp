#include "temp_sensor.h"


TempSensor::TempSensor(ADCInterface& adc, CalibStorage& calib_storage)
	: _adc(adc), _calib_storage(calib_storage) {}

TempSensor::~TempSensor() {}

void TempSensor::begin(uint8_t adc_channel) {
	_adc_channel = adc_channel;
	load_calibration();
}

void TempSensor::calculate_value() {
	_temp_act = _calib.temp_1 + ((_calib.temp_2 - _calib.temp_1)/(_calib.volt_2 - _calib.volt_1)) * (_volt_act - _calib.volt_1);
}

void TempSensor::read_raw() {
	_volt_act = _adc.read_mv(_adc_channel);
}

uint16_t TempSensor::get_raw_value() {
	return _volt_act;
}

float TempSensor::get_value() {
	return _temp_act;
}

void TempSensor::set_calibration(float temp_1, uint16_t volt_1, float temp_2, uint16_t volt_2) {
	_calib.temp_1 = temp_1;
	_calib.volt_1 = volt_1;
	_calib.temp_2 = temp_2;
	_calib.volt_2 = volt_2;
	save_calibration(temp_1, volt_1, temp_2, volt_2);
}

void TempSensor::load_calibration() {
	_calib.temp_1 =	_calib_storage.get_calib_value("temp_1", TEMP_VALUE_1);
	_calib.volt_1 = _calib_storage.get_calib_raw_value("temp_volt_1", TEMP_VOLT_1);
	_calib.temp_2 = _calib_storage.get_calib_value("temp_2", TEMP_VALUE_2);
	_calib.volt_2 = _calib_storage.get_calib_raw_value("temp_volt_2", TEMP_VOLT_2);
}

void TempSensor::save_calibration(float t_1, uint16_t v_1, float t_2, uint16_t v_2) {
	_calib_storage.set_calib_value("temp_1", t_1);
	_calib_storage.set_calib_raw_value("temp_volt_1", v_1);
	_calib_storage.set_calib_value("temp_2", t_2);
	_calib_storage.set_calib_raw_value("temp_volt_2", v_2);
}