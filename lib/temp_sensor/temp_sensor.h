#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <sensor_interface.h>
#include <ad_converter_interface.h>
#include <calib_storage_interface.h>
#include <stability_checker_interface.h>
#include <config.h>

class TempSensor : public SensorInterface
{
public:
	struct calib_data
	{
		float temp_1 = 0.0;
		uint16_t volt_1 = 0;
		float temp_2 = 0.0;
		uint16_t volt_2 = 0;
	};

	TempSensor(AdConverterInterface &adc,
			   CalibStorageInterface &calib_storage,
			   StabilityCheckerInterface &stability);

	~TempSensor();

	void begin(uint8_t adc_channel) override;

	void calculate_value();

	void read_raw() override;

	uint16_t get_raw_value() const override;

	float get_value() const override;

	void check_stability(uint16_t raw_value) override;

	bool is_stable() const override;

	void set_stability_threshold(float threshold) override;

	float get_stability_threshold() const;

	float get_stability_deviation() const;

	void set_calibration(float temp_1, uint16_t volt_1, float temp_2, uint16_t volt_2);

	TempSensor::calib_data get_calibration();

private:
	AdConverterInterface &_adc;
	CalibStorageInterface &_calib_storage;
	StabilityCheckerInterface &_stability;
	uint8_t _adc_channel = 0;
	uint16_t _volt_act = 0;
	float _temp_act = 0.0;
	calib_data _calib;

	void load_calibration();
	void save_calibration(float t_1, uint16_t v_1, float t_2, uint16_t v_2);
};

#endif