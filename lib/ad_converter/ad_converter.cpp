#include <ad_converter.h>


AdConverter::AdConverter() {}

AdConverter::~AdConverter() {}

void AdConverter::begin(uint8_t select) {
	_ad_converter.begin(select);
}

uint16_t AdConverter::read_raw(uint8_t channel) {
    return _ad_converter.read(channel);
}

uint16_t AdConverter::read_mv(uint8_t channel) {
	return _ad_converter.read(channel)*ADC_V_REF;
}