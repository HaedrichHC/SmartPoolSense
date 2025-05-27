#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include "interfaces/adc_interface.h"
#include "configs/config.h"
#include "calib_storage.h"


class PhSensor {
	public:
		struct calib_data {
			float ph_ref = 0.0;
			uint16_t volt_ref = 0;
		};

		PhSensor(ADCInterface& adc, CalibStorage& calib_storage);

		~PhSensor();

		void begin(uint8_t adc_channel);
		
		void calculate_value(float temp);

		void read_raw();

		uint16_t get_raw_value();

		float get_value();

		void set_calibration(float ph_1, uint16_t volt_1);

	private:
		ADCInterface& _adc;
		CalibStorage& _calib_storage;
		uint8_t _adc_channel = 0;
		uint16_t _volt_act = 0;
		float _ph_act = 0.0;
		calib_data _calib;

		void load_calibration();
		void save_calibration(float p_1, uint16_t v_1);

};

#endif