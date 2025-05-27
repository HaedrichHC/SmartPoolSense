#include "drivers/adc_mcp3008.h"


AdcMCP3008::AdcMCP3008() {}

AdcMCP3008::~AdcMCP3008() {}

void AdcMCP3008::begin(uint8_t select) {
	_mcp3008.begin(select);
}

uint16_t AdcMCP3008::read_raw(uint8_t channel) {
    return _mcp3008.read(channel);
}

uint16_t AdcMCP3008::read_mv(uint8_t channel) {
	return _mcp3008.read(channel)*ADC_V_REF;
}