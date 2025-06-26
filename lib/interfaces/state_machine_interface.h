#ifndef STATE_MASCHINE_INTERFACE_H
#define STATE_MASCHINE_INTERFACE_H

#include <cstdint>

struct SensorData {
    float temp_value;
    float ph_value;
    uint16_t temp_raw;
    uint16_t ph_raw;
	bool temp_stable_status;
	bool ph_stable_status;
};

class StateMachineInterface {
    public:
        virtual ~StateMachineInterface() = default;

        virtual SensorData get_sensor_data() = 0;

        virtual void set_temp_calibration(float t_1, uint16_t v_1, float t_2, uint16_t v_2) = 0;

        virtual void set_ph_calibration(float ph_1, uint16_t v_1) = 0;
};


#endif