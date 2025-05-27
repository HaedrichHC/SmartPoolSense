#include "ph_sensor.h"

PhSensor::PhSensor(ADCInterface& adc, CalibStorage& calib_storage)
    : _adc(adc), _calib_storage(calib_storage) {};

PhSensor::~PhSensor() {};

void PhSensor::begin(uint8_t adc_channel) {
    _adc_channel = adc_channel;
	load_calibration();	
}
void PhSensor::calculate_value(float temp) {
	_ph_act = _calib.ph_ref + ((_calib.volt_ref - _volt_act)/5.52)/(0.19842*(273.15 + temp));  
}

void PhSensor::read_raw() {
	_volt_act = _adc.read_mv(_adc_channel);
}

uint16_t PhSensor::get_raw_value() {
	return _volt_act;
}

float PhSensor::get_value() {
	return _ph_act;
}

void PhSensor::set_calibration(float ph_1, uint16_t volt_1) {
	_calib.ph_ref = ph_1;
	_calib.volt_ref = volt_1;
	save_calibration(ph_1, volt_1);
}

void PhSensor::load_calibration() {	
	_calib.ph_ref = _calib_storage.get_calib_value("ph_1", PH_VALUE_1);
	_calib.volt_ref = _calib_storage.get_calib_raw_value("ph_volt_1", PH_VOLT_1);
}

void PhSensor::save_calibration(float p_1, uint16_t v_1) {
	_calib_storage.set_calib_value("ph_1", p_1);
	_calib_storage.set_calib_raw_value("ph_volt_1", v_1);
}