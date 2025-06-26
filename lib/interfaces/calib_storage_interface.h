#ifndef CALIB_STORAGE_INTERFACE_H
#define CALIB_STORAGE_INTERFACE_H

#include <cstdint>

class CalibStorageInterface {
	public:
		virtual ~CalibStorageInterface() = default;

		virtual bool begin(bool readonly) = 0;

		virtual void end() = 0;

		virtual void set_calib_value(const char* name, float value) = 0;

		virtual float get_calib_value(const char* name, float default_value) = 0;

		virtual void set_calib_raw_value(const char* name, uint16_t value) = 0;

		virtual uint16_t get_calib_raw_value(const char* name, uint16_t default_value) = 0;

};

#endif