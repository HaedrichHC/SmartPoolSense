#ifndef CALIB_STORAGE_H
#define CALIB_STORAGE_H

#include "interfaces/calib_storage_interface.h"
#include <Preferences.h>

class CalibStorage : public CalibStorageInterface {
	public:
		CalibStorage(const char* name_space);
		~CalibStorage();

        bool begin(bool readonly = false) override;

        void end() override;

        void set_calib_value(const char* name, float value) override;

        float get_calib_value(const char* name, float default_value) override;

        void set_calib_raw_value(const char* name, uint16_t value) override;

        uint16_t get_calib_raw_value(const char* name, uint16_t default_value) override;

    private:
        Preferences _prefs;
        const char* NAMESPACE;

};

#endif