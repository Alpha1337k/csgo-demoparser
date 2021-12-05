#include <demo.hpp>

//109269

Packet::Packet(FileReader &f)
{

	// fread(this, 1, sizeof(*this) - sizeof(MessageVector), f);
	f.read(this, sizeof(*this) - sizeof(MessageVector));

	int sequenceIn = 0, sequenceOut = 0;
	int chunkSize = 0;
	f.read(&sequenceIn, sizeof(int));
	f.read(&sequenceOut, sizeof(int));

	f.read(&chunkSize, sizeof(int));
	//std::cout << "Chunksize: " << chunkSize << std::endl;

	msg = getProtoMesssages(f, chunkSize);
}

Packet::Packet() {}
