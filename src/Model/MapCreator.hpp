#ifndef ALIENHACK_MAP_CREATOR_HPP
#define	ALIENHACK_MAP_CREATOR_HPP



#include <boost/shared_ptr.hpp>
#include "World-2DTiles/BSPMapGeneration/BSPMapMakeWrapper.hpp"


namespace RL_shared
{
	class Zone;
}

namespace AlienHack
{



class MapCreator : RL_shared::MakeDefaultTiledRepresentation
{
public:

	typedef RL_shared::MakeDefaultTiledRepresentation::DoorType DoorType;
	typedef RL_shared::MakeDefaultTiledRepresentation::DoorOri DoorOri;
	typedef RL_shared::MakeDefaultTiledRepresentation MakeDefaultTiledRepresentation;

	MapCreator(RL_shared::Zone&);

	void create(RL_shared::MapStyles::MapStyle style);

	virtual void makeRoom(RL_shared::DBKeyValue zone_key, int room_key, int N, int S, int E, int W);
	virtual void makeDoor(RL_shared::DBKeyValue zone_key, int door_key, int room_key_1, int room_key_2, DoorType type, DoorOri ori, int N, int S, int E, int W);
	virtual void writeWallTile(int x, int z);
	virtual void writeRoomTile(int x, int z, int room_key, int N, int S, int E, int W);
	virtual void writeDoorTile(int x, int z, int door_key, int room_key_1, int room_key_2, DoorType type, DoorOri ori, int N, int S, int E, int W);

private:

	RL_shared::Zone& m_zone;

};



}



#endif
