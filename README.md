# CSGO Demoparser

### 💨 Rapid CSGO demo file parser

Part one of a three part series where I try to catch cheaters using demo files, the other ones being [metrics](https://media.giphy.com/media/leqmpruKOh3gY/giphy.gif) and [anticheat](https://media.giphy.com/media/eeGcAuoE4yV48bihMs/giphy.gif).
I will also try to make some [documentation](https://github.com/Alpha1337k/csgo-demoparser/docs), since its quite sparse.


# How to use

## Build
You will need to install the [`protobuf-compiler`](https://grpc.io/docs/protoc-installation/) and ofcourse `make` and `clang`.
Alternatively you can use the provided Docker container to run the program.

## And now, Run!
```
make
./democheck [OPTIONS] [FILE]
```
> This will not do too much. you will need to provide a useful main to get some output.


### A couple of options to note:

- `--stdin` -> read from stdin instead of file
- `--parsemsg` -> print all preparsed packages
> of course this only applies to default main

## So, a sample main eh?
```c
#include <demo.hpp>
StartupParser startupParameters;

void	printServerInfo(void *data)
{
	ServerInfo *si = (ServerInfo *)data;

	std::cout << si->DebugString() << std::endl;
}

int main(int argc, char **argv)
{
	startupParameters = StartupParser(argc, argv);

	FileReader f(startupParameters.last().first);

	DemoFile demo(f);
	demo.addEventHook(svc_ServerInfo, printServerInfo);
	demo.create_metrics();
}

```

## But how do I get even more data?
Simple. you can hook a function pointer to each event / packet. You can do that by calling `demo.addEventHook`. All events will return the corresponding Protobuf packet, with the exception being `svc_UserMessage` which will return a string.

For `PacketEntities`, the return value is almost useless. If you want the players for example, you should call `getPlayers()` or `getPlayer()`. More functions will be added, but feel free to help.

> A complete list of the possible types can be found [here](https://github.com/Alpha1337k/csgo-demoparser/blob/main/protobuf/netmessages.proto#L174). 


# Performance

Afaik this is the fastest parser available, and i will always keep performance in mind when adding features.

All the tests are run on `WSL2: Ubuntu 18.04`, and the project is stored on an `HDD`. That combo explains the slow read (i hope). CPU used is an `Intel Core I7 4820k`.

| Type                      | Read  | Preparse | Metrics | Total | Total no IO |
|---------------------------|-------|----------|---------|-------|-------------|
| Basic demo (≈82000 ticks) | 240ms | 105ms    | 360ms   | 705ms | 465ms       |
> Might need some more test cases. feel free!

---

# Another roadmap that will never be finished
- An CGI-like interface, with SQL-like commands
- Live demo parsing
- More getters, setters, and data!
- Support for use in other languages

# Bugs, features.
Please use the tools GitHub provides to contact me. Pull requests and issues / requests are always welcome!

### Enjoy!
