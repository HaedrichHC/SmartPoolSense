#ifndef MOCK_CALIB_STORAGE_H
#define MOCK_CALIB_STORAGE_H

#include <gmock/gmock.h>
#include <calib_storage_interface.h>

class MockCalibStorage : public CalibStorageInterface
{
public:
    MOCK_METHOD(bool, begin, (bool readonly), (override));
    MOCK_METHOD(void, end, (), (override));
    MOCK_METHOD(void, set_calib_value, (const char *name, float value), (override));
    MOCK_METHOD(float, get_calib_value, (const char *name, float default_value), (override));
    MOCK_METHOD(void, set_calib_raw_value, (const char *name, uint16_t value), (override));
    MOCK_METHOD(uint16_t, get_calib_raw_value, (const char *name, uint16_t default_value), (override));
};

#endif