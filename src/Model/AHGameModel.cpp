#include "AHGameModel.hpp"
#include "IGameEvents.hpp"
#include "OverWorld.hpp"
#include "Objects/ObjectType.hpp"
#include "WorldObjects/Actor.hpp"
#include "World-2DTiles/Zone.hpp"
#include "Objects/PlayerCharacter.hpp"
#include "Objects/PlayerStats.hpp"
#include "Objects/Alien.hpp"
#include "Objects/TerrainType.hpp"
#include "Objects/Furniture.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <utility>
#include <sstream>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


namespace
{
	const GameTimeCoordinate SPAWN_ALIEN_TIME = 45000;

	int getSpawnAlienTime(bool)
	{
		return (SPAWN_ALIEN_TIME/2)+(rand()%SPAWN_ALIEN_TIME);
	}

	const GameTimeCoordinate COUNTDOWN_START_TIME = 20 * 60 * 1000;
	const GameTimeCoordinate MINIMUM_SAFE_DISTANCE_TIME = 10 * 60 * 1000;
	const GameTimeCoordinate QUICK_COUNTDOWN_TIME = 15 * 60 * 1000;

	const GameTimeCoordinate SWARM_APPEAR_TIME = 90 * 60 * 1000;
}

AHGameModel::AHGameModel(void)
: 
m_reverse_vision(41, 41), 
m_spawn_alien_timer(SPAWN_ALIEN_TIME), m_countdown_time(0), m_game_type(Normal), 
m_queen_is_found(false), m_queen_is_dead(false), m_countdown_expired(false), 
m_used_master_terminal(false), m_spawned_first(false)
{
	clear();
}

void AHGameModel::clear(void)
{
	GameModel::clear();

	m_overworld.reset();
	m_reverse_vision.clear();
	m_aliens_in_zone_lookup.clear();
	m_player_stats.clear();
	m_countdown_time = 0;
	m_game_type = Normal;
	m_queen_is_found = false;
	m_queen_is_dead = false;
	m_countdown_expired = false;
	m_used_master_terminal = false;
	m_spawned_first = false;
	m_spawn_alien_timer = getSpawnAlienTime(false);
}

void AHGameModel::registerPlayerStats( boost::shared_ptr< PlayerStats > stats )
{
	m_player_stats.push_back( stats );
}

bool AHGameModel::avatarIsVisibleFrom( DBKeyValue zone_key, int zone_x, int zone_z ) const
{
	if (!m_world.objectExists(m_avatar_key))
		return false;
	DBKeyValue avatar_zone( m_world.object(m_avatar_key).location().zone );
	if (zone_key != avatar_zone)
		return false;

	int fox(0), foz(0);
	m_reverse_vision.getFieldOrigin(fox, foz);
	return m_reverse_vision.sample((uint16_t)(zone_x - fox), (uint16_t)(zone_z - foz));
}

void AHGameModel::notifyFoundTheQueen(void)
{
	bool first_queen_sighting = !m_queen_is_found;
	m_queen_is_found = true;

	if (first_queen_sighting)
		if (m_world.objectExists(m_avatar_key))
		{
			WorldObject::WorldLocation player_loc( m_world.object(m_avatar_key).location() );
			for (int block=0; block<OverWorld::NUM_BLOCKS; ++block)
			{
				int num = Quick == gameType() ? 
					((9==block) ? 12 : 8) :
					((9==block) ? 20 : 10);

				for (int floor=0; floor<OverWorld::MAX_FLOORS; ++floor)
				{
					DBKeyValue zone_key( m_overworld.getFloorKey(block, floor) );
					if (m_world.zoneExists(zone_key))
					{
						if (zone_key != player_loc.zone)
						{
							for (int i=0; i<num; ++i)
								spawnAlien(zone_key, 0 != (rand() & 3));
						}
					}
				}
			}
		}
}

void AHGameModel::notifyKilledTheQueen(void)
{ 
	if (!m_queen_is_dead)
	{
		m_queen_is_dead = true; 

		if (m_world.objectExists(m_avatar_key))
		{
			WorldObject::WorldLocation player_loc( m_world.object(m_avatar_key).location() );
			//DBKeyValue exit_zone( m_overworld.getFloorKey( Quick == gameType() ? 9 : 1, 0 ) );
			DBKeyValue exit_zone( m_overworld.getFloorKey( 1, 0 ) );
			if ((exit_zone != player_loc.zone) && (rand()&1))
			{ //Maybe give the player a little surprise at the end, in case they get too complacent. Bruce here was out having a smoke when he heard the call to arms
				Zone& zone( m_world.zone( exit_zone ) );
				for (int tx=0; tx < zone.sizeX(); ++tx)
				{
					if (terrain::Bridge == zone.terrainAt(tx, 0))
					{
						shared_ptr< Alien > bruce( new Alien( aliens::Hulk, false ) );
						m_world.addWorldObject( bruce );
						bruce->moveTo(*this, WorldObject::WorldLocation(zone.key(), tx+(rand()&1), 1), true);
						break;
					}
				}
			}
		}
	}
}


void AHGameModel::increaseEndgameDanger(void)
{
	DBKeyValue zone_key( m_overworld.getFloorKey(1, 0) );
	if (m_world.zoneExists(zone_key))
	{
		AlienHack::spawnAlien( *this, zone_key, aliens::DeathSpitter, 0 == (rand()&1) );
		AlienHack::spawnAlien( *this, zone_key, aliens::Reaper, 0 == (rand()&1) );
	}
}


void AHGameModel::notifyUsedMasterTerminal(void)
{
	m_used_master_terminal = true;

	DBKeyValue gate_zone_key( m_overworld.getFloorKey(7,0) );
	Zone& gate_zone( m_world.zone( gate_zone_key ) );
	for (int tx=0; tx < gate_zone.sizeX(); ++tx)
	{
		if (terrain::BlastDoor == terrain::getType( gate_zone.terrainAt(tx, gate_zone.sizeZ()-1) ))
			gate_zone.putTerrain( terrain::Bridge, tx, gate_zone.sizeZ()-1 );
	}
}

RL_shared::GameTimeCoordinate AHGameModel::countdownBecomesVisible(void) const
{
	return (Quick == gameType() ? QUICK_COUNTDOWN_TIME : COUNTDOWN_START_TIME);
}
void AHGameModel::startCountdown(GameTimeCoordinate delay)
{
	if (!isCountdownStarted())
	{
		m_countdown_time = (Quick == gameType() ? QUICK_COUNTDOWN_TIME+(delay/2) : COUNTDOWN_START_TIME+delay);
	}
}
void AHGameModel::abortCountdown(void)
{
	m_countdown_time = 0;
	m_countdown_expired = false;
}

void AHGameModel::advanceGameTime( RL_shared::GameTimeCoordinate time_step )
{
	GameModel::advanceGameTime( time_step );

	if (m_countdown_time > 0)
	{
		m_countdown_time -= time_step;
		if (m_countdown_time <= 0)
		{
			m_countdown_time = 0;
			m_countdown_expired = true;
		}
	}
}

void AHGameModel::updateObjects( RL_shared::GameTimeCoordinate time_step, const World::WorldObjects& objects_to_update )
{
	GameModel::updateObjects(time_step, objects_to_update);
	updateSpawning(time_step);
}

void AHGameModel::updateVision(void)
{
	GameModel::updateVision();

	if (!m_world.objectExists(m_avatar_key))
		return;
	
	PlayerCharacter& avatar_obj = dynamic_cast<PlayerCharacter&>( m_world.object(m_avatar_key) );

	WorldObject::WorldLocation player_loc = avatar_obj.location();
	if (WorldObject::WorldLocation::INVALID_ZONE == player_loc.zone)
		return;

	OverWorld::BlockAndFloor bnf = m_overworld.getBlockAndFloor(player_loc.zone);

	m_overworld.setVisited(bnf.block, bnf.floor);

	int heading = avatar_obj.getHeading();

	class BlockFunc : public RL_shared::IIsVisibilityBlocker
	{
		const RL_shared::World& m_world;
	public:
		explicit BlockFunc( const RL_shared::World& world )
			: m_world(world)
		{
		}
		virtual bool terrain( TerrainType type )
		{
			return !terrain::isPassable(type);
		}
		virtual bool object( DBKeyValue obj )
		{
			if (m_world.objectExists(obj))
			{
				const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( m_world.object(obj) ) );
				return game_obj.blocksVision();
			}
			return false;
		}
	};
	BlockFunc block_func( m_world );

	m_reverse_vision.update( m_world, player_loc.zone, player_loc.x, player_loc.z, 0, 360, 20, block_func );

	int angle = 90;
	int range = 16;
	if (m_overworld.isDark(bnf.block, bnf.floor))
	{
		range = avatar_obj.hasPerk(player_perks::DarkSightRange) ? 12 : 8;
	}
	m_vision_field.update( m_world, player_loc.zone, player_loc.x, player_loc.z, heading, angle, range, block_func );

	Zone& zone = m_world.zone( player_loc.zone );
	uint16_t sx = m_vision_field.sizeX();
	uint16_t sz = m_vision_field.sizeZ();
	int ox(0), oz(0);
	m_vision_field.getFieldOrigin(ox, oz);
	for (uint16_t iz = 0; iz < sz; ++iz)
	{
		for (uint16_t ix = 0; ix < sx; ++ix)
		{
			if (m_vision_field.sample(ix, iz))
			{
				int tx = ox + (int)ix;
				int tz = oz + (int)iz;
				zone.recordTerrain(tx, tz);
				zone.recordObject(1, tx, tz);
			}
		}
	}
}

namespace
{
	int soundPenetrationFunc( RL_shared::TerrainType there )
	{
		return terrain::isPassable(there) ? 1 : (terrain::Door == terrain::getType(there)) ? 4 : 12;
	}
}

void AHGameModel::updateHearing(void)
{
	GameModel::updateHearing();

	if (!m_world.objectExists(m_avatar_key))
		return;
	
	PlayerCharacter& avatar_obj = dynamic_cast<PlayerCharacter&>( m_world.object(m_avatar_key) );

	WorldObject::WorldLocation player_loc = avatar_obj.location();
	if (WorldObject::WorldLocation::INVALID_ZONE == player_loc.zone)
		return;

	m_hearing_field.update( m_world, player_loc.zone, player_loc.x, player_loc.z, avatar_obj.getOpenHearingRange(), soundPenetrationFunc );
}


void AHGameModel::spawnHole(void)
{
    int block( rand() % 10 );
    int floor( rand() % 3 );
    DBKeyValue zone_key( m_overworld.getFloorKey(block, floor) );
	if (!m_world.zoneExists(zone_key))
		return;
	if (!m_world.objectExists(m_avatar_key))
		return;
    if (m_world.object(m_avatar_key).location().zone == zone_key)
        return;
    Zone& zone( m_world.zone(zone_key) );
    int attempts(20);
    do
    {
        int tx( 1 + (rand() % (zone.sizeX()-2)) );
        int tz( 1 + (rand() % (zone.sizeZ()-2)) );
        if (terrain::Room != zone.terrainAt(tx, tz))
            continue;
        const ObjectList& objects(zone.objectsAt(tx,tz));
        if (!objects.empty())
            continue;
        zone.putTerrain(terrain::Hole, tx, tz);
        return;
    }
    while (--attempts > 0);
}


void AHGameModel::updateSpawning( RL_shared::GameTimeCoordinate time_step )
{
	if (!m_world.objectExists(m_avatar_key))
		return;

	DBKeyValue zone_key( m_world.object(m_avatar_key).location().zone );
	if (!m_world.zoneExists(zone_key))
		return;

	OverWorld::BlockAndFloor bnf = m_overworld.getBlockAndFloor(zone_key);
	int level = m_overworld.level(bnf.block, bnf.floor);

	m_spawn_alien_timer -= time_step;

    if (m_queen_is_found && !isCountdownStarted())
        if ((m_spawn_alien_timer & 8191) > ((m_spawn_alien_timer+time_step) & 8191))
			if ((1 == (rand()&15)) || (level < 8))
			{
				GameTimeCoordinate delay = (30000*(1+(rand()&7))) + (5000 * (rand()&15));
				startCountdown(delay);
			}

	bool get_nasty = m_queen_is_dead || (m_queen_is_found && (4 > level));

	while (m_spawn_alien_timer <= 0)
	{
        if (rand()&1)
        {
            spawnHole();
        }

		if (get_nasty && (rand() & 1))
		{
			if (1 == (rand()&15))
				m_spawn_alien_timer += (SPAWN_ALIEN_TIME * 2);
			else
				m_spawn_alien_timer += (800 + (rand() % 401));

			spawnSwarm( *this, zone_key );
			continue;
		}

		if ((!m_queen_is_dead) && (zone_key != m_overworld.getFloorKey(9,3)))
		{
			int v1 = (std::max)(1, (int)(m_total_game_time >> 10));
			int r1 = rand();
			int swarm_time = AHGameModel::Quick != gameType() ? SWARM_APPEAR_TIME : (SWARM_APPEAR_TIME/2);
			if ((r1 % v1) > (SWARM_APPEAR_TIME >> 10))
			{
 				if (0 == (rand() & 7))
				{
					spawnSwarm( *this, zone_key );
					m_spawn_alien_timer += (3*getSpawnAlienTime(false));
					continue;
				}
			}
		}

		m_spawn_alien_timer += getSpawnAlienTime(m_queen_is_dead);

		spawnAlien(zone_key);

		if (!m_spawned_first)
		{	//For the benefit of new players.
			m_spawned_first = true;
			boost::shared_ptr< IGameEvents > events( m_game_events.lock() );
			if (events)
			{
				events->alienMakesSoundAbove(*this);
			}
		}
	}
}
void AHGameModel::spawnAlien( DBKeyValue zone_key, bool fast_forward )
{
	AlienHack::spawnAlien( *this, zone_key, fast_forward );
}


bool AHGameModel::hasPlayerEscaped(void) const
{
	if (!(isCountdownActive() || isQueenDead()))
		return false;
	if (!m_world.objectExists(m_avatar_key))
		return false;
	WorldObject::WorldLocation loc( m_world.object(m_avatar_key).location() );

	OverWorld::BlockAndFloor bnf = m_overworld.getBlockAndFloor( loc.zone );

	int exit_block = 1;//Quick == gameType() ? 9 : 1;
	return (exit_block == bnf.block) && (0 == bnf.floor) && (0 == loc.z);
}

bool AHGameModel::doesPlayerGetTheGoodEnding(void) const
{
	if (!isCountdownActive())
		return true;
	boost::uint32_t test( (rand() & 0xffff) | (rand() << 16) );
	return (test % MINIMUM_SAFE_DISTANCE_TIME) < (boost::uint32_t)m_countdown_time;
}

bool AHGameModel::worldExplodes(void) const
{
	return (isCountdownExpired() && (0 == (rand() & 15)));
}

void AHGameModel::destroyedSomething( const Furniture& object )
{
	if (furniture::Piping == object.furnitureType())
	{
		OverWorld::BlockAndFloor bnf = m_overworld.getBlockAndFloor(object.location().zone);
		if ((OverWorld::Reactor == m_overworld.blockType(bnf.block)) && (!isCountdownStarted()))
		{
			if (7 == (rand() & 4095))
			{
				GameTimeCoordinate delay = 60000 + (15000 * (rand()&15));
				startCountdown(delay);
			}
		}
	}
}

void AHGameModel::mapZone( Zone& zone )
{
	for (int tz=0; tz < zone.sizeZ(); ++tz)
		for (int tx=0; tx < zone.sizeX(); ++tx)
		{
			if (0 == zone.recordedTerrainAt(tx, tz))
			{
				TerrainType there( zone.terrainAt(tx, tz) );
				if ((terrain::Vent == there) || (terrain::Hole == there) || (terrain::Machinery == there))
					there = terrain::Room;
				zone.recordTerrain(tx, tz, there & (~(terrain::Broken | terrain::Open)));
			}
		}
}

void AHGameModel::mapCurrentZone(void)
{
	if (!m_world.objectExists(avatar()))
		return;

	const WorldObject& player_obj( m_world.object( avatar() ) );
	DBKeyValue zone_key( player_obj.location().zone );
	if (m_world.zoneExists(zone_key))
	{
		mapZone( m_world.zone( zone_key ) );

		OverWorld::BlockAndFloor bnf = m_overworld.getBlockAndFloor(zone_key);
		m_overworld.setMapped( bnf.block, bnf.floor );
	}
}

void AHGameModel::checkAliens(void) const
{
	BOOST_FOREACH( const ObjectsInZoneMap::value_type& entry, m_aliens_in_zone_lookup )
	{
		BOOST_FOREACH( RL_shared::DBKeyValue obj_key, entry.second )
		{
			if (m_world.objectExists(obj_key))
			{
				const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( m_world.object( obj_key ) ) );
				if (objects::Alien != game_obj.type())
				{
#ifdef _DEBUG
					__asm int 3;
#endif
					throw std::runtime_error("GAME MODEL ERROR: An object is in an Alien lookup list which is not an Alien");
				}
			}
		}
	}
}

void AHGameModel::registerObjectInZone( RL_shared::DBKeyValue obj_key, RL_shared::DBKeyValue zone_key )
{
	if (!m_world.objectExists(obj_key))
		return;
	if (!m_world.zoneExists(zone_key))
		return;

	const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( m_world.object( obj_key ) ) );

	if (objects::Alien == game_obj.type())
	{
		Objects& list( m_aliens_in_zone_lookup[ zone_key ] );
		if (list.end() == std::find( list.begin(), list.end(), obj_key ))
		{
			list.push_back( obj_key );
		}
	}
}

void AHGameModel::unRegisterObjectInZone( RL_shared::DBKeyValue obj_key, RL_shared::DBKeyValue zone_key )
{
	if (!m_world.objectExists(obj_key))
		return;
	if (!m_world.zoneExists(zone_key))
		return;

	Objects& list( m_aliens_in_zone_lookup[ zone_key ] );
	Objects::iterator entry( std::find( list.begin(), list.end(), obj_key ) );
	if (list.end() != entry)
	{
		*entry = list.back();
		list.pop_back();
	}
}


void AHGameModel::alertAliensInRadius( RL_shared::DBKeyValue zone_key, int x, int z, int range )
{
	if (!m_world.zoneExists(zone_key))
		return;

	const Zone& zone( m_world.zone(zone_key) );

	const int radius_sq = range*range;

	//For shorter ranges, search the tile contents instead. 
	//TODO decide whether this is worth doing or not.
	if (range <= 3)
	{
		for (int iz = -range; iz <= range; ++iz)
			for (int ix = -range; ix <= range; ++ix)
				if (((ix*ix)+(iz*iz)) <= radius_sq)
					if (zone.isWithin(x+ix, z+iz))
					{
						const ObjectList& objects( zone.objectsAt(x+ix, z+iz) );
						BOOST_FOREACH( RL_shared::DBKeyValue obj_key, objects )
						{
							if (m_world.objectExists(obj_key))
							{
								AHGameObject& obj( dynamic_cast< AHGameObject& >( m_world.object(obj_key) ) );
								if (objects::Alien == obj.type())
								{
									Alien& alien( dynamic_cast<Alien&>(obj) );
									alien.soundAlert(m_world, zone, x, z, range);
								}
							}
						}
					}

		return;
	}

	//Local copy deliberately made to guard against iterator invalidation
	Objects aliens( m_aliens_in_zone_lookup[ zone_key ] );

	BOOST_FOREACH( RL_shared::DBKeyValue obj_key, aliens )
	{
		if (m_world.objectExists(obj_key))
		{
			try
			{
				Alien& alien( dynamic_cast<Alien&>( m_world.object(obj_key) ) );

				const WorldObject::WorldLocation a_loc( alien.location() );

				int offs_x = a_loc.x - x;
				int offs_z = a_loc.z - z;
				int dist_sq = (offs_x*offs_x)+(offs_z*offs_z);
				if (dist_sq <= radius_sq)
				{
					alien.soundAlert(m_world, zone, x, z, range);
				}
			}
			catch(...)
			{
				ASSERT(false);
				unRegisterObjectInZone(obj_key, zone_key);
			}
		}
		else
		{
			unRegisterObjectInZone(obj_key, zone_key);
		}
	}
}



std::string AHGameModel::mortem(void) const
{
	const int LINE_LENGTH = 80;

	std::ostringstream mortem;
	mortem << "/------------------------------------------------------------------------------\\" << std::endl;
	mortem << "|  Mission Report                                                              |" << std::endl;
	mortem << "\\------------------------------------------------------------------------------/" << std::endl;
	mortem << std::endl << "Classification: Highest" << std::endl;
	mortem << std::endl << "Mission type: " << (gameType() == Quick ? "Quick" : "Normal") << std::endl;

	BOOST_FOREACH( boost::shared_ptr< PlayerStats > player, m_player_stats )
	{
		if (!player)
			continue;

		mortem << std::endl;
		mortem << "================================================================================" << std::endl;
		mortem << std::endl;
		mortem << "Name:   " << player->name() << std::endl;
		mortem << "Status: " << player->outcome() << std::endl;
		mortem << std::endl;
		mortem << "================================================================================" << std::endl;
		mortem << std::endl;

		GameTimeCoordinate hours( gameTime() / (60*60*1000) );
		GameTimeCoordinate mins( gameTime() / (60*1000) );
		GameTimeCoordinate secs( gameTime() / 1000 );
		if (mins > 0)
			mins %= 60;
		if (secs > 0)
			secs %= 60;
		boost::format time_str("%d:%02d:%02d");
		mortem << "Time survived: " << (time_str % hours % mins % secs).str() << std::endl;
		if (isCountdownActive())
		{
			GameTimeCoordinate hours( getCountdownTime() / (60*60*1000) );
			GameTimeCoordinate mins( getCountdownTime() / (60*1000) );
			GameTimeCoordinate secs( getCountdownTime() / 1000 );
			if (mins > 0)
				mins %= 60;
			if (secs > 0)
				secs %= 60;
			boost::format time_str("%d:%02d:%02d");
			mortem << "Reactor critical in: " << (time_str % hours % mins % secs).str() << std::endl;
		}
		mortem << std::endl;

		mortem << "Total XP:     " << (player->XP() + player->spentXP()) << std::endl;
		mortem << "XP Remaining: " << player->XP() << std::endl;
		mortem << "XP Spent:     " << player->spentXP() << std::endl;
		mortem << std::endl;
		mortem << "HP:     " << player->HP() << std::endl;
		mortem << "Max HP: " << player->maxHP() << std::endl;
		mortem << std::endl;

		mortem << "Perks: ";
		if (0 == player->perks())
		{
			mortem << "None" << std::endl;
		}
		else
		{
			using namespace player_perks;

			boost::uint32_t perks = player->perks();
			int len = 7;
			bool first(true);

			for (int i=0; i<NumPerks; ++i)
			{
				Perk p( (Perk)i );

				if (perks & perkBitMask(p))
				{
					if (!first)
					{
						if (LINE_LENGTH < len + 2)
						{
							mortem << std::endl << "   ";
							len = 3;
						}
						mortem << ", ";
						len += 2;
					}
					else
						first = false;

					std::string perkname( perkName(p) );

					if (LINE_LENGTH < len + perkname.length())
					{
						mortem << std::endl << "   ";
						len = 3;
					}

					mortem << perkname;
					len += perkname.length();
				}
			}
		}
		mortem << std::endl;

		mortem << "Armour:  " << player->armour() << std::endl;
		mortem << "Sidearm: " << player->sidearm() << std::endl;
		mortem << "Primary: " << player->primary() << std::endl;

		mortem << std::endl;
		mortem << std::endl;
		{
			using namespace pickup;

			mortem << "---------- Inventory -----------------------------------------------------------" << std::endl;
			mortem << "9mm:  " << player->pickups(Ammo_9mm) << std::endl;
			mortem << "12g:  " << player->pickups(Ammo_shotgun) << std::endl; 
			mortem << ".44:  " << player->pickups(Ammo_colt) << std::endl; 
			mortem << "M309: " << player->pickups(Ammo_pulse) << std::endl;
			mortem << "Fuel: " << player->pickups(Ammo_flame) << std::endl;
			mortem << "M250: " << player->pickups(Ammo_smartgun) << std::endl;
			mortem << "PP:   " << player->pickups(Ammo_laser) << std::endl;
			mortem << "30mm HEDP: " << player->pickups(Ammo_HEDP30mm) << std::endl;
			mortem << "Stun grenades: " << player->pickups(StunGrenade) << std::endl;
			mortem << "Frag grenades: " << player->pickups(FragGrenade) << std::endl;
			mortem << "Inc grenades:  " << player->pickups(IncGrenade) << std::endl;
			mortem << "Krak grenades: " << player->pickups(KrakGrenade) << std::endl;
			mortem << "Medkits: " << player->pickups(Medkit) << std::endl;
			mortem << "Neutraliser: " << player->pickups(Neutraliser) << std::endl;
			mortem << "Demo Charge: " << player->pickups(DemoCharge) << std::endl;
			mortem << "Cred Chip: " << player->pickups(CreditChip) << std::endl;
			mortem << "Mega Cred Chip: " << player->pickups(MegaCredChip) << std::endl;
			mortem << "Motion Tracker: " << player->pickups(MotionTracker) << std::endl;

			mortem << std::endl;
			mortem << "---------- Rounds Fired --------------------------------------------------------" << std::endl;
			mortem << "9mm:  " << player->pickupsUsed(Ammo_9mm) << std::endl;
			mortem << "12g:  " << player->pickupsUsed(Ammo_shotgun) << std::endl; 
			mortem << ".44:  " << player->pickupsUsed(Ammo_colt) << std::endl; 
			mortem << "M309: " << player->pickupsUsed(Ammo_pulse) << std::endl;
			mortem << "Fuel: " << player->pickupsUsed(Ammo_flame) << std::endl;
			mortem << "M250: " << player->pickupsUsed(Ammo_smartgun) << std::endl;
			mortem << "PP:   " << player->pickupsUsed(Ammo_laser) << std::endl;
			mortem << "30mm HEDP: " << player->pickupsUsed(Ammo_HEDP30mm) << std::endl;

			mortem << std::endl;
			mortem << "---------- Grenades Thrown -----------------------------------------------------" << std::endl;
			mortem << "Stun grenades: " << player->pickupsUsed(StunGrenade) << std::endl;
			mortem << "Frag grenades: " << player->pickupsUsed(FragGrenade) << std::endl;
			mortem << "Inc grenades:  " << player->pickupsUsed(IncGrenade) << std::endl;
			mortem << "Krak grenades: " << player->pickupsUsed(KrakGrenade) << std::endl;

			mortem << std::endl;
			mortem << "---------- Equipment Used ------------------------------------------------------" << std::endl;
			mortem << "Medkits: " << player->pickupsUsed(Medkit) << std::endl;
			mortem << "Neutraliser: " << player->pickupsUsed(Neutraliser) << std::endl;
			mortem << "Demo Charge: " << player->pickupsUsed(DemoCharge) << std::endl;
		}

		mortem << std::endl;
		mortem << std::endl;
		{
			using namespace aliens;

			mortem << "********** Kills ***************************************************************" << std::endl;

			if (player->kills(Worker) > 0)
				mortem << "Worker:        " << player->kills(Worker) << std::endl;
			if (player->kills(Juvenile) > 0)
				mortem << "Juvenile:      " << player->kills(Juvenile) << std::endl;
			if (player->kills(Harrier) > 0)
				mortem << "Harrier:       " << player->kills(Harrier) << std::endl;
			if (player->kills(Warrior) > 0)
				mortem << "Warrior:       " << player->kills(Warrior) << std::endl;
			if (player->kills(Hunter) > 0)
				mortem << "Hunter:        " << player->kills(Hunter) << std::endl;
			if (player->kills(Gladiator) > 0)
				mortem << "Gladiator:     " << player->kills(Gladiator) << std::endl;
			if (player->kills(Elite) > 0)
				mortem << "Elite:         " << player->kills(Elite) << std::endl;
			if (player->kills(Spitter) > 0)
				mortem << "Spitter:       " << player->kills(Spitter) << std::endl;
			if (player->kills(DeathSpitter) > 0)
				mortem << "Death Spitter: " << player->kills(DeathSpitter) << std::endl;
			if (player->kills(Slayer) > 0)
				mortem << "Slayer:        " << player->kills(Slayer) << std::endl;
			if (player->kills(Reaper) > 0)
				mortem << "Reaper:        " << player->kills(Reaper) << std::endl;
			if (player->kills(Hulk) > 0)
				mortem << "Hulk:          " << player->kills(Hulk) << std::endl;
			if (player->kills(Facehugger) > 0)
				mortem << "Facehugger:    " << player->kills(Facehugger) << std::endl;
			if (player->kills(Fasthugger) > 0)
				mortem << "Fasthugger:    " << player->kills(Fasthugger) << std::endl;
			if (player->kills(Queen) > 0)
				mortem << "Queen:         " << player->kills(Queen) << std::endl;

			mortem << "********************************************************************************" << std::endl;
		}

		mortem << std::endl;
		mortem << "Acid taken: " << player->acid() << std::endl;
		if (player->huggerAttacksSurvived() > 0)
		{
			mortem << "Hugger attacks survived: " << player->huggerAttacksSurvived() << std::endl;
		}

		mortem << "================================================================================\n";
	}

	return mortem.str();
}



}
