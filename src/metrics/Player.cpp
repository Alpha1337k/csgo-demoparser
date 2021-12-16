#include <demo.hpp>

Player::Player(std::string &data): packetRef(0)
{
	memcpy(this, &data[0], sizeof(md));

	md.version = bswap_64(md.version);
	md.xUid = bswap_64(md.xUid);
	md.userId = bswap_32(md.userId);
	md.friendsId = bswap_32(md.friendsId);

}

void DemoFile::addPlayer(Player &p, int idx)
{
	players[idx] = p;
}

const std::unordered_map<int, Player>	&DemoFile::getPlayers()
{
	return players;
}

Player	&DemoFile::getPlayer(size_t idx)
{
	return players[idx];
}

const GameEntities::Property *Player::getProperty(std::string name) const
{
	if (!packetRef)
		return 0;
	std::unordered_map<std::string, GameEntities::Property>::iterator prop = packetRef->properties.find(name);

	if (prop == packetRef->properties.end())
		return 0;

	return &prop->second;
}

Player	&Player::operator=(const Player &p)
{
	memcpy(&md, &p.md, sizeof(md));
	packetRef = p.packetRef;

	return *this;
}