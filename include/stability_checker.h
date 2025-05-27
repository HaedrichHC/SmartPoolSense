#ifndef STABILITY_CHECKER_H
#define STABILITY_CHECKER_H

#include "c_types.h"
#include "math.h"


class StabilityChecker {
    public:
        StabilityChecker(size_t size);
        ~StabilityChecker();

        bool is_stable(uint16_t value);
        uint16_t get_mean_value();
        uint16_t get_standard_deviation();
		bool get_stable_status();

    private:
        bool _is_stable = false;
        size_t _window_size;
        uint16_t *_ring_buffer = nullptr;
        size_t _index = 0;

        uint32_t _sum = 0;
        uint16_t _mean_value = 0;
        uint16_t _standard_deviation = 0;

        void calc_mean_value(uint16_t value);
        void calc_standard_deviation();
};


#endif