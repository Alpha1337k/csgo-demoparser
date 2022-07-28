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
    static std::unordered_map<std::string, int>propHm;

    if (!packetRef)
        return 0;

    std::unordered_map<std::string, int>::iterator foundItem = propHm.find(name);

    if (foundItem == propHm.end()){
        for (int i = 0; i < packetRef->properties.size(); i++)
        {
            if (packetRef->properties[i].first && name == *packetRef->properties[i].first){
                propHm.insert(std::make_pair(name, i));
                return &packetRef->properties[i].second;
            }
        }
    }else{
        int inx = foundItem->second;
        if (packetRef->properties[inx].first && name == *packetRef->properties[inx].first){
            return &packetRef->properties[inx].second;
        }
    }
    return 0;
}

Player	&Player::operator=(const Player &p)
{
	memcpy(&md, &p.md, sizeof(md));
	packetRef = p.packetRef;

	return *this;
}