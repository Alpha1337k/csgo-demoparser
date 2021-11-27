#include <demo.hpp>

DemoFile::DemoFile(FILE *f)
{
	std::size_t headerSize;
	
	headerSize = fread(&header, 1, sizeof(header), f);
	if (headerSize != 1072)
		throw std::exception();
	// std::cout << header << std::endl;
	bool isFinished = false;

	std::size_t packetCount = 0;
	while (!isFinished && packetCount < 1000000)
	{
		Frame frm(f, isFinished);
		// std::cout << frm <<  std::endl;
		frames.push_back(frm);
		packetCount++;
	}
}

DemoFile::~DemoFile()
{
#define DeleteItem(type) \
	case svc_##type: \
	{ \
		delete (type *)frames[i].pckt.msg[x].second; \
		break; \
	}
#define DeleteNet(type) \
	case net_##type: \
	{ \
		delete (type *)frames[i].pckt.msg[x].second; \
		break; \
	}
std::cout << "Freeing" << std::endl;
	for (size_t i = 0; i < frames.size(); i++)
	{
		for (size_t x = 0; x < frames[i].pckt.msg.size(); x++)
		{
			switch (frames[i].pckt.msg[x].first)
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
					DataTable *d = (DataTable *)frames[i].pckt.msg[x].second;
					for (size_t y = 0; y < d->msg.size(); y++)
					{
						delete (SendTable *)d->msg[y].second;
					}
					
				}
			
			default:
				break;
			}
		}
		
	}
#undef DeleteItem
#undef DeleteNet
	
}