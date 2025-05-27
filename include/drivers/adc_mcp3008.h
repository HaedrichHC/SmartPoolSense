#ifndef ADC_MCP3008_H
#define ADC_MCP3008_H

#include "interfaces/adc_interface.h"
#include "configs/config.h"
#include "MCP_ADC.h"

class AdcMCP3008 : public ADCInterface {
    public:

        AdcMCP3008();

        ~AdcMCP3008();

		void begin(uint8_t select) override;

        uint16_t read_raw(uint8_t channel) override;

		uint16_t read_mv(uint8_t channel) override;

    private:
        MCP3008 _mcp3008;
};

#endif