#include <stability_checker.h>


StabilityChecker::StabilityChecker(size_t size) 
	: _window_size(size), _ring_buffer(new uint16_t[size]()), _index(0) {}

StabilityChecker::~StabilityChecker() {delete[] _ring_buffer;}

bool StabilityChecker::is_stable(uint16_t value) {
	calc_mean_value(value);
	calc_standard_deviation();
	if (_standard_deviation < 10) {
		_is_stable = true;
		return true;
	} else {
		_is_stable = false;
		return false;
	}
}

void StabilityChecker::calc_mean_value(uint16_t value) {
	_sum -= _ring_buffer[_index];
	_ring_buffer[_index] = value;
	_sum += value;

	_index = (_index + 1) % _window_size;

	_mean_value = _sum / _window_size;
}

void StabilityChecker::calc_standard_deviation() {
	uint16_t sum_sq_diff = 0;

	for(uint8_t i = 0; i < _window_size; i++) {
		sum_sq_diff += pow(_ring_buffer[i] - _mean_value, 2);
	}
	
	_standard_deviation = sqrt(sum_sq_diff/_window_size);
}

uint16_t StabilityChecker::get_mean_value() {
	return _mean_value;
}

uint16_t StabilityChecker::get_standard_deviation() {
	return _standard_deviation;
}

bool StabilityChecker::get_stable_status() {
	return _is_stable;
}