#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include <sensor_interface.h>
#include <ad_converter_interface.h>
#include <calib_storage_interface.h>
#include <stability_checker_interface.h>

#include <config.h>

class PhSensor : public SensorInterface
{
public:
	struct calib_data
	{
		float ph_1 = 0.0;
		uint16_t volt_1 = 0;
	};

	PhSensor(AdConverterInterface &adc,
			 CalibStorageInterface &calib_storage,
			 StabilityCheckerInterface &stability);

	~PhSensor();

	void begin(uint8_t adc_channel) override;

	void calculate_value(float temp);

	void read_raw() override;

	uint16_t get_raw_value() const override;

	float get_value() const override;

	void check_stability(uint16_t raw_value) override;

	bool is_stable() const override;

	void set_stability_threshold(float threshold) override;

	float get_stability_threshold() const;

	float get_stability_deviation() const;

	void set_calibration(float ph_1, uint16_t volt_1);

	PhSensor::calib_data get_calibration() const;

private:
	AdConverterInterface &_adc;
	CalibStorageInterface &_calib_storage;
	StabilityCheckerInterface &_stability;
	uint8_t _adc_channel = 0;
	uint16_t _volt_act = 0;
	float _ph_act = 0.0;
	calib_data _calib;

	static constexpr float SLOPE_FACTOR = 5.52f;
	static constexpr float NERNST_FACTOR = 0.19842f;
	static constexpr float ABSOLUTE_ZERO = 273.15f;

	void load_calibration();
	void save_calibration(float p_1, uint16_t v_1);
};

#endif