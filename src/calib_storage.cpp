#include "calib_storage.h"

CalibStorage::CalibStorage(const char* name_space) 
    : _prefs(), NAMESPACE(name_space) {}

CalibStorage::~CalibStorage() {}

bool CalibStorage::begin(bool readonly) {
    return _prefs.begin(NAMESPACE, readonly);
}

void CalibStorage::end() {
    _prefs.end();
}

void CalibStorage::set_calib_value(const char* name, float value) {
    _prefs.putFloat(name, value);
}

float CalibStorage::get_calib_value(const char* name, float default_value) {
    return _prefs.getFloat(name, default_value);
}

void CalibStorage::set_calib_raw_value(const char* name, uint16_t value) {
    _prefs.putUShort(name, value);
}

uint16_t CalibStorage::get_calib_raw_value(const char* name, uint16_t default_value) {
    return _prefs.getUShort(name, default_value);
}