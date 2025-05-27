#ifndef CALIB_STORAGE_H
#define CALIB_STORAGE_H

#include <Preferences.h>

class CalibStorage {
	public:
		CalibStorage(const char* name_space);
		~CalibStorage();

        bool begin(bool readonly = false);

        void end();

        void set_calib_value(const char* name, float value);

        float get_calib_value(const char* name, float default_value);

        void set_calib_raw_value(const char* name, uint16_t value);

        uint16_t get_calib_raw_value(const char* name, uint16_t default_value);

		bool clear_all();

        bool remove_calib_value(const char* name);

    private:
        Preferences _prefs;
        const char* NAMESPACE;

};

#endif