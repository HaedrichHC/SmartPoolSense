#ifndef CLIENT_ID_GENERATOR_H
#define CLIENT_ID_GENERATOR_H


class ClientIDGenerator {
	public:
		virtual ~ClientIDGenerator() = default;
		virtual const char* generate() const = 0;
};

#endif