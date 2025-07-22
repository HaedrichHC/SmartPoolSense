#ifndef STABILITY_CHECKER_H
#define STABILITY_CHECKER_H

#include <stability_checker_interface.h>
#include <cstdint>
#include <math.h>

class StabilityChecker : public StabilityCheckerInterface
{
public:
	StabilityChecker(size_t size, float threshold);
	~StabilityChecker();

	void check_stability(uint16_t value) override;
	void set_threshold(float threshold) override;
	float get_threshold() const override;
	float get_mean_value() const override;
	float get_standard_deviation() const override;
	bool get_stable_state() const override;

private:
	size_t _window_size;
	uint16_t *_ring_buffer = nullptr;
	float _threshold;
	size_t _index = 0;
	uint32_t _sum = 0;
	float _mean_value = 0;
	float _standard_deviation = 0;
	bool _is_stable = false;

	void calc_mean_value(uint16_t value);
	void calc_standard_deviation();
};

#endif