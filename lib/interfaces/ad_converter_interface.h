#ifndef AD_CONVERTER_INTERFACE_H
#define AD_CONVERTER_INTERFACE_H

#include <cstdint>

class AdConverterInterface {
    public:
        virtual ~AdConverterInterface() = default;

		virtual void begin(uint8_t select) = 0;

        virtual uint16_t read_raw(uint8_t channel) = 0;

		virtual uint16_t read_mv(uint8_t channel) = 0;

};

#endif