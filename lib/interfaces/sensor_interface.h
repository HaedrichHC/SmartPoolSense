#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <stdint.h>

class SensorInterface
{
public:
	virtual ~SensorInterface() = default;

	virtual void begin(uint8_t adc_channel) = 0;
	virtual void read_raw() = 0;
	virtual uint16_t get_raw_value() const = 0;
	virtual float get_value() const = 0;
	virtual void check_stability(uint16_t raw_value) = 0;
	virtual bool is_stable() const = 0;
	virtual void set_stability_threshold(float threshold) = 0;
};

#endif