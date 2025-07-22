#ifndef STABILITY_CHECKER_INTERFACE_H
#define STABILITY_CHECKER_INTERFACE_H

#include <cstdint>

class StabilityCheckerInterface
{
public:
	virtual ~StabilityCheckerInterface() = default;

	virtual void check_stability(uint16_t value) = 0;
	virtual void set_threshold(float threshold) = 0;
	virtual float get_threshold() const = 0;
	virtual float get_mean_value() const = 0;
	virtual float get_standard_deviation() const = 0;
	virtual bool get_stable_state() const = 0;
};

#endif