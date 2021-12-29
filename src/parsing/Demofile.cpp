#include <demo.hpp>

DemoFile::DemoFile(FileReader &f): tick(0)
{
	eventHooks.resize(33, 0);
	std::size_t headerSize;

	headerSize = f.read(&header, sizeof(header));
	if (headerSize != 1072)
		throw std::exception();
	// std::cout << header << std::endl;
	bool isFinished = false;

	frames.reserve(header.frames + 50);
	packets.reserve(header.ticks * 2);

	while (!isFinished)
	{
		frames.emplace_back(f, isFinished, packets);
	}
}

DemoFile::~DemoFile()
{
#define DeleteItem(type)								\
	case svc_##type:									\
	{													\
		delete (type *)packets[i].second;	\
		break;											\
	}
#define DeleteNet(type)									\
	case net_##type:									\
	{													\
		delete (type *)packets[i].second;	\
		break;											\
	}
	for (size_t i = 0; i < packets.size(); i++)
	{
		switch (packets[i].first)
		{
			DeleteItem(GameEventList);
			DeleteItem(GameEvent);
			DeleteItem(ServerInfo);
			DeleteItem(CreateStringTable);
			DeleteItem(UpdateStringTable);
			DeleteItem(UserMessage);
			DeleteItem(PacketEntities);

			DeleteNet(Disconnect);
			DeleteNet(File);
			DeleteNet(Tick);
			DeleteNet(StringCmd);
			DeleteNet(SetConVar);
			DeleteNet(SignonState);

			DeleteItem(ClassInfo);
			DeleteItem(SetPause);
			DeleteItem(VoiceInit);
			DeleteItem(VoiceData);
			DeleteItem(Print);
			DeleteItem(Sounds);
			DeleteItem(SetView);
			DeleteItem(FixAngle);
			DeleteItem(CrosshairAngle);
			DeleteItem(BSPDecal);
			DeleteItem(TempEntities);
			DeleteItem(Prefetch);
			DeleteItem(Menu);
			DeleteItem(GetCvarValue);
			case svc_DataTable:
			{
				DataTable *d = (DataTable *)packets[i].second;
				for (size_t y = 0; y < d->msg.size(); y++)
				{
					delete (SendTable *)d->msg[y].second;
				}
				delete d;
			}
		
		default:
			break;
		}
	}
#undef DeleteItem
#undef DeleteNet
	
}