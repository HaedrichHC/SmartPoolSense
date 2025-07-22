#include <ph_sensor.h>

PhSensor::PhSensor(AdConverterInterface &adc,
				   CalibStorageInterface &calib_storage,
				   StabilityCheckerInterface &stability)
	: _adc(adc), _calib_storage(calib_storage), _stability(stability)
{
	load_calibration();
};

PhSensor::~PhSensor() {};

void PhSensor::begin(uint8_t adc_channel)
{
	_adc_channel = adc_channel;
}
void PhSensor::calculate_value(float temp)
{
	const float abs_temp = ABSOLUTE_ZERO + temp;
	if (abs_temp < 1.0f)
	{
		_ph_act = -1.0f;
		return;
	}
	_ph_act = _calib.ph_1 + ((_calib.volt_1 - _volt_act) / SLOPE_FACTOR) / (NERNST_FACTOR * (abs_temp));
}

void PhSensor::read_raw()
{
	_volt_act = _adc.read_mv(_adc_channel);
}

uint16_t PhSensor::get_raw_value() const
{
	return _volt_act;
}

float PhSensor::get_value() const
{
	return _ph_act;
}

void PhSensor::check_stability(uint16_t raw_value)
{
	_stability.check_stability(raw_value);
}

bool PhSensor::is_stable() const
{
	return _stability.get_stable_state();
}

void PhSensor::set_stability_threshold(float threshold)
{
	_stability.set_threshold(threshold);
}

float PhSensor::get_stability_threshold() const
{
	return _stability.get_threshold();
}

float PhSensor::get_stability_deviation() const
{
	return _stability.get_standard_deviation();
}

void PhSensor::set_calibration(float ph_1, uint16_t volt_1)
{
	_calib.ph_1 = ph_1;
	_calib.volt_1 = volt_1;
	save_calibration(ph_1, volt_1);
}

PhSensor::calib_data PhSensor::get_calibration() const
{
	return _calib;
}

void PhSensor::load_calibration()
{
	_calib.ph_1 = _calib_storage.get_calib_value("ph_1", PH_VALUE_1);
	_calib.volt_1 = _calib_storage.get_calib_raw_value("ph_volt_1", PH_VOLT_1);
}

void PhSensor::save_calibration(float p_1, uint16_t v_1)
{
	_calib_storage.set_calib_value("ph_1", p_1);
	_calib_storage.set_calib_raw_value("ph_volt_1", v_1);
}