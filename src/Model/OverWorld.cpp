#include "OverWorld.hpp"
#include "AHGameModel.hpp"
#include "Objects/PlayerCharacter.hpp"
#include "Objects/Alien.hpp"
#include "Objects/Egg.hpp"
#include "Objects/Pickup.hpp"
#include "Objects/Armour.hpp"
#include "Objects/Weapon.hpp"
#include "Objects/WeaponData.hpp"
#include "Objects/Decoration.hpp"
#include "Objects/Terminal.hpp"
#include "Objects/Furniture.hpp"
#include "Objects/Barrel.hpp"
#include "Objects/TerrainType.hpp"
#include "Objects/ObjectType.hpp"
#include "MapCreator.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


namespace
{
	const int GROUND_FLOOR = 0;
	const int QUICK_NORMAL_SIZE = 36;
	const int QUICK_REACTOR_SIZE = 48;
	const int NORMAL_NORMAL_SIZE = 48;
	const int NORMAL_REACTOR_SIZE = 64;
}


void spawnSwarm( AHGameModel& model, RL_shared::DBKeyValue zone_key )
{
	World& world( model.world() );
	Zone& zone( world.zone(zone_key) );
	OverWorld& overworld( model.overworld() );

	if (!world.objectExists( model.avatar() ))
		return;

	model.checkAliens();

	bool spitter_swarm = (1 == (rand() & 15));
	bool hugger_swarm = ((rand() & 15) < 3);

	bool quick(AHGameModel::Quick == model.gameType());

	int num = (quick ? 4 : 8) + (rand() & 7);

	WorldObject::WorldLocation player_loc( world.object( model.avatar() ).location() );

	int sz = (player_loc.z+20) < zone.sizeZ() ? (player_loc.z+20) : (player_loc.z-20);
	sz = (std::min)( ((int)zone.sizeZ())-1, (std::max)(0, sz) );

	for (; num > 0; --num)
	{
		aliens::Type type_to_spawn = aliens::Warrior;
		if (hugger_swarm)
		{
			type_to_spawn = (rand() & 7) ? aliens::Facehugger : aliens::Fasthugger;
		}
		else if (spitter_swarm)
		{
			type_to_spawn = (rand() & 3) ? aliens::Spitter : aliens::DeathSpitter;
		}
		else
		{
			type_to_spawn = (rand() & 3) ? aliens::Warrior : (rand() & 3) ? aliens::Elite : aliens::DeathSpitter;
		}

		shared_ptr< Alien > alien_obj( new Alien(type_to_spawn, true) );
		DBKeyValue alien_key( world.addWorldObject( alien_obj ) );

		for (int attempts = 20; attempts > 0; --attempts)
		{
			int tx( rand() % zone.sizeX() );
			int tz( sz );
			if (alien_obj->moveTo(model, WorldObject::WorldLocation( zone_key, tx, tz ), true))
				break;
		}

		for (int attempts = 20; attempts > 0; --attempts)
		{
			int tx( player_loc.x - 10 + ( rand() % 21 ));
			int tz( player_loc.z - 10 + ( rand() % 21 ));
			tx = (std::max)( 0, (std::min)( ((int)zone.sizeX())-1, tx ) );
			tz = (std::max)( 0, (std::min)( ((int)zone.sizeZ())-1, tz ) );
			if (terrain::Room == zone.terrainAt(tx,tz))
			{
				if (alien_obj->pickExitVent( world, zone, WorldObject::WorldLocation( zone_key, tx, tz ) ))
					break;
			}
		}
	}

	model.checkAliens();
}

void spawnAlien( AHGameModel& model, RL_shared::DBKeyValue zone_key, aliens::Type type_to_spawn, bool fast_forward )
{
	model.checkAliens();

	World& world( model.world() );
	Zone& zone( world.zone(zone_key) );

	if (aliens::isLargeAlien(type_to_spawn))
	{
		if (!fast_forward)
		{
			RL_shared::DBKeyValue avatar_zone = INVALID_KEY;
			if (world.objectExists(model.avatar()))
				avatar_zone = world.object(model.avatar()).location().zone;

			if (zone_key == avatar_zone)
			{
				type_to_spawn = aliens::Elite;
			}
			else
			{
				fast_forward = true;
			}
		}
	}

	shared_ptr< Alien > alien_obj( new Alien(type_to_spawn, true) );
	DBKeyValue alien_key( world.addWorldObject( alien_obj ) );

	for (int attempts = 100; attempts > 0; --attempts)
	{
		int tx( rand() % zone.sizeX() );
		int tz( rand() % zone.sizeZ() );
		if (alien_obj->moveTo(model, WorldObject::WorldLocation( zone_key, tx, tz ), true))
			break;
	}

	if (model.isQueenDead())
	{
		if (world.objectExists( model.avatar() ))
		{
			WorldObject::WorldLocation player_loc( world.object( model.avatar() ).location() );
			if (zone_key == player_loc.zone)
			{
				if (alien_obj->pickExitVent( world, zone, player_loc ))
				{
					model.checkAliens();
					return;
				}
			}
		}
	}

	for (int attempts = 100; attempts > 0; --attempts)
	{
		int tx( rand() % zone.sizeX() );
		int tz( rand() % zone.sizeZ() );
		if (terrain::Room == zone.terrainAt(tx,tz))
		{
			if (alien_obj->pickExitVent( world, zone, WorldObject::WorldLocation( zone_key, tx, tz ) ))
			{
				if (fast_forward)
				{
					shared_ptr< Alien > alien_obj_2( new Alien(type_to_spawn, false) );
					DBKeyValue alien_key_2( world.addWorldObject( alien_obj_2 ) );
					if (alien_obj_2->moveTo(model, WorldObject::WorldLocation( zone_key, tx, tz ), true))
					{
						world.removeWorldObject( alien_key );
						alien_obj.reset();
						break;
					}
				}
				else
					break;
			}
		}
	}

	model.checkAliens();
}

aliens::Type pickAlienType( AHGameModel& model, RL_shared::DBKeyValue zone_key )
{
	World& world( model.world() );
	Zone& zone( world.zone(zone_key) );
	OverWorld& overworld( model.overworld() );

	OverWorld::BlockAndFloor bnf = overworld.getBlockAndFloor(zone_key);
	if ((-1 == bnf.block) || (-1 == bnf.floor))
		return aliens::Juvenile;

	int level = overworld.level(bnf.block, bnf.floor);
	OverWorld::BlockType block_type = overworld.blockType(bnf.block);

	//1 in 16 chance of coming from a higher list
	if ((0 != level) && (0 == (rand() & 15)))
		level += 2;

	if (model.isQueenFound())
		level = 9;

	level = (std::max)(0, (std::min)(9, level));


	using namespace aliens;
	const int NUM_TYPES = 8;
	static const Type alien_types[10][NUM_TYPES] = 
	{
		{Juvenile, Juvenile, Juvenile, Juvenile, Juvenile, Juvenile, Juvenile, Juvenile},
		{Juvenile, Juvenile, Juvenile, Juvenile, Juvenile, Juvenile, Juvenile,  Harrier},
		{Juvenile, Juvenile, Juvenile, Juvenile,  Harrier,  Warrior,  Warrior,   Hunter},
		{Juvenile, Juvenile,  Harrier,  Harrier,  Warrior,  Warrior,   Hunter,   Hunter},
		{Juvenile,  Harrier,  Warrior,  Warrior,  Warrior,   Hunter,   Hunter,   Hunter},
		{Juvenile,  Spitter,  Warrior,  Warrior,   Hunter,   Hunter,   Hunter,    Elite},
		{Juvenile,  Spitter,  Warrior,  Warrior,   Hunter,   Hunter,    Elite,   Slayer},
		{Juvenile,  Spitter,  Warrior,   Hunter,   Hunter,    Elite,    Elite,   Slayer},
		{Juvenile,  Spitter,  Warrior,   Hunter,    Elite,    Elite,   Slayer,     Hulk},
		{Juvenile,  Spitter,  Warrior,   Hunter,    Elite,    Elite,   Slayer,     Hulk},
	};

	Type type_to_spawn = alien_types[level][rand() % NUM_TYPES];

	//add workers by special rule
	if ((Juvenile == type_to_spawn) && (0 == (rand() & 3)))
		type_to_spawn = Worker;
	//add huggers by special rule
	if (level >= 3)
	{
		if (OverWorld::Medical == block_type)
		{
			if (1 == (rand() & 3))
				type_to_spawn = Facehugger;
		}
		if ((OverWorld::Accommodation == block_type) || ((OverWorld::Reactor == block_type) && (bnf.floor > 1)))
		{
			if (1 == (rand() & 7))
				type_to_spawn = Facehugger;
		}
		if (Worker == type_to_spawn) 
		{
			if (
				((OverWorld::Reactor == block_type) && (1 == (rand() & 1)))
				|| (1 == (rand() & 7))
				)
				type_to_spawn = Facehugger;
		}
	}
	if ((0 == level) && (type_to_spawn != Worker))
		type_to_spawn = Juvenile;
	if ((model.isQueenDead()) && (Worker == type_to_spawn))
		type_to_spawn = (rand() & 1) ? Elite : Slayer;

	//rarer types
	if ((Facehugger == type_to_spawn) && (level >= 6) && (0 == (rand()&7)))
		type_to_spawn = Fasthugger;
	if (level > 0)
	{
		if ((Juvenile == type_to_spawn) && ((rand() % level) >= 3))
			type_to_spawn = Spitter;
		if ((type_to_spawn < Elite) && ((rand() % level) >= 6))
			type_to_spawn = DeathSpitter;
		if ((Warrior == type_to_spawn) && ((rand() & 31) < level))
			type_to_spawn = Gladiator;
		if ((Hunter == type_to_spawn) && ((rand() & 31) < (level-3)))
			type_to_spawn = Reaper;
	}

	return type_to_spawn;
}

void spawnAlien( AHGameModel& model, RL_shared::DBKeyValue zone_key, bool fast_forward )
{
	aliens::Type type = pickAlienType(model, zone_key);
	spawnAlien( model, zone_key, type, fast_forward );
}

void putTerminal( AHGameModel& model, Zone& zone, terminal::Type type, int x, int z, bool force_broken )
{
	World& world( model.world() );

	for( int iz = -2; iz < 3; ++iz )
		for ( int ix = -2; ix < 3; ++ix )
			zone.putTerrain( terrain::Room, x+ix, z+iz );

	zone.putTerrain( terrain::Wall, x, z );

	bool broke1(false), broke2(false), broke3(false), broke4(false);
	if (terminal::Master != type)
	{
		broke1 = force_broken || (0 != (rand() & 3));
		broke2 = force_broken || (0 != (rand() & 3));
		broke3 = force_broken || (0 != (rand() & 3));
		broke4 = force_broken || (0 != (rand() & 3));
	}

	shared_ptr< Terminal > term1( new Terminal( type, broke1 ) );
	DBKeyValue term1_key( world.addWorldObject( term1 ) );
	term1->moveTo( model, WorldObject::WorldLocation( zone.key(), x, z-1 ), true );

	shared_ptr< Terminal > term2( new Terminal( type, broke2 ) );
	DBKeyValue term2_key( world.addWorldObject( term2 ) );
	term2->moveTo( model, WorldObject::WorldLocation( zone.key(), x-1, z ), true );

	shared_ptr< Terminal > term3( new Terminal( type, broke3 ) );
	DBKeyValue term3_key( world.addWorldObject( term3 ) );
	term3->moveTo( model, WorldObject::WorldLocation( zone.key(), x+1, z ), true );

	shared_ptr< Terminal > term4( new Terminal( type, broke4 ) );
	DBKeyValue term4_key( world.addWorldObject( term4 ) );
	term4->moveTo( model, WorldObject::WorldLocation( zone.key(), x, z+1 ), true );
}

bool canPutLargeObject( Zone& zone, int x, int z, int min, int max )
{
	for( int iz = min; iz <= max; ++iz )
		for ( int ix = min; ix <= max; ++ix )
		{
			if (!terrain::isClear( zone.terrainAt(x+ix, z+iz)) )
				return false;

			if (!zone.objectsAt(x+ix, z+iz).empty())
				return false;
		}

	return true;
}

bool putTerminal( AHGameModel& model, Zone& zone, terminal::Type type, bool force_broken )
{
	int tx = rand() % zone.sizeX();
	int tz = rand() % zone.sizeZ();
	int max = zone.sizeX() * zone.sizeZ();

	for (; max > 0; --max)
	{
		++tx;
		if (tx >= zone.sizeX())
		{
			tx = 0;
			++tz;
			if (tz >= zone.sizeZ())
				tz = 0;
		}

		if (canPutLargeObject(zone, tx, tz, -2, 2))
		{
			putTerminal(model, zone, type, tx, tz, force_broken);
			return true;
		}
	}

	return false;
}

bool putHeavyMachinery( AHGameModel& model, Zone& zone )
{
	int tx = rand() % zone.sizeX();
	int tz = rand() % zone.sizeZ();
	int max = zone.sizeX() * zone.sizeZ();

	for (; max > 0; --max)
	{
		++tx;
		if (tx >= zone.sizeX())
		{
			tx = 0;
			++tz;
			if (tz >= zone.sizeZ())
				tz = 0;
		}

		if ((rand()&1) && canPutLargeObject(zone, tx, tz, -2, 2))
		{
			zone.putTerrain(terrain::Machinery, tx-1, tz  );
			zone.putTerrain(terrain::Machinery, tx  , tz+1);
			zone.putTerrain(terrain::Machinery, tx  , tz  );
			zone.putTerrain(terrain::Machinery, tx  , tz-1);
			zone.putTerrain(terrain::Machinery, tx+1, tz  );
			return true;
		}
		else if (canPutLargeObject(zone, tx, tz, -1, 2))
		{
			zone.putTerrain(terrain::Machinery, tx  , tz+1);
			zone.putTerrain(terrain::Machinery, tx  , tz  );
			zone.putTerrain(terrain::Machinery, tx+1, tz+1);
			zone.putTerrain(terrain::Machinery, tx+1, tz  );
			return true;
		}
	}

	return false;
}

void putStairs( AHGameModel& model, Zone& floor_zone, Zone* floor_below, int floor, TerrainType stairs, TerrainType broken_stairs, 
				bool is_reactor_entrance, bool is_reactor )
{
	using namespace terrain;

	int stairlowx(0), stairlowz(0), stairhix(0), stairhiz(0);

	if (((floor & 1) && (StairsDown == stairs))
		|| ((0 == (floor & 1)) && (StairsUp == stairs)))
	{
		stairlowx = 1;
		stairlowz = 1;
		stairhix = floor_zone.sizeX()-2;
		stairhiz = floor_zone.sizeZ()-2;
	}
	else
	{
		stairlowx = 2;
		stairlowz = 2;
		stairhix = floor_zone.sizeX()-3;
		stairhiz = floor_zone.sizeZ()-3;
	}

	if (floor_below && (StairsDown == stairs))
	{
		floor_zone.putTerrain( isBroken( floor_below->terrainAt(stairlowx, stairlowz) ) ? broken_stairs : stairs, stairlowx, stairlowz);
		floor_zone.putTerrain( isBroken( floor_below->terrainAt(stairlowx, stairhiz) ) ? broken_stairs : stairs, stairlowx, stairhiz);
		floor_zone.putTerrain( isBroken( floor_below->terrainAt(stairhix, stairlowz) ) ? broken_stairs : stairs, stairhix, stairlowz);
		floor_zone.putTerrain( isBroken( floor_below->terrainAt(stairhix, stairhiz) ) ? broken_stairs : stairs, stairhix, stairhiz);
	}
	else
	{

		floor_zone.putTerrain(stairs, stairlowx, stairlowz);
		floor_zone.putTerrain(stairs, stairlowx, stairhiz);
		floor_zone.putTerrain(stairs, stairhix, stairlowz);
		floor_zone.putTerrain(stairs, stairhix, stairhiz);

		//For evulz, break alternate stairs in the reactor to stymie lucky quick ascent.
		//(TODO: Continuous stairwells can still occur at the SE corner, making it favourable). 
		//NW and NE stairs always broken at reactor entrance.
		bool break_NW(is_reactor_entrance), break_NE(is_reactor_entrance), break_SW(false), break_SE(false);
		if (is_reactor && (floor > GROUND_FLOOR) && (StairsUp == stairs))
		{
			if (floor & 1)
			{
				break_NW |= (StairsDown == floor_zone.terrainAt( 1, floor_zone.sizeZ()-2 ) );
				break_NE |= (StairsDown == floor_zone.terrainAt( floor_zone.sizeX()-2, floor_zone.sizeZ()-2 ) );
				break_SW |= (StairsDown == floor_zone.terrainAt( 1, 1 ) );
				break_SE |= (StairsDown == floor_zone.terrainAt( floor_zone.sizeX()-2, 1 ) );
			}
			else
			{
				break_NW |= (StairsDown == floor_zone.terrainAt( 2, floor_zone.sizeZ()-3 ) );
				break_NE |= (StairsDown == floor_zone.terrainAt( floor_zone.sizeX()-3, floor_zone.sizeZ()-3 ) );
				break_SW |= (StairsDown == floor_zone.terrainAt( 2, 2 ) );
				break_SE |= (StairsDown == floor_zone.terrainAt( floor_zone.sizeX()-3, 2 ) );
			}
		}

		int num_broken_stairs(0);
		if ((rand() & 1) || break_NW)
		{
			++num_broken_stairs;
			floor_zone.putTerrain(broken_stairs, stairlowx, stairhiz);
		}
		if ((rand() & 1) || break_NE)
		{
			++num_broken_stairs;
			floor_zone.putTerrain(broken_stairs, stairhix, stairhiz);
		}
		if ((rand() & 1) || break_SW)
		{
			++num_broken_stairs;
			floor_zone.putTerrain(broken_stairs, stairlowx, stairlowz);
		}
		if (((rand() & 1) || break_SE) && (num_broken_stairs < 3))
		{
			++num_broken_stairs;
			floor_zone.putTerrain(broken_stairs, stairhix, stairlowz);
		}
	}
}

bool tryPutBridge(Zone& zone, int tx, int tz, int adjx, int adjz, int offx, int offz, bool blocked)
{
	if ( terrain::Wall == zone.terrainAt(tx, tz) )
		if ( terrain::isClear( zone.terrainAt(tx+offx, tz+offz) ) )
			if ( terrain::Wall == zone.terrainAt(tx+adjx, tz+adjz) )
				if ( terrain::isClear( zone.terrainAt(tx+adjx+offx, tz+adjz+offz) ) )
				{
					if (blocked)
					{
						zone.putTerrain( terrain::BlastDoor, tx, tz );
						zone.putTerrain( terrain::BlastDoor, tx+adjx, tz+adjz );
					}
					else
					{
						zone.putTerrain( terrain::Bridge, tx, tz );
						zone.putTerrain( terrain::Bridge, tx+adjx, tz+adjz );
					}
					return true;
				}
	return false;
}

void putBridge( Zone& zone, int inx, int inz, int adjx, int adjz, int offx, int offz, bool blocked )
{
	int advx(adjx), advz(adjz);
	if (rand() & 1)
	{
		advx = -adjx;
		advz = -adjz;
	}

	int tx1(inx);
	int tz1(inz);
	int tx2(inx);
	int tz2(inz);
	for(;;)
	{
		if (zone.isWithin(tx1,tz1) && zone.isWithin(tx1+adjx, tz1+adjz))
		{
			if (tryPutBridge(zone, tx1, tz1, adjx, adjz, offx, offz, blocked))
				return;
		}
		else
			break;

		if (zone.isWithin(tx2,tz2) && zone.isWithin(tx2+adjx, tz2+adjz))
		{
			if (tryPutBridge(zone, tx2, tz2, adjx, adjz, offx, offz, blocked))
				return;
		}
		else
			break;

		tx1 += advx;
		tz1 += advz;
		tx2 -= advx;
		tz2 -= advz;
	}
}

void spawnCorpse( AHGameModel& model, Zone& zone, int x, int z, bool blood_only )
{
	World& world( model.world() );
	if (!blood_only)
	{
		shared_ptr< Decoration > corpse_obj( new Decoration( decoration::Corpse ) );
		DBKeyValue corpse_key( world.addWorldObject( corpse_obj ) );
		int tx = (rand()%3) - 1;
		int tz = (rand()%3) - 1;
		int count = 9;
		for (;count > 0; --count)
		{
			if (terrain::Room == zone.terrainAt(x+tx, z+tz))
				if (corpse_obj->moveTo(model, WorldObject::WorldLocation( zone.key(), x+tx, z+tz ), true))
					break;

			++tx;
			if (tx > 1)
			{
				tx = -1;
				++tz;
			}
			if (tz > 1)
				tz = -1;
		}
		if (0 == count)
			world.removeWorldObject(corpse_key);
	}

	int numblood = rand() & 3;
	for (; numblood > 0; --numblood)
	{
		shared_ptr< Decoration > blood_obj( new Decoration( decoration::Blood ) );
		DBKeyValue blood_key( world.addWorldObject( blood_obj ) );
		int tx = (rand()%3) - 1;
		int tz = (rand()%3) - 1;
		if (terrain::Room == zone.terrainAt(x+tx, z+tz))
			if (blood_obj->moveTo(model, WorldObject::WorldLocation( zone.key(), x+tx, z+tz ), true))
				continue;
		world.removeWorldObject(blood_key);
	}
}

void spawnAlienCorpses( AHGameModel& model, Zone& zone, int num )
{
	World& world( model.world() );

	int attempts = num * 3;

	while ((--attempts > 0) && (num > 0))
	{
		int tx = 1+(rand()%(zone.sizeX()-1));
		int tz = 1+(rand()%(zone.sizeZ()-1));

		if (terrain::Room == zone.terrainAt(tx, tz))
		{
			shared_ptr< Decoration > corpse_obj( new Decoration( decoration::AlienCorpse ) );
			DBKeyValue corpse_key( world.addWorldObject( corpse_obj ) );

			if (corpse_obj->moveTo(model, WorldObject::WorldLocation( zone.key(), tx, tz ), true))
			{
				--num;
			}
			else
			{
				world.removeWorldObject(corpse_key);
			}
		}
	}
}


bool canPlaceFurniture(Zone& floor_zone, int tx, int tz)
{
	return	floor_zone.isWithin(tx, tz)
			&& (terrain::Room == floor_zone.terrainAt(tx,tz))
			&& (floor_zone.objectsAt(tx, tz).empty())
			&& (terrain::Door != (terrain::getType(floor_zone.terrainAt(tx-1,tz))))
			&& (terrain::Door != (terrain::getType(floor_zone.terrainAt(tx+1,tz))))
			&& (terrain::Door != (terrain::getType(floor_zone.terrainAt(tx,tz-1))))
			&& (terrain::Door != (terrain::getType(floor_zone.terrainAt(tx,tz+1))))
			&& (terrain::Bridge != floor_zone.terrainAt(tx-1,tz))
			&& (terrain::Bridge != floor_zone.terrainAt(tx+1,tz))
			&& (terrain::Bridge != floor_zone.terrainAt(tx,tz-1))
			&& (terrain::Bridge != floor_zone.terrainAt(tx,tz+1));
}

//TODO move to Furniture
bool isBlockingFurniture(furniture::Type type)
{
	switch(type)
	{
		case furniture::CrateStack:
		case furniture::Machinery:
		case furniture::Table:
		case furniture::Desk:
		case furniture::Sofa:
		case furniture::Bed:
		case furniture::Workbench:
		case furniture::Hopper:
			return true;
		default:;
	}
	return false;
}

bool isBlocked(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	if (!zone.isWithin(loc.x, loc.z))
		return true;
	if (!terrain::isClear(zone.terrainAt(loc.x, loc.z)))
		return true;
	BOOST_FOREACH(DBKeyValue obj_key, zone.objectsAt(loc.x, loc.z))
	{
		if (world.objectExists(obj_key))
		{
			const AHGameObject& game_obj( dynamic_cast<const AHGameObject&>( world.object(obj_key) ) );
			WorldObjectType type = game_obj.type();
			if (objects::Barrel == type)
				return true;
			if (objects::Furniture == type)
			{
				if (isBlockingFurniture(dynamic_cast<const Furniture&>(game_obj).furnitureType()))
					return true;
			}
		}
	}

	return false;
}

int numBlockedSides(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	return (isBlocked(model, world, zone, WorldObject::WorldLocation(zone.key(), loc.x-1,loc.z)) ? 1 : 0)
		 + (isBlocked(model, world, zone, WorldObject::WorldLocation(zone.key(), loc.x+1,loc.z)) ? 1 : 0)
		 + (isBlocked(model, world, zone, WorldObject::WorldLocation(zone.key(), loc.x,loc.z-1)) ? 1 : 0)
		 + (isBlocked(model, world, zone, WorldObject::WorldLocation(zone.key(), loc.x,loc.z+1)) ? 1 : 0);
}

bool isWallAdjacent(Zone& zone, WorldObject::WorldLocation loc)
{
	if ((0 == loc.x) || (0 == loc.z) || (zone.sizeX()-1 == loc.x) || (zone.sizeZ()-1 == loc.z))
		return true;

	return
		  ( (terrain::Wall == (terrain::getType(zone.terrainAt(loc.x-1,loc.z))))
		 || (terrain::Wall == (terrain::getType(zone.terrainAt(loc.x+1,loc.z))))
		 || (terrain::Wall == (terrain::getType(zone.terrainAt(loc.x,loc.z-1))))
		 || (terrain::Wall == (terrain::getType(zone.terrainAt(loc.x,loc.z+1)))) );
}

typedef bool (*CreateObjectFunc)(AHGameModel&, World&, Zone&, WorldObject::WorldLocation);

void spawnObjects(CreateObjectFunc func, AHGameModel& model, Zone& floor_zone, int num)
{
	World& world( model.world() );

	int iterations = 5 * num;

	while (num && iterations)
	{
		int tx( rand() % floor_zone.sizeX() );
		int tz( rand() % floor_zone.sizeZ() );
		tx = (std::max)(1, (std::min)(tx, floor_zone.sizeX()-2));
		tz = (std::max)(1, (std::min)(tz, floor_zone.sizeZ()-2));
		while (num && iterations)
		{
			--iterations;
			if (canPlaceFurniture(floor_zone, tx, tz))
			{
				bool ok = func(model, world, floor_zone, WorldObject::WorldLocation( floor_zone.key(), tx, tz ));

				if (ok)
					--num;

				if (rand()&3)
				{
					do
					{
						int dx = (rand()%2)-1;
						int dz = (rand()%2)-1;
						dx = (rand()&1) ? ((dz != 0) ? 0 : dx) : dx;
						dz = (dx != 0) ? 0 : dz;
						tx += dx;
						tz += dz;
					} while (0 == (rand()&3));
					continue;
				}
			}
			break;
		}
	}
}

bool spawnBarrel(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	if (numBlockedSides(model, world, zone, loc) > 1)
		return false;

	shared_ptr< Barrel > barrel_obj( new Barrel() );
	DBKeyValue barrel_key( world.addWorldObject( barrel_obj ) );
	if (!barrel_obj->moveTo(model, loc, true))
	{
		world.removeWorldObject( barrel_key );
		return false;
	}
	return true;
}

bool spawnFurniture(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc, furniture::Type type, bool broken)
{
	if (isBlockingFurniture(type) && (numBlockedSides(model, world, zone, loc) > 1))
		return false;

	shared_ptr< Furniture > obj( new Furniture(type, broken) );
	DBKeyValue key( world.addWorldObject( obj ) );
	if (!obj->moveTo(model, loc, true))
	{
		world.removeWorldObject( key );
		return false;
	}
	return true;
}

bool spawnCrate(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	furniture::Type type( (rand() & 1) ? furniture::Crate : furniture::CrateStack );
	return spawnFurniture(model, world, zone, loc, type, false);
}

bool spawnWorkshop(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	furniture::Type type = furniture::Workbench;

	if (isWallAdjacent(zone, loc))
	{
		switch (rand() & 7)
		{
			case 0: type = furniture::Workbench; break;
			case 1: type = furniture::Hopper; break;
			case 2: type = furniture::Junk; break;
			case 3: type = furniture::Locker; break;
			default:
				type = furniture::Piping;
		}
	}
	else if (rand()&1)
		return false;

	bool broken( (furniture::Workbench == type) && (0 == (rand() & 3)) );
	bool success = spawnFurniture(model, world, zone, loc, type, broken);
	return success;
}

bool spawnReactor(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	furniture::Type type = furniture::None;

	if (isWallAdjacent(zone, loc))
	{
		switch (rand() & 7)
		{
			case 0: type = furniture::Locker; break;
			case 1:
			case 2:
			case 3: type = furniture::Junk; break;
			default:
				type = furniture::Piping;
		}
	}
	else
		return false;

	bool success = spawnFurniture(model, world, zone, loc, type, false);
	return success;
}

bool spawnFurnishings(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc, furniture::Type type)
{
	if ( ((furniture::Locker == type) || (furniture::Desk == type) || (furniture::Junk == type))
		 && (!isWallAdjacent(zone, loc)))
	{
		type = (rand() & 1) ? furniture::Table : furniture::Chair;
	}

	if (isBlockingFurniture(type) && (numBlockedSides(model, world, zone, loc) > 1))
		type = furniture::Chair;

	bool broken( (furniture::Junk != type) && (0 == (rand() & 3)) );

	return spawnFurniture(model, world, zone, loc, type, broken);
}

bool spawnOffice(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	furniture::Type type( (furniture::Type)((int)furniture::Locker + (rand() % 6)) );

	if (isWallAdjacent(zone, loc))
	{
		switch(rand() & 3)
		{
			case 0:	type = furniture::Desk; break;
			case 1: type = furniture::Locker; break;
			default:;
		}
	}

	bool success = spawnFurnishings(model, world, zone, loc, type);
	return success;
}
bool spawnMedical(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	furniture::Type type( (furniture::Type)((int)furniture::Locker + (rand() % 6)) );

	//if (isWallAdjacent(zone, loc))
	//{
	//	if (rand() & 1)
	//		type = furniture::Bed;
	//}

	bool success = spawnFurnishings(model, world, zone, loc, type);
	return success;
}
bool spawnResidential(AHGameModel& model, World& world, Zone& zone, WorldObject::WorldLocation loc)
{
	furniture::Type type( (furniture::Type)((int)furniture::Locker + (rand() % 6)) );

	//if (isWallAdjacent(zone, loc))
	//{
	//	if (0 == (rand() & 7))
	//		type = furniture::Bed;
	//}

	bool success = spawnFurnishings(model, world, zone, loc, type);
	return success;
}

void spawnPickups(AHGameModel& model, World& world, Zone& floor_zone, int num_pickups, int level, OverWorld::BlockType block_type)
{
	using namespace pickup;
	const int LIST_SIZE = 14;
	const Type pickup_types[10][LIST_SIZE] = 
	{
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit, FragGrenade, FragGrenade, Ammo_9mm,    Ammo_9mm,   Ammo_9mm, Ammo_shotgun,  Ammo_shotgun, FragGrenade}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit, FragGrenade, KrakGrenade, Ammo_9mm,    Ammo_9mm,   Ammo_9mm, Ammo_shotgun,  Ammo_shotgun, FragGrenade}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit,  IncGrenade, KrakGrenade, Ammo_colt,   Ammo_9mm,   Ammo_9mm,     Ammo_9mm,  Ammo_shotgun, FragGrenade}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit,  IncGrenade, KrakGrenade, Ammo_colt,   Ammo_9mm,   Ammo_9mm, Ammo_shotgun,  Ammo_shotgun, FragGrenade}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit,  IncGrenade, KrakGrenade, Ammo_colt, Ammo_flame,   Ammo_9mm,   Ammo_pulse,  Ammo_shotgun, FragGrenade}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit,  IncGrenade, KrakGrenade, Ammo_colt, Ammo_flame,   Ammo_9mm,   Ammo_pulse,  Ammo_shotgun, FragGrenade}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit,  IncGrenade, KrakGrenade, Ammo_colt, Ammo_flame, Ammo_laser,   Ammo_pulse,      Ammo_9mm, Ammo_shotgun}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit,  IncGrenade, KrakGrenade, Ammo_colt, Ammo_flame, Ammo_laser,   Ammo_pulse,      Ammo_9mm, Ammo_shotgun}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit,  IncGrenade, KrakGrenade, Ammo_colt, Ammo_flame, Ammo_laser,   Ammo_pulse, Ammo_smartgun, Ammo_HEDP30mm}, 
		{Ammo_9mm, Ammo_shotgun, FragGrenade, StunGrenade, Neutraliser, Medkit,  IncGrenade, KrakGrenade, Ammo_colt, Ammo_flame, Ammo_laser,   Ammo_pulse, Ammo_smartgun, Ammo_HEDP30mm}, 
	};

	for (int num=num_pickups; num>0;)
	{
		int pick_level = level;
		if (0 == (rand() & 15))
			pick_level += (rand() & 7);
		if (0 == (rand() & 31))
			pick_level = 9;
		pick_level = (std::max)(0, (std::min)(9, pick_level));
		Type type = pickup_types[pick_level][rand() % LIST_SIZE];

		//Special adjustment rule: medkits are too common for their usefulness, and 9mm not common enough.
		if ((Medkit == type) && (rand() & 1))
			type = Ammo_9mm;

		{ //special block rules
			if (OverWorld::Security == block_type)
			{
				if (0 == (rand() & 7))
					type = StunGrenade;
				else if (0 == (rand() & 15))
					type = Ammo_9mm;
				else if (0 == (rand() & 15))
					type = Ammo_shotgun;
				else if (0 == (rand() & 15))
					type = Ammo_colt;
				else if (0 == (rand() & 31))
					type = Ammo_pulse;
				else if (0 == (rand() & 31))
					type = Ammo_laser;
				else if (0 == (rand() & 31))
					type = Ammo_HEDP30mm;
			}
			if (OverWorld::Medical == block_type)
			{
				if (0 == (rand() & 7))
					type = Medkit;
			}
			if ((OverWorld::Workshop == block_type) || (OverWorld::Engineering == block_type))
			{
				if (0 == (rand() & 7))
					type = Neutraliser;
				else if (0 == (rand() & 15))
					type = Medkit;
				else if (0 == (rand() & 31))
					type = DemoCharge;
			}
			if (OverWorld::Storage == block_type)
			{
				if (0 == (rand() & 7))
					type = Ammo_9mm;
				else if (0 == (rand() & 7))
					type = Ammo_shotgun;
				else if (0 == (rand() & 7))
					type = Ammo_colt;
				else if (0 == (rand() & 15))
					type = Ammo_flame;
				else if (0 == (rand() & 31))
					type = DemoCharge;
			}
			if ((OverWorld::Accommodation == block_type) || (OverWorld::Administration == block_type))
			{
				int mega_chance = (OverWorld::Administration == block_type) ? 7 : 15;
				if (0 == (rand() & 3))
					type = (0 == (rand()&mega_chance)) ? MegaCredChip : CreditChip;
			}
		}

		shared_ptr< Pickup > item_obj( new Pickup(type) );
		DBKeyValue item_key( world.addWorldObject( item_obj ) );

		for (int attempts = 50; attempts > 0; --attempts)
		{
			int tx( rand() % floor_zone.sizeX() );
			int tz( rand() % floor_zone.sizeZ() );
			if ((terrain::Room == floor_zone.terrainAt(tx,tz)) && floor_zone.objectsAt(tx,tz).empty())
			{
				bool put( item_obj->moveTo(model, WorldObject::WorldLocation(floor_zone.key(), tx, tz), true) );
				if (put)
				{
					--num;

					if (0 == (rand() & 7))
						spawnCorpse(model, floor_zone, tx, tz, false);
					else if (0 == (rand() & 7))
						spawnCorpse(model, floor_zone, tx, tz, true);

					break;
				}
			}
		}
	}
}

void spawnArmours(AHGameModel& model, World& world, Zone& floor_zone, int num_armours, int level, OverWorld::BlockType block_type)
{
	using namespace armour;
	const int LIST_SIZE = 4;
	const Type armour_types[10][LIST_SIZE] = 
	{
		{FlakJacket, FlakJacket,   FlakJacket, RiotArmour},
		{FlakJacket, FlakJacket,   RiotArmour, RiotArmour},
		{FlakJacket, RiotArmour,   RiotArmour, CombatArmour},
		{FlakJacket, RiotArmour,   RiotArmour, CombatArmour},
		{FlakJacket, RiotArmour, CombatArmour, CombatArmour},
		{FlakJacket, RiotArmour, CombatArmour, CombatArmour},
		{FlakJacket, RiotArmour, CombatArmour, CombatArmour},
		{FlakJacket, RiotArmour, CombatArmour, PlasteelArmour},
		{FlakJacket, RiotArmour, CombatArmour, PlasteelArmour},
		{FlakJacket, RiotArmour, CombatArmour, PlasteelArmour},
	};
	const int armour_cond[10][LIST_SIZE] = 
	{
		{100, 100, 100,  60},
		{100, 100,  60,  50},
		{100,  60,  60,  30},
		{100,  60,  80,  60},
		{100,  80,  30,  80},
		{100, 100,  60, 100},
		{100, 100,  80, 100},
		{100, 100, 100,  40},
		{100, 100, 100,  80},
		{100, 100, 100, 100}
	};

	for (int num=num_armours; num>0;)
	{
		int pick_level = level;
		if (0 == (rand() & 15))
			pick_level += (rand() & 7);
		if (0 == (rand() & 31))
			pick_level = 9;
		pick_level = (std::max)(0, (std::min)(9, pick_level));

		int pick = rand() % LIST_SIZE;
		Type type = armour_types[pick_level][pick];
		int condition = (armour_cond[pick_level][pick]);
		condition = (condition/2) + 1 + (rand() % (condition/2));

		//Block-specific rules for special armour types
		if ((OverWorld::Security == block_type) || (OverWorld::Reactor == block_type))
		{
			if (0 == (rand() & 31))
			{
				type = ChemhazardArmour;
				condition = (std::min)(100, (std::max)(25, (2*pick_level)+(rand() % 100)));
			}
			else if (0 == (rand() & 63))
			{
				type = PowerArmour;
				condition = (std::min)(100, (std::max)(25, (2*pick_level)+(rand() % 100)));
			}
			else if (OverWorld::Reactor == block_type)
			{
				if ((1 == (rand() & 3)) && ((FlakJacket == type) || (condition < 50)))
				{
					type = ChemhazardSuit;
					condition = 100;
				}
			}
		}
		else if ((OverWorld::Workshop == block_type) || (OverWorld::Engineering == block_type))
		{
			if ((rand() & 1) && ((FlakJacket == type) || (condition < 50)))
			{
				type = ChemhazardSuit;
				condition = 100;
			}
		}

		if ((CombatArmour == type) && ((rand()&15)+3 < pick_level))
			type = AdvCombatArmour;
		if ((PlasteelArmour == type) && (1==(rand()&3)))
			type = AdvCombatArmour;

		condition = (std::max)(25, condition);
		if ((FlakJacket == type) && (level > 4))
			condition = (std::min)(100, condition+50);

		int special_chance = 63;
		if (OverWorld::Workshop == block_type)
			special_chance = 15;
		if (1 == (rand() & special_chance))
		{
			type = (armour::Type)(type | armour::HP);
			condition = 75 + (rand() % 26);
		}
		if (1 == (rand() & special_chance))
			type = (armour::Type)(type | armour::Value);
		if (1 == (rand() & special_chance))
			type = (armour::Type)(type | armour::Weight);

		shared_ptr< Armour > item_obj( new Armour(type, condition) );
		DBKeyValue item_key( world.addWorldObject( item_obj ) );

		for (int attempts = 50; attempts > 0; --attempts)
		{
			int tx( rand() % floor_zone.sizeX() );
			int tz( rand() % floor_zone.sizeZ() );
			if ((terrain::Room == floor_zone.terrainAt(tx,tz)) && floor_zone.objectsAt(tx,tz).empty())
			{
				bool put( item_obj->moveTo(model, WorldObject::WorldLocation(floor_zone.key(), tx, tz), true) );
				if (put)
				{
					--num;

					if (0 == (rand() & 1))
						spawnCorpse(model, floor_zone, tx, tz, false);
					else if (0 == (rand() & 1))
						spawnCorpse(model, floor_zone, tx, tz, true);

					break;
				}
			}
		}
	}
}

void spawnWeapons(AHGameModel& model, World& world, Zone& floor_zone, int num_weapons, int level, OverWorld::BlockType block_type)
{
	using namespace weapon;
	const int LIST_SIZE = 11;
	const Type weapon_types[10][LIST_SIZE] = 
	{
		{Pistol, SMG, CombatShotgun,     Pistol, SawnoffShotgun, CombatShotgun, Pistol,    SMG, SawnoffShotgun, CombatShotgun,           SMG},
		{Pistol, SMG, CombatShotgun,     Pistol, SawnoffShotgun, CombatShotgun,   Colt,    SMG, SawnoffShotgun, CombatShotgun,           SMG},
		{Pistol, SMG, MachinePistol, PulseRifle, SawnoffShotgun, CombatShotgun,   Colt,    SMG, SawnoffShotgun, CombatShotgun,           SMG},
		{Pistol, SMG, MachinePistol, PulseRifle, SawnoffShotgun, CombatShotgun,   Colt, Flamer, SawnoffShotgun, CombatShotgun,           SMG},
		{Pistol, SMG, MachinePistol, PulseRifle, SawnoffShotgun, CombatShotgun,   Colt, Flamer, SawnoffShotgun, CombatShotgun,        Pistol},
		{Pistol, SMG, MachinePistol, PulseRifle, SawnoffShotgun, CombatShotgun,   Colt, Flamer, SawnoffShotgun,         Laser, MachinePistol},
		{Pistol, SMG, MachinePistol, PulseRifle, SawnoffShotgun, CombatShotgun,   Colt, Flamer,       SmartGun,         Laser,          Colt},
		{Pistol, SMG, MachinePistol, PulseRifle, SawnoffShotgun, CombatShotgun,   Colt, Flamer,       SmartGun,         Laser,    PulseRifle},
		{Pistol, SMG, MachinePistol, PulseRifle, SawnoffShotgun, CombatShotgun,   Colt, Flamer,       SmartGun,         Laser, CanisterRifle},
		{Pistol, SMG, MachinePistol, PulseRifle, SawnoffShotgun, CombatShotgun,   Colt, Flamer,       SmartGun,         Laser, CanisterRifle},
	};
	const int weapon_ammo[10][LIST_SIZE] = 
	{
		{100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
		{100, 100, 100, 100, 100, 100,  50, 100, 100, 100, 100},
		{100, 100,  25,  25, 100, 100, 100, 100, 100, 100, 100},
		{100, 100,  50,  50, 100, 100, 100,  25, 100, 100, 100},
		{100, 100,  75,  75, 100, 100, 100,  50, 100, 100,  50},
		{100, 100, 100, 100, 100, 100, 100,  75,  25, 100,  75},
		{100, 100, 100, 100, 100, 100, 100, 100,  50,  25, 100},
		{100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  25},
		{100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50},
		{100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
	};

	for (int num=num_weapons; num>0;)
	{
		int pick_level = level;
		if (0 == (rand() & 15))
			pick_level += (rand() & 7);
		if (0 == (rand() & 31))
			pick_level = 9;
		pick_level = (std::max)(0, (std::min)(9, pick_level));

		int pick = rand() % LIST_SIZE;
		Type type = weapon_types[pick_level][pick];

		Special spec = Special_None;
		int special_chance = 
			(block_type == OverWorld::Workshop) ? 3 : 
			(block_type == OverWorld::Security) ? 7 : 
			(block_type == OverWorld::Reactor ) ? 7 : 15;
		if (1 == (rand() & special_chance))
		{
			if (1 == (rand() & 7))
				spec = (Special)(spec | Clip);
			if (2 == (rand() & 7))
				spec = (Special)(spec | Weight);
			if (3 == (rand() & 7))
				spec = (Special)(spec | Damage);
			if (4 == (rand() & 7))
				spec = (Special)(spec | Accuracy);
			if (5 == (rand() & 7))
				spec = (Special)(spec | Rate);
			if (6 == (rand() & 7))
				spec = (Special)(spec | Reload);
			spec = (Special)(spec & weapon_data::getWeaponData(type, Special_None).allowed_mods);
		}

		int ammo_percent = rand() % (1 + (weapon_ammo[pick_level][pick]));
		int ammo = (getClipSize(type, spec) * ammo_percent) / 100;

		shared_ptr< Weapon > item_obj( new Weapon(type, spec, ammo) );
		DBKeyValue item_key( world.addWorldObject( item_obj ) );

		for (int attempts = 50; attempts > 0; --attempts)
		{
			int tx( rand() % floor_zone.sizeX() );
			int tz( rand() % floor_zone.sizeZ() );
			if ((terrain::Room == floor_zone.terrainAt(tx,tz)) && floor_zone.objectsAt(tx,tz).empty())
			{
				bool put( item_obj->moveTo(model, WorldObject::WorldLocation(floor_zone.key(), tx, tz), true) );
				if (put)
				{
					--num;

					if (0 == (rand() & 1))
						spawnCorpse(model, floor_zone, tx, tz, false);
					else if (0 == (rand() & 1))
						spawnCorpse(model, floor_zone, tx, tz, true);

					break;
				}
			}
		}
	}
}

void spawnEggs( AHGameModel& model, World& world, Zone& zone, int number )
{
	int count = 5*number;

	while (number && count)
	{
		shared_ptr<Egg> egg_obj( new Egg() );
		DBKeyValue egg_key( world.addWorldObject(egg_obj) );

		while(--count)
		{
			int tx( (zone.sizeX()/4) + (rand() % (zone.sizeX()/2)) );
			int tz( (zone.sizeZ()/4) + (rand() % (zone.sizeZ()/2)) );
			if ((terrain::Room == zone.terrainAt(tx,tz)) && zone.objectsAt(tx,tz).empty())
			{
				bool put( egg_obj->moveTo(model, WorldObject::WorldLocation(zone.key(), tx, tz), true) );
				if (put)
				{
					--number;
					break;
				}
			}
		}
	}
}

int adjustNumber( bool quick, bool is_reactor, int number )
{
	if (quick)
		if (is_reactor)
			return 1+((number*(QUICK_REACTOR_SIZE*QUICK_REACTOR_SIZE))/(NORMAL_REACTOR_SIZE*NORMAL_REACTOR_SIZE));
		else
			return 1+((number*(QUICK_NORMAL_SIZE*QUICK_NORMAL_SIZE))/(NORMAL_NORMAL_SIZE*NORMAL_NORMAL_SIZE));
	return number;
}

RL_shared::MapStyles::MapStyle getStyleForBlockType(OverWorld::BlockType type, int floor, int top_floor)
{
	RL_shared::MapStyles::MapStyle room_type = RL_shared::MapStyles::RoomMess;
    switch (type)
    {
		case OverWorld::Accommodation: 
		case OverWorld::Medical: 
		case OverWorld::Administration: 
		case OverWorld::Security: 
			if (rand() & 3) 
				room_type = RL_shared::MapStyles::CorridorMess;
		case OverWorld::Storage: 
		case OverWorld::Workshop: 
		case OverWorld::Engineering: 
		case OverWorld::Reactor: 
		default:;
	}
	if (RL_shared::MapStyles::RoomMess == room_type)
	{
		if (0 == (rand() & 3))
			room_type = rand() & 1 ? RL_shared::MapStyles::HFrame : RL_shared::MapStyles::IFrame;
		if ((OverWorld::Reactor == type) && floor < top_floor)
			room_type = rand() & 1 ? room_type : RL_shared::MapStyles::Airlocks;
	}
    return room_type;
}

void populateFloor( AHGameModel& model, Zone& floor_zone, Zone* floor_below, int block, int floor, int top_floor )
{
	World& world( model.world() );
	OverWorld& overworld( model.overworld() );

	bool quick(AHGameModel::Quick == model.gameType());

	MapCreator create_map( floor_zone );
	create_map.create( getStyleForBlockType(overworld.blockType(block), floor, top_floor) );

	if ((9 == block) && (3 == floor))
	{
		for (int z = floor_zone.sizeZ()/4; z < (3*(floor_zone.sizeZ()/4)); ++z)
			for (int x = floor_zone.sizeX()/4; x < (3*(floor_zone.sizeX()/4)); ++x)
				if (terrain::getType((floor_zone.terrainAt(x, z))) == terrain::Door)
					floor_zone.putTerrain( floor_zone.terrainAt(x, z) | terrain::Broken, x, z);
	}

	if (floor > GROUND_FLOOR)
	{
		putStairs(model, floor_zone, floor_below, floor, terrain::StairsDown, terrain::BrokenStairsDown, false, 9==block);
	}
	if (floor < top_floor)
	{
		putStairs(model, floor_zone, floor_below, floor, terrain::StairsUp, terrain::BrokenStairsUp, ((7 == block) && (0 == floor)), 9==block);
	}

	//Bridges:
	//West
	if (-1 != overworld.exitsToBlock(block, floor, OverWorld::West))
		putBridge( floor_zone, 0, floor_zone.sizeZ()/2, 0, -1, 1, 0, false );
	//East
	if (-1 != overworld.exitsToBlock(block, floor, OverWorld::East))
		putBridge( floor_zone, floor_zone.sizeX()-1, floor_zone.sizeZ()/2, 0, -1, -1, 0, false );
	//South
	//Block 1, floor 0 gets a special case as the 'bridge' there is the map entrance.
	if ((-1 != overworld.exitsToBlock(block, floor, OverWorld::South))
		|| ( (1 == block) && (0 == floor) )
		)
		putBridge( floor_zone, floor_zone.sizeX()/2, 0, -1, 0, 0, 1, false );
	//North
	if (-1 != overworld.exitsToBlock(block, floor, OverWorld::North))
		putBridge( floor_zone, floor_zone.sizeX()/2, floor_zone.sizeZ()-1, -1, 0, 0, -1, (7 == block) );


	if ((2 == floor) && (OverWorld::Administration == overworld.blockType(block)))
	{
		if (!putTerminal(model, floor_zone, terminal::Master, false))
		{
			putTerminal(model, floor_zone, terminal::Master, floor_zone.sizeX()/2, floor_zone.sizeZ()/2, false);
		}
		else
		{
			putTerminal(model, floor_zone, terminal::Master, false);
			putTerminal(model, floor_zone, terminal::Master, false);
			putTerminal(model, floor_zone, terminal::Master, false);
			putTerminal(model, floor_zone, terminal::Master, false);
		}
	}
	else
	{
		bool force_all_broken = (0 == (rand() & 3));
		putTerminal(model, floor_zone, terminal::Info, force_all_broken);
		putTerminal(model, floor_zone, terminal::Info, force_all_broken);
		if (rand() & 1)
			putTerminal(model, floor_zone, terminal::Info, force_all_broken);
		if (rand() & 1)
			putTerminal(model, floor_zone, terminal::Info, force_all_broken);
	}



	OverWorld::BlockType block_type( overworld.blockType(block) );

	bool is_reactor(OverWorld::Reactor == block_type);

	int num_machinery = (rand() & 1);
	if ((OverWorld::Engineering == block_type) || (OverWorld::Workshop == block_type))
	{
		num_machinery = adjustNumber( quick, false, 10 + (rand()&3) );
	}
	else if (OverWorld::Reactor == block_type)
	{
		num_machinery = adjustNumber( quick, true, (rand()&7) + 20 );
	}
	else if (OverWorld::Medical == block_type)
	{
		num_machinery = rand() & 3;
	}
	while (num_machinery > 0)
	{
		if (!putHeavyMachinery(model, floor_zone))
			break;
		--num_machinery;
	}


	int level = overworld.level(block, floor);
	ASSERT( level >= 0 );
	if (level > 9)
		level = 9;


	if (level > 0)
	{
		int num_aliens = adjustNumber( quick, is_reactor, 3 * level );
		if (9 == block)
			if (3 == floor)
				num_aliens = (num_aliens/2)+3;

		for (int aliens=0; aliens < num_aliens; ++aliens)
			spawnAlien( model, floor_zone.key(), true );
	}

	if (is_reactor)
	{
		//Lots of workers in the reactor.
		int num_workers = adjustNumber( quick, true, 15 );

		for (int w=0; w < num_workers; ++w)
			spawnAlien( model, floor_zone.key(), aliens::Worker, true );
	}


	if ((9 == block) && (3 == floor))
		spawnEggs(model, world, floor_zone, adjustNumber( quick, true, 50 ));



	if (OverWorld::Storage == block_type)
	{
		spawnObjects(spawnBarrel, model, floor_zone, adjustNumber( quick, false, 20 + (rand() & 15) ) );
		spawnObjects(spawnCrate, model, floor_zone, adjustNumber( quick, false, 40 + (rand() & 63) ) );
	}
	else if ((OverWorld::Engineering == block_type) || (OverWorld::Workshop == block_type))
	{
		spawnObjects(spawnWorkshop, model, floor_zone, adjustNumber( quick, false, 70 ));
	}
	else if (OverWorld::Reactor == block_type)
	{
		spawnObjects(spawnReactor, model, floor_zone, adjustNumber( quick, true, 100 ));
	}
	else
	{
		if ((OverWorld::Security == block_type) || (OverWorld::Administration == block_type))
			spawnObjects(spawnOffice, model, floor_zone, adjustNumber( quick, false, 80 ));
		else if (OverWorld::Medical == block_type)
			spawnObjects(spawnMedical, model, floor_zone, adjustNumber( quick, false, 80 ));
		else
			spawnObjects(spawnResidential, model, floor_zone, adjustNumber( quick, false, 80 ));
	}


	int num_corpses = rand() % (level+1);
	if (OverWorld::Accommodation == block_type)
		num_corpses += (rand() & 7);
	for (int attempts = 100; attempts > 0; --attempts)
	{
		int tx( rand() % floor_zone.sizeX() );
		int tz( rand() % floor_zone.sizeZ() );
		if (terrain::Room == floor_zone.terrainAt(tx,tz))
		{
			spawnCorpse( model, floor_zone, tx, tz, (rand() & 1) );
			--num_corpses;
			if (1 > num_corpses)
				break;
		}
	}
	int num_alien_corpses = (level < 3) || (level > 7) ? rand()&7 : rand()&15;
	spawnAlienCorpses(model, floor_zone, num_alien_corpses);



	int num_pickups = 8 + (rand() % (level+2));
	int num_weapons = (std::max)( 3, (std::min)( 6, (rand() % (level+3)) ) );
	int num_armours = (std::max)( 3, (std::min)( 6, (rand() % (level+3)) ) );

	if (OverWorld::Security == block_type)
	{
		num_weapons += 2;
		num_armours += 2;
	}
	if (OverWorld::Storage == block_type)
	{
		num_pickups += 3;
	}
	if ((7 == block) && (0 == floor))
	{	//"dry" floor
		num_pickups /= 2;
		num_weapons /= 2;
		num_armours /= 2;
	}


	spawnPickups(model, world, floor_zone, adjustNumber( quick, is_reactor, num_pickups ), level, block_type);

	spawnArmours(model, world, floor_zone, adjustNumber( quick, is_reactor, num_armours ), level, block_type);

	spawnWeapons(model, world, floor_zone, adjustNumber( quick, is_reactor, num_weapons ), level, block_type);

}


void spawnMT(AHGameModel& model, int block)
{
	World& world( model.world() );
	OverWorld& overworld( model.overworld() );

	int floor = rand() % ((9 == block) ? OverWorld::MAX_FLOORS : OverWorld::MAX_FLOORS-1);

	shared_ptr< Pickup > mt_obj( new Pickup(pickup::MotionTracker) );
	DBKeyValue mt_key( world.addWorldObject( mt_obj ) );
	DBKeyValue mt_zone_key( overworld.getFloorKey(block, floor) );
	Zone& mt_zone( world.zone(mt_zone_key) );
	for (int attempts = 100; attempts > 0; --attempts)
	{
		int tx( rand() % mt_zone.sizeX() );
		int tz( rand() % mt_zone.sizeZ() );
		if (terrain::Room == mt_zone.terrainAt(tx,tz))
		{
			bool put( mt_obj->moveTo(model, WorldObject::WorldLocation(mt_zone.key(), tx, tz), true) );
			if (put)
			{
				spawnCorpse(model, mt_zone, tx, tz, (0 == (rand() & 3)));
				break;
			}
		}
	}
}

void spawnQueen(AHGameModel& model, World& world, OverWorld& overworld)
{
	shared_ptr< Alien > queen_obj( new Alien(aliens::Queen, false) );
	DBKeyValue queen_key( world.addWorldObject( queen_obj ) );
	Zone& queen_zone( world.zone(overworld.getFloorKey(9,3)) );
	for (;;)
	{
		int tx( (queen_zone.sizeX()/4) + (rand() % (queen_zone.sizeX()/2)) );
		int tz( (queen_zone.sizeZ()/4) + (rand() % (queen_zone.sizeZ()/2)) );

		int dx = (queen_zone.sizeX()/2) - tx;
		int dz = (queen_zone.sizeZ()/2) - tz;
		int rs = (queen_zone.sizeX()/8)*(queen_zone.sizeZ()/8);
		if ((dx*dx)+(dz*dz)>rs)
			continue;

		if (terrain::Room == queen_zone.terrainAt(tx,tz))
		{
			bool put( queen_obj->moveTo(model, WorldObject::WorldLocation(queen_zone.key(), tx, tz), true) );
			if (put)
				break;
		}
	}
}

void initialiseWorld(AHGameModel& model)
{
	World& world( model.world() );
	OverWorld& overworld( model.overworld() );

	world.clear();

	bool quick(AHGameModel::Quick == model.gameType());

	initialiseOverWorld( overworld );

	const int standard_floor_size = quick ? QUICK_NORMAL_SIZE : NORMAL_NORMAL_SIZE;
	const int reactor_floor_size = quick ? QUICK_REACTOR_SIZE : NORMAL_REACTOR_SIZE;

	for (int block=0; block < OverWorld::NUM_BLOCKS; ++block)
	{
		int floor_size = (9 == block) ? reactor_floor_size : standard_floor_size;

		DBKeyValue floor_0 = ( world.createZone(floor_size, floor_size) );
		Zone& ground_floor_zone( world.zone( floor_0 ) );
		DBKeyValue floor_1 = ( world.createZone(floor_size, floor_size) );
		Zone& floor_1_zone( world.zone( floor_1 ) );
		DBKeyValue floor_2 = ( world.createZone(floor_size, floor_size) );
		Zone& floor_2_zone( world.zone( floor_2 ) );

		overworld.setFloorKey( block, 0, floor_0 );
		overworld.setFloorKey( block, 1, floor_1 );
		overworld.setFloorKey( block, 2, floor_2 );

		int top_floor(OverWorld::MAX_FLOORS-2);
		if (9 == block)
		{
			top_floor = OverWorld::MAX_FLOORS-1;
		}

		populateFloor( model, ground_floor_zone, 0, block, 0, top_floor );
		populateFloor( model, floor_1_zone, &ground_floor_zone, block, 1, top_floor );
		populateFloor( model, floor_2_zone, &floor_1_zone, block, 2, top_floor );

		if (9 == block)
		{ //extra floor for reactor

			DBKeyValue floor_3 = ( world.createZone(floor_size, floor_size) );
			Zone& floor_3_zone( world.zone( floor_3 ) );

			overworld.setFloorKey( block, 3, floor_3 );

			populateFloor( model, floor_3_zone, &floor_2_zone, block, 3, top_floor );			
		}
	}

	spawnAlien( model, overworld.getFloorKey( rand()&1 ? 3 : 6, rand() % 3 ), aliens::Hulk, true );
	spawnAlien( model, overworld.getFloorKey( rand()&1 ? 4 : 7, rand() % 3 ), aliens::Hulk, true );
	spawnAlien( model, overworld.getFloorKey( rand()&1 ? 5 : 8, rand() % 3 ), aliens::Hulk, true );

	spawnMT(model, rand() % 9);
	spawnMT(model, 9);

	spawnQueen(model, world, overworld);

	shared_ptr< PlayerCharacter > player_obj( new PlayerCharacter(model) );
	DBKeyValue player_key( world.addWorldObject( player_obj ) );
	model.setAvatar( player_key );

	Zone& start_zone( world.zone(overworld.getFloorKey(1,0)) );
	int tx(0);
	int tz(0);
	for (;;)
	{
		if (terrain::Bridge == start_zone.terrainAt(tx,tz))
		{
			bool put( player_obj->moveTo(model, WorldObject::WorldLocation(start_zone.key(), tx, tz), true) );
			if (put)
			{
				shared_ptr< Weapon > primary_obj( new Weapon( weapon::SMG, weapon::Special_None, getClipSize(weapon::SMG, weapon::Special_None) ) );
				DBKeyValue primary_key( world.addWorldObject( primary_obj ) );
				shared_ptr< Weapon > side_obj( new Weapon( weapon::Pistol, weapon::Special_None, getClipSize(weapon::Pistol, weapon::Special_None) ) );
				DBKeyValue side_key( world.addWorldObject( side_obj ) );
				shared_ptr< Armour > armr_obj( new Armour( armour::FlakJacket, 100 ) );
				DBKeyValue armr_key( world.addWorldObject( armr_obj ) );

				player_obj->useWeapon(model, *side_obj, PlayerCharacter::Sidearm);
				player_obj->useWeapon(model, *primary_obj, PlayerCharacter::Primary);
				player_obj->wearArmour(model, *armr_obj);
				player_obj->addPickup(pickup::Ammo_9mm, 60);
				player_obj->addPickup(pickup::StunGrenade, 2);
				player_obj->addPickup(pickup::FragGrenade, 1);
				player_obj->addPickup(pickup::Medkit, 1);

				player_obj->addXP(500);

				break;
			}
		}
		++tx;

		if (tx >= start_zone.sizeX())
			throw std::runtime_error("Invalid start zone");
	}

	model.checkAliens();
}












OverWorld::OverWorld(void)
{
	reset();
}

void OverWorld::reset(void)
{
	for (int block=0; block < NUM_BLOCKS; ++block)
	{
		m_block_types[block] = Unknown;
		for (int floor=0; floor < MAX_FLOORS; ++floor)
		{
			m_zone_keys[block][floor] = INVALID_KEY;
			m_level[block][floor] = 0;
			m_floor_flags[block][floor] = 0;
			for (int dir=0; dir<4; ++dir)
				m_exits_to_block[block][floor][dir] = -1;
		}
	}
}

DBKeyValue OverWorld::getFloorKey( int block, int floor ) const
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
	{
		return m_zone_keys[block][floor];
	}
	return INVALID_KEY;
}
void OverWorld::setFloorKey( int block, int floor, DBKeyValue key )
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
	{
		m_zone_keys[block][floor] = key;
	}
}

int OverWorld::exitsToBlock( int block, int floor, Direction way ) const
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		if ((way >= 0) && (way < 4))
			return m_exits_to_block[block][floor][way];

	return -1;
}
void OverWorld::setExitsToBlock( int block, int floor, Direction way, int target_block )
{
	if ((-1 == target_block) || ((target_block >= 0) && (target_block < NUM_BLOCKS)))
		if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
			if ((way >= 0) && (way < 4))
				m_exits_to_block[block][floor][way] = target_block;
}

OverWorld::BlockAndFloor OverWorld::getBlockAndFloor( RL_shared::DBKeyValue zone_key ) const
{
	for (int block=0; block < NUM_BLOCKS; ++block)
		for (int floor=0; floor < MAX_FLOORS; ++floor)
		{
			if (m_zone_keys[block][floor] == zone_key)
				return BlockAndFloor(block, floor);
		}

	return BlockAndFloor();
}

OverWorld::BlockType OverWorld::blockType( int block ) const
{
	if ((block >= 0) && (block < NUM_BLOCKS))
		return m_block_types[block];
	return Unknown;
}

namespace
{
	const std::string empty_string;
}

const std::string& OverWorld::blockName( int block ) const
{
	if ((block >= 0) && (block < NUM_BLOCKS))
		return m_block_names[block];
	return empty_string;
}

void OverWorld::setBlockType( int block, BlockType type )
{
	if ((block >= 0) && (block < NUM_BLOCKS))
		m_block_types[block] = type;
}
void OverWorld::setBlockName( int block, const std::string& name )
{
	if ((block >= 0) && (block < NUM_BLOCKS))
		m_block_names[block] = name;
}

int OverWorld::level( int block, int floor ) const
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		return m_level[block][floor];
	return 0;
}
void OverWorld::setLevel( int block, int floor, int level )
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		m_level[block][floor] = level;
}

bool OverWorld::isMapped(int block, int floor) const
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		return (0 != (m_floor_flags[block][floor] & Mapped));
	return true;
}
void OverWorld::setMapped(int block, int floor)
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		m_floor_flags[block][floor] |= Mapped;
}
bool OverWorld::isVisited(int block, int floor) const
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		return (0 != (m_floor_flags[block][floor] & Visited));
	return false;
}
void OverWorld::setVisited(int block, int floor)
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		m_floor_flags[block][floor] |= Visited;
}
bool OverWorld::isDark(int block, int floor) const
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		return (0 != (m_floor_flags[block][floor] & Dark));
	return false;
}
void OverWorld::setDark(int block, int floor)
{
	if ((block >= 0) && (block < NUM_BLOCKS) && (floor >= 0) && (floor < MAX_FLOORS))
		m_floor_flags[block][floor] |= Dark;
}

namespace
{
	const int block_neighbours[OverWorld::NUM_BLOCKS][8] = 
	{
		{ 3, -1,  1, -1, -1,  4, -1, -1 }, 
		{ 4, -1,  2,  0,  3,  5, -1, -1 }, 
		{ 5, -1, -1,  1,  4, -1, -1, -1 },
		{ 6,  0,  4, -1, -1,  7, -1,  1 }, 
		{ 7,  1,  5,  3,  6,  8,  0,  2 },
		{ 8,  2, -1,  4,  7, -1,  1, -1 },
		{-1,  3,  7, -1, -1,  9, -1,  4 }, 
		{ 9,  4,  8,  6, -1, -1,  3,  5 },
		{-1,  5, -1,  7,  9, -1,  4, -1 },
		{-1,  7, -1, -1, -1, -1,  6,  8 }
	};

	const OverWorld::Direction directions[3][3] = 
	{
		{OverWorld::SW, OverWorld::South, OverWorld::SE}, 
		{OverWorld::West, OverWorld::None, OverWorld::East},
		{OverWorld::NW, OverWorld::North, OverWorld::NE}
	};
}

int OverWorld::getNeighbouringBlock(int block, int dx, int dz) const
{
	Direction way( directions[1+dz][1+dx] );
	if (None != way)
	{
		return block_neighbours[block][way];
	}
	return block;
}














void initialiseOverWorld( OverWorld& overworld )
{
	ASSERT( 10 == OverWorld::NUM_BLOCKS );
	ASSERT( 4 == OverWorld::MAX_FLOORS );

	//Block 9 is always the reactor.
	overworld.setBlockType(9, OverWorld::Reactor);
	overworld.setBlockName(9, "Reactor");

	//Block 7 is always engineering.
	overworld.setBlockType(7, OverWorld::Engineering);
	overworld.setBlockName(7, "Engineering Block");

	//Administration is always 6 or 8.
	int admin_block = (rand() & 1) ? 6 : 8;
	overworld.setBlockType(admin_block, OverWorld::Administration);
	overworld.setBlockName(admin_block, "Administration Block");

	const OverWorld::BlockType block_types[OverWorld::NUM_BLOCKS] = 
	{
		OverWorld::Administration, 
		OverWorld::Reactor, 
		OverWorld::Engineering, 
		OverWorld::Accommodation, OverWorld::Accommodation,
		OverWorld::Medical,
		OverWorld::Workshop, 
		OverWorld::Storage, OverWorld::Storage,
		OverWorld::Security
	};
	const char * block_names[OverWorld::NUM_BLOCKS] = 
	{
		"I am Error!", 
		"I am Error!", 
		"I am Error!", 
		"Accommodation Block A", "Accommodation Block B", 
		"Medical Block", 
		"Workshop Block", 
		"Storage Block A", "Storage Block B", 
		"Security Block"
	};
	bool block_used[OverWorld::NUM_BLOCKS] = {true, true, true, false, false, false, false, false, false, false};

	for (int block=0; block < OverWorld::NUM_BLOCKS; ++block)
	{
		if (OverWorld::Unknown == overworld.blockType(block))
		{
			for (;;)
			{
				int attempt = rand() % OverWorld::NUM_BLOCKS;
				if (!block_used[attempt])
				{
					block_used[attempt] = true;
					overworld.setBlockType(block, block_types[attempt]);
					overworld.setBlockName(block, block_names[attempt]);
					break;
				}
			}
		}
	}

	const OverWorld::Direction opposite_way[4] = {OverWorld::South, OverWorld::North, OverWorld::West, OverWorld::East};

	for (int block=0; block < OverWorld::NUM_BLOCKS; ++block)
		for (int floor=0; floor < OverWorld::MAX_FLOORS; ++floor)
			for (int way=0; way < 4; ++way)
			{
				if (OverWorld::MAX_FLOORS-1 != floor)
					overworld.setExitsToBlock( block, floor, (OverWorld::Direction)way, block_neighbours[block][way] );
			}

	//Entrance to reactor on ground level only
	for (int floor=1; floor < OverWorld::MAX_FLOORS-1; ++floor)
	{
		overworld.setExitsToBlock( 9, floor, OverWorld::South, -1 );
		overworld.setExitsToBlock( 7, floor, OverWorld::North, -1 );
	}
	//No other ground level entrance to engineering (usually)
	overworld.setExitsToBlock( 7, 0, OverWorld::South, -1 );
	overworld.setExitsToBlock( 4, 0, OverWorld::North, -1 );
	overworld.setExitsToBlock( 7, 0, OverWorld::East, -1 );
	overworld.setExitsToBlock( 8, 0, OverWorld::West, -1 );
	overworld.setExitsToBlock( 7, 0, OverWorld::West, -1 );
	overworld.setExitsToBlock( 6, 0, OverWorld::East, -1 );


	for (int block=0; block < OverWorld::NUM_BLOCKS-1; ++block)
	{
		for (int floor=0; floor < (OverWorld::MAX_FLOORS-1); ++floor)
		{
			if ((7 == block) && (0 == floor))
				continue; //floor 0 of block 7 is special
			for (int way=0; way < 4; ++way)
			{
				int target_block = block_neighbours[block][way];
				if ((-1 != target_block) && (0 == (rand() % 3)))
				{
					overworld.setExitsToBlock( block, floor, (OverWorld::Direction)way, -1 );
					overworld.setExitsToBlock( target_block, floor, opposite_way[way], -1 );
				}
			}
		}
	}
	for (int block=0; block < OverWorld::NUM_BLOCKS-1; ++block)
	{
		for (int way=0; way < 4; ++way)
		{
			int target_block = block_neighbours[block][way];
			if (-1 == target_block)
				continue;

			int num_exits = 0;
			for (int floor=0; floor < (OverWorld::MAX_FLOORS-1); ++floor)
			{
				if (-1 != overworld.exitsToBlock(block, floor, (OverWorld::Direction)way))
					++num_exits;
			}

			if (0 == num_exits)
			{
				int floor = (rand() % (OverWorld::MAX_FLOORS-1));
				overworld.setExitsToBlock( block, floor, (OverWorld::Direction)way, target_block );
				overworld.setExitsToBlock( target_block, floor, opposite_way[way], block );
			}
		}
	}



	const int floor_levels[OverWorld::NUM_BLOCKS][OverWorld::MAX_FLOORS] = 
	{
		{1, 2, 3, 0},
		{0, 1, 2, 0},
		{1, 2, 3, 0},
		{3, 4, 5, 0},
		{2, 3, 4, 0},
		{3, 4, 5, 0}, 
		{4, 5, 6, 0},
		{7, 6, 5, 0},
		{4, 5, 6, 0},
		{6, 7, 8, 9}
	};

	for (int block=0; block < OverWorld::NUM_BLOCKS; ++block)
		for (int floor=0; floor < OverWorld::MAX_FLOORS; ++floor)
			overworld.setLevel( block, floor, floor_levels[block][floor] );

	for (int block=0; block < OverWorld::NUM_BLOCKS; ++block)
	{
		int chance = (OverWorld::Storage == overworld.blockType(block)) ? 1 : 7;
		for (int floor=0; floor < (OverWorld::MAX_FLOORS-1); ++floor)
		{
			if (overworld.level(block, floor) > 1)
				if (0 == (rand() & chance))
					overworld.setDark(block, floor);
		}
	}
}



}
