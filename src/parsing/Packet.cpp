#include <demo.hpp>

Packet::Packet(FileReader &f)

	f.read(this, sizeof(*this) - sizeof(MessageVector));

	int sequenceIn = 0, sequenceOut = 0;
	int chunkSize = 0;
	f.read(&sequenceIn, sizeof(int));
	f.read(&sequenceOut, sizeof(int));
	f.read(&chunkSize, sizeof(int));

	getProtoMesssages(f, chunkSize, msg);
}

Packet::Packet() {}
