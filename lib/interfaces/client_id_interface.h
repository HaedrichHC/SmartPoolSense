#ifndef CLIENT_ID_INTERFACE_H
#define CLIENT_ID_INTERFACE_H

class ClientIdInterface {
	public:
		virtual ~ClientIdInterface() = default;
		virtual void generate(const char* id_name) = 0;
		virtual const char* get_id() const = 0;
};

#endif