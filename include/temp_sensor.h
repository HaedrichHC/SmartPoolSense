#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "interfaces/adc_interface.h"
#include "configs/config.h"
#include "calib_storage.h"

class TempSensor {
    public:
		struct calib_data {
			float temp_1 = 0.0;
			uint16_t volt_1 = 0;
			float temp_2 = 0.0;
			uint16_t volt_2 = 0;
		};

		TempSensor(ADCInterface& adc, CalibStorage& calib_storage);
		~TempSensor();

        void begin(uint8_t adc_channel);

        void calculate_value();

		void read_raw();

		uint16_t get_raw_value();

		float get_value();

		void set_calibration(float temp_1, uint16_t volt_1, float temp_2, uint16_t volt_2);

    private:
        ADCInterface& _adc;
		CalibStorage& _calib_storage;
        uint8_t _adc_channel = 0;
		uint16_t _volt_act = 0;
		float _temp_act = 0.0;
		calib_data _calib;

		void load_calibration();
		void save_calibration(float t_1, uint16_t v_1, float t_2, uint16_t v_2);
};

#endif