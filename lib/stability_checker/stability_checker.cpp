#include <stability_checker.h>

StabilityChecker::StabilityChecker(size_t size, float threshold)
	: _window_size(size),
	  _ring_buffer(new uint16_t[size]()),
	  _threshold(threshold),
	  _index(0),
	  _sum(0) {}

StabilityChecker::~StabilityChecker() { delete[] _ring_buffer; }

void StabilityChecker::check_stability(uint16_t value)
{
	calc_mean_value(value);
	calc_standard_deviation();
	if (_standard_deviation < _threshold)
	{
		_is_stable = true;
	}
	else
	{
		_is_stable = false;
	}
}

void StabilityChecker::calc_mean_value(uint16_t value)
{
	_sum -= _ring_buffer[_index];
	_ring_buffer[_index] = value;
	_sum += value;

	_index = (_index + 1) % _window_size;

	_mean_value = static_cast<float>(_sum) / _window_size;
}

void StabilityChecker::calc_standard_deviation()
{
	float sum_sq_diff = 0;

	for (uint8_t i = 0; i < _window_size; i++)
	{
		float diff = _ring_buffer[i] - _mean_value;
		sum_sq_diff += diff * diff;
	}

	_standard_deviation = sqrtf(sum_sq_diff / _window_size);
}

void StabilityChecker::set_threshold(float threshold)
{
	_threshold = threshold;
}

float StabilityChecker::get_threshold() const
{
	return _threshold;
}

float StabilityChecker::get_mean_value() const
{
	return _mean_value;
}

float StabilityChecker::get_standard_deviation() const
{
	return _standard_deviation;
}

bool StabilityChecker::get_stable_state() const
{
	return _is_stable;
}