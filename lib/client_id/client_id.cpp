#include <client_id.h>

ClientId::ClientId() {}

ClientId::~ClientId() {}

void ClientId::generate(const char* id_name) {
	static char buffer[50];
	snprintf(buffer, sizeof(buffer), "%s_%u", id_name, ESP.getChipId());
    _client_id = buffer;
}

const char* ClientId::get_id() const {
	return _client_id;
}