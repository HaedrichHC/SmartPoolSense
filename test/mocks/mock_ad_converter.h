#ifndef MOCK_AD_CONVERTER_H
#define MOCK_AD_CONVERTER_H

#include <gmock/gmock.h>
#include <ad_converter_interface.h>

class MockAdConverter : public AdConverterInterface
{
public:
    MOCK_METHOD(void, begin, (uint8_t select), (override));
    MOCK_METHOD(uint16_t, read_raw, (uint8_t channel), (override));
    MOCK_METHOD(uint16_t, read_mv, (uint8_t channel), (override));
};

#endif