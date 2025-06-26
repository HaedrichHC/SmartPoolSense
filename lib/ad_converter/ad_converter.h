#ifndef AD_CONVERTER_H
#define AD_CONVERTER_H

#include <ad_converter_interface.h>
#include <config.h>
#include <MCP_ADC.h>

class AdConverter : public AdConverterInterface {
    public:

        AdConverter();

        ~AdConverter();

		void begin(uint8_t select) override;

        uint16_t read_raw(uint8_t channel) override;

		uint16_t read_mv(uint8_t channel) override;

    private:
        MCP3008 _ad_converter;
};

#endif