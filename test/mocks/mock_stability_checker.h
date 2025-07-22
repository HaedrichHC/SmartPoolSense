#ifndef MOCK_STABILITY_CHECKER_H
#define MOCK_STABILITY_CHECKER_H

#include <gmock/gmock.h>
#include <stability_checker_interface.h>

class MockStabilityChecker : public StabilityCheckerInterface
{
public:
	MOCK_METHOD(void, check_stability, (uint16_t value), (override));
	MOCK_METHOD(void, set_threshold, (float threshold), (override));
	MOCK_METHOD(float, get_threshold, (), (const, override));
	MOCK_METHOD(float, get_mean_value, (), (const, override));
	MOCK_METHOD(float, get_standard_deviation, (), (const, override));
	MOCK_METHOD(bool, get_stable_state, (), (const, override));
};

#endif