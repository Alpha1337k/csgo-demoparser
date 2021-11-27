#include <demo.hpp>

//109269

Packet::Packet(FILE *f)
{

	fread(this, 1, sizeof(*this) - sizeof(MessageVector), f);

	int sequenceIn = 0, sequenceOut = 0;
	size_t size = 0;
	int chunkSize = 0;
	size = fread(&sequenceIn, sizeof(int), 1, f);
	size = fread(&sequenceOut, sizeof(int), 1, f);

	fread(&chunkSize, sizeof(int), 1, f);
	//std::cout << "Chunksize: " << chunkSize << std::endl;

	msg = getProtoMesssages(f, chunkSize);
}

Packet::Packet() {}
