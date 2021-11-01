# csgo-democheck
## CSGO Demo cheater study

## `This is a work in progress!`

Tired of cheaters? Yeah me too..

In this study i will try to detect malicious behaviour using demo files.

## Roadmap

- Read and parse .dem files
- Create metrics of every player
- Run an (ML)algorithm over the metricies

# .dem parsing
Demo parsing documentation is quite sparse, so i will try to document it!

## Header
The header is quite easy to parse. It has a fixed length so a single copy is all that is needed: [Source](https://developer.valvesoftware.com/wiki/DEM_Format)

| Type   | Field            | Value                                                     |
|--------|------------------|-----------------------------------------------------------|
| String | Header           | 8 characters, should be "HL2DEMO"+NULL                    |
| int    | Demo Protocol    | Demo protocol version (stored in little endian)           |
| Int    | Network Protocol | Network protocol version number (stored in little endian) |
| String | Server name      | 260 characters long                                       |
| String | Client name      | 260 characters long                                       |
| String | Map name         | 260 characters long                                       |
| String | Game directory   | 260 characters long                                       |
| Int    | Ticks            | The number of ticks in the demo                           |
| Float  | Playback time    | The length of the demo, in seconds                        |
| Int    | Frames           | The number of frames in the demo                          |
| Int    | Sign on length   | Length of the signon data (Init for first frame)          |

## Frame
After the header, the real parsing can start. The frame around the 'real' data stores only a couple of fields

| Type   | Field            | Value                                                     |
|--------|------------------|-----------------------------------------------------------|
| Int    | Command          | The command to be executed                                |
| Int    | Tick             | The tick of the current packet. Only needed for some commands |
| Int    | Playerslot       | No clue. seems useless                                    |

### Commands
There are only a few commands possible for CSGO:

| Type   | Field            | Value  | Description                                      |
|--------|------------------|--------|--------------------------------------------------|
| Int    | dem_signon       | 1      | The first packet send. Same usage as dem_signon  |
| Int    | dem_packet       | 2      | A data packet. can contain multiple packets      |
| Int    | dem_synctick     | 3      | Tick to sync client and server. can be ignored   |
| Int    | dem_consolecmd   | 4      | A data packet, deal as necessary                 |
| Int    | dem_usercmd      | 5      | A data packet, deal as necessary                 |
| Int    | dem_datatables   | 6      | A data table....                                 |
| Int    | dem_stop         | 7      | The last frame. stop after this                  |
| Int    | dem_lastcommand  | 7      |                                                  |

### Packets

A packet will start with some viewangles. These seem quite useless, and used more for ingame data. 

> This is in total 152 bytes, since its in the packet twice

> A vector is the same as float[3]

| Type   | Field                 
|--------|-----------------------
| Int    | Flags                 
| Vector | viewOrigin            
| Vector | viewAngles            
| Vector | localViewAngles       
| Vector | viewOrigin2           
| Vector | viewAngles2           
| Vector | localViewAngles2      


