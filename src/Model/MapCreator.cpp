#include "MapCreator.hpp"
#include "Objects/TerrainType.hpp"
#include "World-2DTiles/Zone.hpp"
#include "assert.hpp"
#include <stdlib.h>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;






MapCreator::MapCreator(Zone& zone)
	: m_zone(zone)
{
}

void MapCreator::create(RL_shared::MapStyles::MapStyle style)
{
	BSPMapGenerator(*this, m_zone.sizeX(), m_zone.sizeZ(), m_zone.sizeX()+m_zone.sizeZ(), style);
}

void MapCreator::makeRoom(DBKeyValue zone_key, int room_key, int N, int S, int E, int W)
{
}
void MapCreator::makeDoor(DBKeyValue zone_key, int door_key, int room_key_1, int room_key_2, DoorType type, DoorOri ori, int N, int S, int E, int W)
{
}
void MapCreator::writeWallTile(int x, int z)
{
	m_zone.putTerrain(terrain::Wall, x, z);
}
void MapCreator::writeRoomTile(int x, int z, int /*room_key*/, int N, int S, int E, int W)
{
	int width(E+1-W), height(N+1-S);

	if ((x==E-1) || (x==W))
		if ((z==N-1) || (z==S))
			if (5 == (rand() % 6))
				if (((width + height) > 10) || (7 == (rand() & 7)))
				{
					m_zone.putTerrain(terrain::Vent, x, z);
					return;
				}

	if (511 == (rand() & 511))
	{
		m_zone.putTerrain(terrain::Vent, x, z);
		return;
	}

	m_zone.putTerrain(terrain::Room, x, z);
}
void MapCreator::writeDoorTile(int x, int z, int /*door_key*/, int /*room_key_1*/, int /*room_key_2*/, DoorType type, DoorOri ori, int /*N*/, int /*S*/, int /*E*/, int /*W*/)
{
	if (MakeDefaultTiledRepresentation::REMOVE_WALL == type)
	{
		m_zone.putTerrain(terrain::Room, x, z);
	}
	else
	{
		if (MakeDefaultTiledRepresentation::Z_ALIGNED == ori)
			m_zone.putTerrain(terrain::Door | terrain::EW_facing, x, z);
		else
			m_zone.putTerrain(terrain::Door, x, z);
	}
}



}
