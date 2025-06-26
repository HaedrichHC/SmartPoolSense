#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include <ad_converter_interface.h>
#include <calib_storage_interface.h>
#include <config.h>


class PhSensor {
	public:
		struct calib_data {
			float ph_ref = 0.0;
			uint16_t volt_ref = 0;
		};

		PhSensor(AdConverterInterface& adc, CalibStorageInterface& calib_storage);

		~PhSensor();

		void begin(uint8_t adc_channel);
		
		void calculate_value(float temp);

		void read_raw();

		uint16_t get_raw_value();

		float get_value();

		void set_calibration(float ph_1, uint16_t volt_1);

	private:
		AdConverterInterface& _adc;
		CalibStorageInterface& _calib_storage;
		uint8_t _adc_channel = 0;
		uint16_t _volt_act = 0;
		float _ph_act = 0.0;
		calib_data _calib;

		void load_calibration();
		void save_calibration(float p_1, uint16_t v_1);

};

#endif