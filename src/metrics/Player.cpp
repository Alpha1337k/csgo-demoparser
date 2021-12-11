#include <demo.hpp>

Player::Player(std::string &data): packetRef(0)
{
	memcpy(this, &data[0], sizeof(md));

	md.version = bswap_64(md.version);
	md.xUid = bswap_64(md.xUid);
	md.userId = bswap_32(md.userId);
	md.friendsId = bswap_32(md.friendsId);

}

void DemoFile::AddPlayer(Player &p)
{
	players.push_back(p);
}

const std::vector<Player>	&DemoFile::getPlayers()
{
	return players;
}

Player	&DemoFile::getPlayer(size_t idx)
{
	return players[idx];
}