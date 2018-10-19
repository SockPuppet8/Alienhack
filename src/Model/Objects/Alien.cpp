#include "Alien.hpp"
#include "Decoration.hpp"
#include "Ping.hpp"
#include "ObjectType.hpp"
#include "TerrainType.hpp"
#include "PlayerStats.hpp"
#include "../Actions/DiscreteMoveAction.hpp"
#include "../Actions/EnterExitVentAction.hpp"
#include "../Actions/DirectAttackAction.hpp"
#include "../Actions/HuggerAttackAction.hpp"
#include "../Actions/SpitterAttackAction.hpp"
#include "../Actions/BreakDoorAction.hpp"
#include "../LoSSampler.hpp"
#include "../IGameEvents.hpp"
#include "../Overworld.hpp"
#include "World-2DTiles/VisionField.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <list>



namespace AlienHack
{



using namespace RL_shared;
using namespace boost;

const int AI_INTERVAL = 500;
const int HUNT_PLAYER_TIME = 4000;

const int VISION_RANGE = 18;




namespace alien_data
{

struct AlienData
{
	aliens::Type type;
	GameTimeCoordinate move_speed;
	GameTimeCoordinate move_diagonal_speed;
	GameTimeCoordinate substructure_move_speed;
	GameTimeCoordinate substructure_move_diagonal_speed;
	GameTimeCoordinate traverse_vent_time;
	int HP;
	int armour;
	int grapple_str;
	int XP;
	int attack_dmg;
	int attack_var;
	GameTimeCoordinate attack_time;
	GameTimeCoordinate attack_commit;
	int evasion;

	AlienData(void)
		: type(aliens::None)
		, move_speed(0)
		, move_diagonal_speed(0)
		, substructure_move_speed(0)
		, substructure_move_diagonal_speed(0)
		, traverse_vent_time(0)
		, HP(0)
		, armour(0)
		, grapple_str(0) 
		, XP(0)
		, attack_dmg(0)
		, attack_var(0)
		, attack_time(0)
		, attack_commit(0)
		, evasion(0)
	{
	}

	AlienData(
		aliens::Type in_type, 
		int in_move_speed, 
		int in_move_diagonal_speed, 
		int in_substructure_move_speed, 
		int in_substructure_move_diagonal_speed, 
		int in_traverse_vent_time, 
		int in_HP, 
		int in_armour,
		int in_grapple_str, 
		int in_XP, 
		int in_attack_dmg, 
		int in_attack_var, 
		GameTimeCoordinate in_attack_time, 
		GameTimeCoordinate in_attack_commit,
		int in_evasion
	)
		: type( in_type )
		, move_speed( in_move_speed )
		, move_diagonal_speed( in_move_diagonal_speed )
		, substructure_move_speed( in_substructure_move_speed )
		, substructure_move_diagonal_speed( in_substructure_move_diagonal_speed )
		, traverse_vent_time( in_traverse_vent_time )
		, HP( in_HP )
		, armour( in_armour )
		, grapple_str(in_grapple_str) 
		, XP( in_XP )
		, attack_dmg(in_attack_dmg)
		, attack_var(in_attack_var)
		, attack_time(in_attack_time)
		, attack_commit(in_attack_commit)
		, evasion(in_evasion)
	{
	}	
};


const AlienData no_alien_data;

const AlienData juvenile_data
(
	aliens::Juvenile,	//aliens::Type in_type, 
	300,	//int in_move_speed, 
	424,	//int in_move_diagonal_speed, 
	900,	//int in_substructure_move_speed, 
	1272,	//int in_substructure_move_diagonal_speed, 
	800,	//int in_traverse_vent_time, 
	14,	//int in_HP, 
	0,	//int in_armour
	0,	//int in_grapple_str, 
	5,	//XP
	4,	//int in_attack_dmg, 
	2,	//int in_attack_var, 
	300,	//GameTimeCoordinate in_attack_time, 
	150	//GameTimeCoordinate in_attack_commit
	,1
);

const AlienData harrier_data
(
	aliens::Harrier,	//aliens::Type in_type, 
	150,	//int in_move_speed, 
	212,	//int in_move_diagonal_speed, 
	450,	//int in_substructure_move_speed, 
	636,	//int in_substructure_move_diagonal_speed, 
	533,	//int in_traverse_vent_time, 
	10,	//int in_HP, 
	0,	//int in_armour
	0,	//int in_grapple_str, 
	10,	//XP
	3,	//int in_attack_dmg, 
	2,	//int in_attack_var, 
	150,	//GameTimeCoordinate in_attack_time, 
	75	//GameTimeCoordinate in_attack_commit
	,3
);

const AlienData worker_data
(
	aliens::Worker,	//aliens::Type in_type, 
	500,	//int in_move_speed, 
	707,	//int in_move_diagonal_speed, 
	1500,	//int in_substructure_move_speed, 
	2121,	//int in_substructure_move_diagonal_speed, 
	1000,	//int in_traverse_vent_time, 
	25,	//int in_HP, 
	2,	//int in_armour
	60,	//int in_grapple_str, 
	1,	//XP
	4,	//int in_attack_dmg, 
	2,	//int in_attack_var, 
	600,	//GameTimeCoordinate in_attack_time, 
	300	//GameTimeCoordinate in_attack_commit
	,0
);

const AlienData warrior_data
(
	aliens::Warrior,	//aliens::Type in_type, 
	300,	//int in_move_speed, 
	424,	//int in_move_diagonal_speed, 
	900,	//int in_substructure_move_speed, 
	1272,	//int in_substructure_move_diagonal_speed, 
	800,	//int in_traverse_vent_time, 
	25,	//int in_HP, 
	1,	//int in_armour
	60,	//int in_grapple_str, 
	10,	//XP
	5,	//int in_attack_dmg, 
	3,	//int in_attack_var, 
	300,	//GameTimeCoordinate in_attack_time, 
	150	//GameTimeCoordinate in_attack_commit
	,0
);

const AlienData hunter_data
(
	aliens::Hunter,	//aliens::Type in_type, 
	200,	//int in_move_speed, 
	283,	//int in_move_diagonal_speed, 
	600,	//int in_substructure_move_speed, 
	849,	//int in_substructure_move_diagonal_speed, 
	533,	//int in_traverse_vent_time, 
	15,	//int in_HP, 
	0,	//int in_armour
	30,	//int in_grapple_str, 
	10,	//XP
	4,	//int in_attack_dmg, 
	2,	//int in_attack_var, 
	200,	//GameTimeCoordinate in_attack_time, 
	100	//GameTimeCoordinate in_attack_commit
	,2
);

const AlienData gladiator_data
(
	aliens::Gladiator,	//aliens::Type in_type, 
	300,	//int in_move_speed, 
	424,	//int in_move_diagonal_speed, 
	900,	//int in_substructure_move_speed, 
	1272,	//int in_substructure_move_diagonal_speed, 
	800,	//int in_traverse_vent_time, 
	35,	//int in_HP, 
	1,	//int in_armour
	60,	//int in_grapple_str, 
	15,	//XP
	5,	//int in_attack_dmg, 
	3,	//int in_attack_var, 
	300,	//GameTimeCoordinate in_attack_time, 
	150	//GameTimeCoordinate in_attack_commit
	,0
);

const AlienData elite_data
(
	aliens::Elite,	//aliens::Type in_type, 
	300,	//int in_move_speed, 
	424,	//int in_move_diagonal_speed, 
	900,	//int in_substructure_move_speed, 
	1272,	//int in_substructure_move_diagonal_speed, 
	800,	//int in_traverse_vent_time, 
	30,	//int in_HP, 
	3,	//int in_armour
	75,	//int in_grapple_str, 
	15, //XP
	6,	//int in_attack_dmg, 
	3,	//int in_attack_var, 
	300,	//GameTimeCoordinate in_attack_time, 
	150	//GameTimeCoordinate in_attack_commit
	,0
);

const AlienData slayer_data
(
	aliens::Slayer,	//aliens::Type in_type, 
	150,	//int in_move_speed, 
	212,	//int in_move_diagonal_speed, 
	600,	//int in_substructure_move_speed, 
	849,	//int in_substructure_move_diagonal_speed, 
	533,	//int in_traverse_vent_time, 
	18,	//int in_HP, 
	0,	//int in_armour
	30,	//int in_grapple_str, 
	15, //XP
	5,	//int in_attack_dmg, 
	3,	//int in_attack_var, 
	150,	//GameTimeCoordinate in_attack_time, 
	75	//GameTimeCoordinate in_attack_commit
	,2
);

const AlienData reaper_data
(
	aliens::Reaper,	//aliens::Type in_type, 
	150,	//int in_move_speed, 
	212,	//int in_move_diagonal_speed, 
	600,	//int in_substructure_move_speed, 
	849,	//int in_substructure_move_diagonal_speed, 
	533,	//int in_traverse_vent_time, 
	25,	//int in_HP, 
	1,	//int in_armour
	60,	//int in_grapple_str, 
	20, //XP
	5,	//int in_attack_dmg, 
	3,	//int in_attack_var, 
	150,	//GameTimeCoordinate in_attack_time, 
	75	//GameTimeCoordinate in_attack_commit
	,3
);

const AlienData hulk_data
(
	aliens::Hulk,	//aliens::Type in_type, 
	600,	//int in_move_speed, 
	848,	//int in_move_diagonal_speed, 
	1800,	//int in_substructure_move_speed, 
	2500,	//int in_substructure_move_diagonal_speed, 
	1600,	//int in_traverse_vent_time, 
	60,	//int in_HP, 
	6,	//int in_armour
	50,	//int in_grapple_str, 
	250, //XP
	8,	//int in_attack_dmg, 
	5,	//int in_attack_var, 
	600,	//GameTimeCoordinate in_attack_time, 
	300	//GameTimeCoordinate in_attack_commit
	,0
);

const AlienData queen_data
(
	aliens::Queen,	//aliens::Type in_type, 
	500,	//int in_move_speed, 
	707,	//int in_move_diagonal_speed, 
	1800,	//int in_substructure_move_speed, 
	2500,	//int in_substructure_move_diagonal_speed, 
	1200,	//int in_traverse_vent_time, 
	99,	//int in_HP, 
	6,	//int in_armour
	75,	//int in_grapple_str, 
	1000, //XP
	9,	//int in_attack_dmg, 
	6,	//int in_attack_var, 
	600,	//GameTimeCoordinate in_attack_time, 
	300	//GameTimeCoordinate in_attack_commit
	,0
);

const AlienData facehugger_data
(
	aliens::Facehugger,	//aliens::Type in_type, 
	200,	//int in_move_speed, 
	283,	//int in_move_diagonal_speed, 
	400,	//int in_substructure_move_speed, 
	565,	//int in_substructure_move_diagonal_speed, 
	600,	//int in_traverse_vent_time, 
	4,	//int in_HP, 
	0,	//int in_armour
	0,	//int in_grapple_str, 
	10,	//XP
	0,	//int in_attack_dmg, 
	0,	//int in_attack_var, 
	300,	//GameTimeCoordinate in_attack_time, 
	150	//GameTimeCoordinate in_attack_commit
	,9
);

const AlienData Fasthugger_data
(
	aliens::Fasthugger,	//aliens::Type in_type, 
	150,	//int in_move_speed, 
	212,	//int in_move_diagonal_speed, 
	300,	//int in_substructure_move_speed, 
	424,	//int in_substructure_move_diagonal_speed, 
	300,	//int in_traverse_vent_time, 
	4,	//int in_HP, 
	0,	//int in_armour
	0,	//int in_grapple_str, 
	15,	//XP
	0,	//int in_attack_dmg, 
	0,	//int in_attack_var, 
	200,	//GameTimeCoordinate in_attack_time, 
	100	//GameTimeCoordinate in_attack_commit
	,10
);

const AlienData spitter_data
(
	aliens::Spitter,	//aliens::Type in_type, 
	200,	//int in_move_speed, 
	283,	//int in_move_diagonal_speed, 
	600,	//int in_substructure_move_speed, 
	849,	//int in_substructure_move_diagonal_speed, 
	533,	//int in_traverse_vent_time, 
	14,	//int in_HP, 
	0,	//int in_armour
	30,	//int in_grapple_str, 
	10,	//XP
	5,	//int in_attack_dmg, 
	2,	//int in_attack_var, 
	200,	//GameTimeCoordinate in_attack_time, 
	100	//GameTimeCoordinate in_attack_commit
	,2
);

const AlienData deathspitter_data
(
	aliens::DeathSpitter,	//aliens::Type in_type, 
	200,	//int in_move_speed, 
	283,	//int in_move_diagonal_speed, 
	600,	//int in_substructure_move_speed, 
	849,	//int in_substructure_move_diagonal_speed, 
	800,	//int in_traverse_vent_time, 
	20,	//int in_HP, 
	1,	//int in_armour
	50,	//int in_grapple_str, 
	15,	//XP
	6,	//int in_attack_dmg, 
	3,	//int in_attack_var, 
	200,	//GameTimeCoordinate in_attack_time, 
	100	//GameTimeCoordinate in_attack_commit
	,1
);


const struct AlienData& getAlienData( aliens::Type type )
{
	using namespace aliens;
	switch(type)
	{
		case Juvenile:	return juvenile_data;
		case Harrier:	return harrier_data;
		case Worker:	return worker_data;
		case Warrior:	return warrior_data;
		case Hunter:	return hunter_data;
		case Elite:		return elite_data;
		case Slayer:	return slayer_data;
		case Hulk:		return hulk_data;
		case Queen:		return queen_data;
		case Facehugger:	return facehugger_data;
		case Fasthugger:	return Fasthugger_data;
		case Spitter:		return spitter_data;
		case DeathSpitter:	return deathspitter_data;
		case Gladiator:		return gladiator_data;
		case Reaper:		return reaper_data;
	}
	ASSERT(false);
	return no_alien_data;
}

}

namespace aliens
{
	int getEvasion(Type type)
	{
		using namespace alien_data;
		const struct AlienData& alien_data( getAlienData( type ) );
		//int evasion = 
		//	  (alien_data.move_speed <= 200) 
		//	+ (alien_data.move_speed <= 150) 
		//	+ (alien_data.HP <= 15) 
		//	+ (7*(alien_data.HP <= 5));
		//return evasion;
		return alien_data.evasion;
	}

	int getAttackRange(Type type)
	{
		switch(type)
		{
			case Spitter:
				return 4;
			case DeathSpitter:
				return 6;
			default:;
		}
		return 1;
	}
	int getRangedAttackAmount(Type type)
	{
		switch(type)
		{
			case Spitter:
				return 3;
			case DeathSpitter:
				return 4;
			default:;
		}
		return 0;
	}
}




namespace
{
	bool isFireThere( const World& world, const Zone& zone, int x, int z )
	{
		if (zone.isWithin(x,z))
		{
			const ObjectList& objects( zone.objectsAt(x,z) );
			BOOST_FOREACH( DBKeyValue obj_key, objects )
			{
				if (world.objectExists(obj_key))
				{
					const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( world.object( obj_key ) ) );
					if (objects::Fire == game_obj.type())
						return true;
				}
			}
		}
		return false;
	}
}




Alien::Alien( aliens::Type type, bool in_substructure )
: AHGameObject( objects::Alien )
, m_ai_timer(0)
, m_stun_timer(0)
, m_last_seen_player(0)
, m_behaviour( in_substructure ? alien_behaviour::moving_in_substructure : alien_behaviour::none )
, m_random_searches(0)
, m_blocked_on_path(0)
, m_HP(0)
, m_alien_type(type)
, m_sees_player(false)
, m_activated(false)
, m_player_has_seen_me(false)
{
	m_HP = alien_data::getAlienData(m_alien_type).HP;
}

//Constructor for serialization
Alien::Alien(void)
: AHGameObject( objects::Alien )
, m_ai_timer(0)
, m_stun_timer(0)
, m_last_seen_player(0)
, m_behaviour( alien_behaviour::none )
, m_random_searches(0)
, m_blocked_on_path(0)
, m_HP(0)
, m_alien_type(aliens::None)
, m_sees_player(false)
, m_activated(false)
, m_player_has_seen_me(false)
{
}

boost::shared_ptr< Alien > Alien::getSharedFromThis( RL_shared::World& world )
{
	if (world.objectExists( key() ))
	{
		return dynamic_pointer_cast< Alien >( world.objectPtr( key() ) );
	}
	return boost::shared_ptr< Alien >();
}

bool Alien::canOverlap(const RL_shared::WorldObject& obj) const
{
	if (obj.key() == key())
		return true;

	const AHGameObject& rhs_obj( dynamic_cast< const AHGameObject& >(obj) );

	WorldObjectType	rhs_type( rhs_obj.type() );

	if (objects::Alien == rhs_type)
		return isInSubstructure() != (dynamic_cast<const Alien&>(obj).isInSubstructure());

	if (isInSubstructure())
		return true;

	if (objects::Player == rhs_type)
		return false;

	if (objects::Barrel == rhs_type)
		return false;

	if ((objects::Egg == rhs_type) && (!aliens::isHugger(alienType())))
		return false;

	return true;
}
bool Alien::canOverlap(TerrainType type) const
{
	if (isInSubstructure())
		return true;
	//if (terrain::Wall == type)
	//	return false;
	//if (terrain::Door == (type & terrain::Types))
	//	return (0 != (terrain::Open & type));
	if (!terrain::isPassable(type))
		return false;
	if (terrain::Bridge == terrain::getType(type))
		return false;
	if ((terrain::StairsUp == terrain::getType(type)) || (terrain::StairsDown == terrain::getType(type)))
		if (aliens::Queen != alienType())
			return false; //don't block stairs or bridge tiles, but wait to ambush next to them. Queen is allowed to sit on stairs. 
	return true;
}
bool Alien::canOverlapOrBreak(const RL_shared::World& world, const RL_shared::Zone& zone, int x, int z) const
{
	if (!AHGameObject::canOverlap(world, zone, x, z))
	{
		if (aliens::isLargeAlien(alienType()))
		{
			if (zone.isWithin(x, z))
			{
				if (terrain::isBreakable(zone.terrainAt(x, z)))
					return true;
			}
		}
		return false;
	}
	return true;
}

bool Alien::removeMe(AHGameModel&) const
{
	return (m_HP <= 0);
}
bool Alien::shouldDraw(void) const
{
	return !isInSubstructure();
}
bool Alien::alwaysProcess(void) const
{
	return (aliens::Queen == alienType()) && m_activated;
}

void Alien::notifyTimeStep( AHGameModel& model, RL_shared::GameTimeCoordinate dt )
{
	if ((alienType() == aliens::Queen) && (!model.isQueenFound()) && (!isInSubstructure()))
	{
		WorldObject::WorldLocation my_loc( location() );
		if (model.isVisible(my_loc.zone, my_loc.x, my_loc.z))
		{
			model.notifyFoundTheQueen();
			m_ai_timer = 0;
		}
	}

	if (m_last_seen_player > 0)
	{
		m_last_seen_player -= dt;
		if (m_last_seen_player <= 0)
			m_last_seen_player = 0;
	}

	if (m_stun_timer > 0)
	{
		int old_stun_timer = m_stun_timer;
		m_stun_timer -= dt;
		if (m_stun_timer > 0)
			return;
		dt -= old_stun_timer;
		m_stun_timer = 0;
		if (dt <= 0)
			return;
	}

	if (!(actions().empty()))
	{
		m_ai_timer = 0;
		return;
	}

	m_ai_timer -= dt;

	if (m_ai_timer > 0)
		return;

	m_ai_timer += AI_INTERVAL; //Use of this timer avoids having to use wait actions for aliens

	doAI(model);

	//Sometimes generate a movement trace even when doing nothing.
	if (actions().empty())
		if (0 == (rand() & 127))
		{
			WorldObject::WorldLocation my_loc( location() );
			World& world( model.world() );
			Zone& zone( world.zone( my_loc.zone ) );
			shared_ptr< Ping > ping( new Ping( ping::Motion, 400 ) );
			DBKeyValue ping_key( world.addWorldObject( ping ) );
			ping->moveTo(model, my_loc, true);
		}
}

void Alien::doAI( AHGameModel& model )
{
	if (m_stun_timer > 0)
		return;

	const alien_data::AlienData& type_stats( alien_data::getAlienData( alienType() ) );

	World& world( model.world() );
	const WorldObject::WorldLocation my_loc( location() );
	const Zone& zone( world.zone( my_loc.zone ) );

	shared_ptr< IGameEvents > events( model.gameEventsObserver() );

	if ((aliens::Queen == alienType()) && (model.isQueenFound()))
		if (queenJump(model))
			return;

	//Workers will wander about randomly. So will any other alien with nothing to do when "swarming mode" is on.
	if ((aliens::Worker == alienType()) || (model.isQueenDead()))
		if ((alien_behaviour::none == m_behaviour) && (0 == (rand() & 63)))
		{
			m_random_searches = 1;
			pickNewTarget(world, zone);
		}

	bool fire_here( isFireThere( world, zone, my_loc.x, my_loc.z ) );
	bool is_overhead(false);
	bool noise_pings(false);
	bool has_los(false);
	bool player_can_see_me( model.isVisible(my_loc.zone, my_loc.x, my_loc.z) );
	int distance_to_player_squared(0);

	if (world.objectExists(model.avatar()))
	{
		PlayerCharacter& player_obj( dynamic_cast< PlayerCharacter& >(world.object( model.avatar() )) );
		const WorldObject::WorldLocation player_loc( player_obj.location() );

		if ((my_loc.zone == player_loc.zone) && (WorldObject::WorldLocation::INVALID_ZONE != my_loc.zone))
		{
			int offs_x = player_loc.x - my_loc.x;
			int offs_z = player_loc.z - my_loc.z;
			distance_to_player_squared = (offs_x*offs_x) + (offs_z*offs_z);

			bool could_see_player_before = m_sees_player;
			m_sees_player = false;
			if (!isInSubstructure())
			{
				if (distance_to_player_squared <= 2)
				{
					m_sees_player = true;
					shared_ptr< PlayerCharacter > player_ptr( dynamic_pointer_cast< PlayerCharacter >( world.objectPtr( model.avatar() ) ) );
					if (aliens::isHugger(alienType()))
					{
						GameTimeCoordinate attack_time = type_stats.attack_time;
						GameTimeCoordinate attack_commit = type_stats.attack_commit;
						shared_ptr< HuggerAttackAction > attack_action( 
							new HuggerAttackAction( getSharedFromThis(world), player_ptr, 
								attack_time, attack_commit ) );
						shared_ptr< AlienActionComplete > observer( new AlienActionComplete( getSharedFromThis(world) ) );
						attack_action->setActionCompleteObserver( observer );
						model.actionEngine().addAction( attack_action );
					}
					else
					{
						int damage_amt = type_stats.attack_dmg - type_stats.attack_var + (rand() % (2*type_stats.attack_var));
						damage_amt = (std::min)(m_HP, damage_amt);
						int dmg_penetration = 0;
						GameTimeCoordinate attack_time = type_stats.attack_time;
						GameTimeCoordinate attack_commit = type_stats.attack_commit;
						shared_ptr< DirectAttackAction > attack_action( new DirectAttackAction( getSharedFromThis(world), player_ptr, 
							damage::Melee, damage_amt, dmg_penetration, attack_time, attack_commit ) );
						shared_ptr< AlienActionComplete > observer( new AlienActionComplete( getSharedFromThis(world) ) );
						attack_action->setActionCompleteObserver( observer );
						model.actionEngine().addAction( attack_action );
					}
					m_ai_timer = 0;
					return;
				}


				bool can_see_player( false );

				if (distance_to_player_squared <= (VISION_RANGE*VISION_RANGE))
				{
					{
						if (player_can_see_me)
						{
							can_see_player = true;
							player_obj.youCanSeeMe(model, *this, m_player_has_seen_me);
							m_player_has_seen_me = true;
						}
						else
						{
							//LoSSampler los_test( zone, 0, player_loc.x, player_loc.z );
							//lineCast(my_loc.x, my_loc.z, player_loc.x, player_loc.z, los_test, VisitCellFunctor());
							//can_see_player = !los_test.hit;

							if (model.avatarIsVisibleFrom(my_loc.zone, my_loc.x, my_loc.z))
							{
								can_see_player = true;
							}
						}
					}
				}

				has_los = can_see_player;
				if ((aliens::Worker == alienType()) && (distance_to_player_squared > 1))
					can_see_player = false;
				if ((aliens::Queen == alienType()) && (distance_to_player_squared > (16*16)))
					can_see_player = false;

				if (can_see_player)
				{
					m_activated = true;
					m_sees_player = true;

					if ((alienType() == aliens::Queen) && (!could_see_player_before) && (distance_to_player_squared >= 25))
					{
						model.alertAliensInRadius(my_loc.zone, my_loc.x, my_loc.z, 128); //TODO add a function to just alert all aliens in zone

						model.notifyFoundTheQueen();

						if (events)
							events->queenScreech(model, *this);

						m_ai_timer = 2000;
						m_behaviour = alien_behaviour::none;
						m_path_to_target.clear();
						return; //don't act, pause for 2000. TODO make queen screech an action
					}

					m_target = player_loc;

					Behaviour old_behaviour = m_behaviour;

					m_target_out_vent = WorldObject::WorldLocation();
					m_path_to_vent.clear();
					m_path_to_target.clear();
					m_behaviour = alien_behaviour::chasing_target;
					m_last_seen_player = (HUNT_PLAYER_TIME/2) + (rand() % (HUNT_PLAYER_TIME/2));
					m_random_searches = 1 + (rand() % 3);

					if (aliens::isRangedAttacker(alienType()))
					{ 
						int range = aliens::getAttackRange(alienType());
						int rsq = range*range;
						if ((distance_to_player_squared <= rsq) && (0 != (rand()&3)))
						{
							shared_ptr< PlayerCharacter > player_ptr( dynamic_pointer_cast< PlayerCharacter >( world.objectPtr( model.avatar() ) ) );
							GameTimeCoordinate attack_time = 800;
							shared_ptr< SpitterAttackAction > attack_action( 
								new SpitterAttackAction( getSharedFromThis(world), player_ptr, 
									aliens::getRangedAttackAmount(alienType()), rsq, attack_time ) );
							shared_ptr< AlienActionComplete > observer( new AlienActionComplete( getSharedFromThis(world) ) );
							attack_action->setActionCompleteObserver( observer );
							model.actionEngine().addAction( attack_action );
							m_ai_timer = 0;
							return;
						}
					}
				}
				else if ((aliens::Queen == alienType()) && m_activated && (alien_behaviour::none == m_behaviour))
				{
					m_target = player_loc;
					findPathToTarget(world, zone, player_loc);
				}
				else if (m_last_seen_player > 0)
				{
					m_target = player_loc;

					if (m_last_seen_player < (HUNT_PLAYER_TIME/2))
					{
						m_target.x += (rand() % 9) - 4;
						m_target.z += (rand() % 9) - 4;
						m_target.x = (std::max)(0, (std::min)(zone.sizeX()-1, m_target.x) );
						m_target.z = (std::max)(0, (std::min)(zone.sizeZ()-1, m_target.z) );
					}

					if ( (!aliens::isLargeAlien(alienType()) && (rand() & 1)) 
						 || !findPathToTarget(world, zone, m_target))
						findVent(world, zone, m_target);
				}
			}
		}
	}

	WorldObject::WorldLocation target( isInSubstructure() ? m_target_out_vent : m_target );

	if (isInSubstructure())
	{
		m_path_to_vent.clear();
		if (my_loc == m_target_out_vent)
		{
			if (!fire_here)
			{
				//exitVent(model);
				shared_ptr< EnterExitVentAction > action( new EnterExitVentAction( 
					dynamic_pointer_cast<Alien>( getSharedFromThis(world) ), 
					false, type_stats.traverse_vent_time ) );
				shared_ptr< AlienActionComplete > observer( new AlienActionComplete( getSharedFromThis(world) ) );
				action->setActionCompleteObserver( observer );
				model.actionEngine().addAction( action );
				m_ai_timer = 0;
			}
			return;
		}
	}

	if (alien_behaviour::path_to_vent == m_behaviour)
		if (!m_path_to_vent.empty())
		{
			if (terrain::accessSubstructure( zone.terrainAt(my_loc.x, my_loc.z) ))
			{
				//enterVent(model);
				shared_ptr< EnterExitVentAction > action( new EnterExitVentAction( 
					dynamic_pointer_cast<Alien>( getSharedFromThis(world) ), 
					true, type_stats.traverse_vent_time ) );
				shared_ptr< AlienActionComplete > observer( new AlienActionComplete( getSharedFromThis(world) ) );
				action->setActionCompleteObserver( observer );
				model.actionEngine().addAction( action );
				m_ai_timer = 0;
				return;
			}
			else
			{	//Follow path
				while (!m_path_to_vent.empty())
				{
					const PathPoint& next( m_path_to_vent.back() );
					if ((next.x == my_loc.x) && (next.z == my_loc.z))
						m_path_to_vent.pop_back();
					else if (!canOverlapOrBreak( world, zone, next.x, next.z ))
						m_path_to_vent.pop_back();
					else
						break;
				}
				if (!m_path_to_vent.empty())
				{
					const PathPoint& next( m_path_to_vent.back() );
					target.x = next.x;
					target.z = next.z;
					target.zone = my_loc.zone;
				}
			}
		}
		else
		{
			m_behaviour = alien_behaviour::none;
			m_player_has_seen_me = false;
		}


	if (alien_behaviour::path_to_target == m_behaviour)
		if (!m_path_to_target.empty())
		{
			//Follow path
			while (!m_path_to_target.empty())
			{
				const PathPoint& next( m_path_to_target.back() );
				if ((next.x == my_loc.x) && (next.z == my_loc.z))
					m_path_to_target.pop_back();
				else if (!canOverlapOrBreak( world, zone, next.x, next.z ))
					m_path_to_target.pop_back();
				else
					break;
			}
			if (!m_path_to_target.empty())
			{
				const PathPoint& next( m_path_to_target.back() );
				target.x = next.x;
				target.z = next.z;
				target.zone = my_loc.zone;
			}
		}
		else
		{
			m_behaviour = alien_behaviour::none;
			m_player_has_seen_me = false;
		}

	if (my_loc.zone == target.zone)
	{
		int odx = (my_loc.x < target.x) ? 1 : (my_loc.x > target.x) ? -1 : 0;
		int odz = (my_loc.z < target.z) ? 1 : (my_loc.z > target.z) ? -1 : 0;
		int dx = odx;
		int dz = odz;

		if ((0==dx) && (0==dz))
		{
			pickNewTarget(world, zone);
		}
		else
		{
			//Zigzagging
			if (m_sees_player && player_can_see_me && (aliens::Worker != alienType()))
			{
				if (0 == dx)
					dx = (rand()%3)-1;
				else if (0 == dz)
					dz = (rand()%3)-1;
				else if (rand()&1)
				{
					if (rand()&1)
						dx = 0;
					else
						dz = 0;
				}
			}

			if (!AHGameObject::canOverlap( world, zone, my_loc.x + dx, my_loc.z + dz ))
			{
				if (aliens::isLargeAlien(alienType()) && terrain::isBreakable(zone.terrainAt(my_loc.x + dx, my_loc.z + dz)))
				{
					if ((0 == dx) || (0 == dz)) //first get in line. No diagonal door ramming
					{
						WorldObject::WorldLocation move_to( my_loc.zone, my_loc.x + dx, my_loc.z + dz );
						shared_ptr< BreakDoorAction > break_action( new BreakDoorAction( getSharedFromThis(world), move_to, 6000 ) );
						shared_ptr< AlienActionComplete > observer( new AlienActionComplete( getSharedFromThis(world) ) );
						break_action->setActionCompleteObserver( observer );
						model.actionEngine().addAction( break_action );

						goto endAI;
					}

					goto skip; // TODO
				}

				if ( ((my_loc.x + dx) == target.x) && ((my_loc.z + dz) == target.z) )
				{
					dx = 0;
					dz = 0;

					//We were probably following a path, and found ourselves blocked.
					//It could be another alien that blocked us, or something like a door.
					++m_blocked_on_path;
					if ((m_blocked_on_path > 9) || (!canOverlap( zone.terrainAt(my_loc.x + odx, my_loc.z + odz) )))
					{
						m_blocked_on_path = 0;
						m_path_to_vent.clear();
						m_path_to_target.clear();
						m_behaviour = alien_behaviour::chasing_target;
						m_random_searches += 1;
						pickNewTarget( world, zone );
					}
					//...otherwise, wait and try again.
				}
				else
				{
					skip:

					if ((dz != 0) && AHGameObject::canOverlap( world, zone, my_loc.x, my_loc.z + dz ))
						dx = 0;
					else if ((dx != 0) && AHGameObject::canOverlap( world, zone, my_loc.x + dx, my_loc.z ))
						dz = 0;
					else 
					{
						if (0 == dx)
						{
							if (AHGameObject::canOverlap( world, zone, my_loc.x-1, my_loc.z + dz ))
								dx = -1;
							else if (AHGameObject::canOverlap( world, zone, my_loc.x+1, my_loc.z + dz ))
								dx = 1;
						}
						else if (0 == dz)
						{
							if (AHGameObject::canOverlap( world, zone, my_loc.x + dx, my_loc.z-1 ))
								dz = -1;
							else if (AHGameObject::canOverlap( world, zone, my_loc.x + dx, my_loc.z+1 ))
								dz = 1;
						}
					}

				}
			}

			if (isInSubstructure())
				if (!AHGameObject::canOverlap( world, zone, my_loc.x + dx, my_loc.z + dz ))
				{
					dx = (rand() % 3) - 1;
					dz = (rand() % 3) - 1;
				}

			//Don't try to move to a location containing something we can't overlap with.
			//Also, don't be silly enough to move into fire, unless you're in fire already.
			if ( (!AHGameObject::canOverlap( world, zone, my_loc.x + dx, my_loc.z + dz ))
				 || ((!isInSubstructure()) && (!fire_here) && (isFireThere( world, zone, my_loc.x + dx, my_loc.z + dz ))) )
			{
				dx = 0;
				dz = 0;
			}

			if ( (dx != 0) || (dz != 0) )
			{
				m_blocked_on_path = 0;
				WorldObject::WorldLocation move_to( my_loc.zone, my_loc.x + dx, my_loc.z + dz );

				if ((!isInSubstructure()) && (aliens::Worker != alienType()))
					if ((distance_to_player_squared > 9) && (!m_player_has_seen_me))
					{
						if (model.isVisible(move_to.zone, move_to.x, move_to.z) && (!model.isVisible(my_loc.zone, my_loc.x, my_loc.z)))
							if (rand() & 7)
								return;
					}

				GameTimeCoordinate move_time(0);
				if ((dx != 0) && (dz != 0))
				{
					move_time = isInSubstructure() ? type_stats.substructure_move_diagonal_speed : type_stats.move_diagonal_speed;
				}
				else
				{
					move_time = isInSubstructure() ? type_stats.substructure_move_speed : type_stats.move_speed;
				}
				if (aliens::isSneaky(alienType()) && (!isInSubstructure()) && (!playerHasSeenMe()))
				{ //Trade speed for stealth.
					move_time *= 2;
				}
				shared_ptr< DiscreteMoveAction > move_action( new DiscreteMoveAction( getSharedFromThis(world), move_to, move_time ) );
				shared_ptr< AlienActionComplete > observer( new AlienActionComplete( getSharedFromThis(world) ) );
				move_action->setActionCompleteObserver( observer );
				model.actionEngine().addAction( move_action );
			}
			else
			{
				if (alien_behaviour::chasing_target == m_behaviour)
				{
					bool done(false);
					if (aliens::isLargeAlien(alienType()) || (rand()&1))
					{
						done = findPathToTarget(world, zone, target);
					}
					if (!done)
						if (aliens::isLargeAlien(alienType()) || (!findVent(world, zone, target)))
						{
							pickNewTarget( world, zone );
						}
				}
			}
		}
	}

endAI:

	if (!(actions().empty()))
		m_ai_timer = 0;
}

void Alien::pickNewTarget( const RL_shared::World& world, const RL_shared::Zone& zone )
{
	if (isInSubstructure())
		return;

	const WorldLocation my_loc( location() );

	if (m_random_searches > 0)
	{
		--m_random_searches;
		for (int attempts(5); attempts > 0; --attempts)
		{
			int o1 = (7 + (rand() % 3)) * ((rand() & 1) ? 1 : -1);
			int o2 = rand() % 7;

			if (WorldObject::WorldLocation::INVALID_ZONE == m_target.zone)
				m_target = my_loc;

			if (rand() & 1)
			{
				m_target.x += o1;
				m_target.z += o2;
			}
			else
			{
				m_target.x += o2;
				m_target.z += o1;
			}
			m_target.x = (std::max)(1, (std::min)(zone.sizeX()-2, m_target.x));
			m_target.z = (std::max)(1, (std::min)(zone.sizeZ()-2, m_target.z));

			if (m_target != my_loc)
				if ( AHGameObject::canOverlap(world, zone, m_target.x, m_target.z) )
				{
					m_behaviour = alien_behaviour::chasing_target;
					break;
				}
		}
	}
	else
	{
		m_random_searches = 0;
		m_path_to_vent.clear();
		m_path_to_target.clear();
		m_target = WorldObject::WorldLocation();
		m_behaviour = alien_behaviour::none;
		m_player_has_seen_me = false;
	}
}

bool Alien::enterVent(AHGameModel& model)
{
	m_behaviour = alien_behaviour::moving_in_substructure;
	m_path_to_vent.clear();
	return true;
}
bool Alien::exitVent(AHGameModel& model)
{
	const WorldLocation my_loc( location() );
	World& world( model.world() );
	if (!world.zoneExists(my_loc.zone))
		return false;
	const Zone& zone( world.zone( my_loc.zone ) );

	bool out_ok(true);

	m_behaviour = alien_behaviour::path_to_target;

	if (!AHGameObject::canOverlap(world, zone, my_loc.x, my_loc.z))
	{
		out_ok = false;

		const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
		const int dz[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

		for (int ti=0; ti<8; ++ti)
		{
			int tx = my_loc.x + dx[ti];
			int tz = my_loc.z + dz[ti];
			if (AHGameObject::canOverlap(world, zone, tx, tz))
			{
				if (moveTo(model, WorldLocation(my_loc.zone, tx, tz), true))
				{
					out_ok = true;
					break;
				}
			}
		}
	}

	if (!out_ok)
	{
		m_behaviour = alien_behaviour::moving_in_substructure;
		//can't get out this vent. Should probably pick a different one, but for now, wait
		return false;
	}
	else
	{
		m_target_out_vent = WorldObject::WorldLocation();
	}
	return true;
}

int Alien::takeDamage( AHGameModel& model, DBKeyValue attacker_key, damage::Type type, int amount, int penetration )
{
	if (damage::Acid == type)
		return 0;

	if (m_HP <= 0)
		return 0;

	if (isInSubstructure())
		return 0;

	//Queens and Hulks are fire-resistant
	if ((damage::Fire == type) && ((aliens::Queen == alienType()) || (aliens::Hulk == alienType())))
	{
		if (amount > 1)
			amount /= 2;
		else if (1 == amount)
			amount = rand() & 1;
	}

	//All aliens notice you if you attack them, even Workers.
	m_last_seen_player = HUNT_PLAYER_TIME;

	const alien_data::AlienData& type_stats( alien_data::getAlienData( alienType() ) );

	int armour = damage::Fire == type ? 0 : type_stats.armour;
	armour -= penetration;
	armour = (std::max)(0, armour);

	amount -= armour;
	amount = (std::max)(0, amount);
	amount = (std::min)(m_HP, amount);

	m_HP -= amount;
	m_HP = (std::max)(0, m_HP);

	boost::shared_ptr<IGameEvents> events( model.gameEventsObserver() );
	if (events)
	{
		events->alienHit(model, key(), type, amount);
		if (m_HP <= 0)
		{
			events->alienDies(model, key());
		}
	}

	WorldLocation my_loc( location() );

	if (m_HP <= 0)
	{
		World& world( model.world() );
		if (world.zoneExists(my_loc.zone))
		{
			Zone& zone( world.zone( my_loc.zone ) );
			shared_ptr< Decoration > corpse( new Decoration( decoration::AlienCorpse ) );
			DBKeyValue corpse_key( world.addWorldObject( corpse ) );
			corpse->moveTo(model, my_loc, true);
		}
		if (alienType() == aliens::Queen)
		{
			model.notifyKilledTheQueen();
		}

		//if (world.objectExists(attacker_key))
		//{
		//	AHGameObject& attacker( dynamic_cast< AHGameObject& >( world.object(attacker_key) ) );

		//	if (objects::Player == attacker.type())
		//	{
		//		PlayerCharacter& player_obj( dynamic_cast< PlayerCharacter& >( attacker ) );
		//		player_obj.addXP( type_stats.XP );
		//	}

		//	boost::shared_ptr< PlayerStats > stats( attacker.stats() );
		//	if (stats)
		//		(++stats->kills( alienType() ));
		//}
		if (world.objectExists(model.avatar()))
		{
			PlayerCharacter& player_obj( dynamic_cast< PlayerCharacter& >( world.object( model.avatar() ) ) );
			//player_obj.addXP( type_stats.XP * (AHGameModel::Quick == model.gameType() ? 2 : 1) );
			player_obj.addXP( type_stats.XP );
			boost::shared_ptr< PlayerStats > stats( player_obj.stats() );
			if (stats)
				(++stats->kills( alienType() ));
		}
	}

	if ((damage::Fire != type) && (damage::Acid != type) && (amount > 0 ))
	{
		World& world( model.world() );
		if (world.zoneExists(my_loc.zone))
		{
			const Zone& zone( world.zone( my_loc.zone ) );
			for (int tz=-1; tz<=1; ++tz)
				for (int tx=-1; tx<=1; ++tx)
				{
					int sx(my_loc.x+tx), sz(my_loc.z+tz);
					if (zone.isWithin(sx,sz))
					{
						const ObjectList& objects( zone.objectsAt( sx, sz ) );
						BOOST_FOREACH( DBKeyValue obj_key, objects )
						{
							if (world.objectExists(obj_key))
							{
								AHGameObject& game_obj( dynamic_cast< AHGameObject& >( world.object(obj_key) ) );
								int splash( rand() % amount );
								game_obj.acidSplash( model, splash, (std::max)(3, type_stats.HP/3) );
							}
						}
					}
				}
		}
	}

	return amount;
}

void Alien::stun( AHGameModel& model, RL_shared::GameTimeCoordinate time )
{
	if (m_HP <= 0)
		return;

	if (isInSubstructure())
		return;

	if (alienType() == aliens::Queen)
		time /= 2;	//Queen is stun-resistant

	m_stun_timer = (std::max)(time, m_stun_timer);

	boost::shared_ptr<IGameEvents> events( model.gameEventsObserver() );
	if (events)
		events->actorStunned(model, *this, time);
}

bool Alien::blocksDoor(void) const
{
	return !isInSubstructure();
}

void Alien::soundAlert(World& world, const Zone& zone, int x, int z, int error)
{
	if ((aliens::Worker == alienType()) || (aliens::Queen == alienType()))
		return; //workers are not interested in sounds, and neither is the queen.

	if (error > 0)
	{
		//x += (rand() % 9) - 4;
		//z += (rand() % 9) - 4;
		x += (rand() % (2*error)+1) - 4;
		z += (rand() % (2*error)+1) - 4;
		x = (std::max)(0, (std::min)(zone.sizeX()-1, x) );
		z = (std::max)(0, (std::min)(zone.sizeZ()-1, z) );
	}

	m_target = location();
	m_target.x = x;
	m_target.z = z;

	if (!isInSubstructure())
	{
		if (!findPathToTarget(world, zone, m_target))
			if (!findVent(world, zone, m_target))
				if (rand()&1)
					m_behaviour = alien_behaviour::chasing_target;
	}
	else
		findExitVent(world, zone, m_target);

	m_random_searches = 1 + (rand() % 3);
}





bool soundExploreForAliens( void * data, World& world, const Zone& zone, int x, int z, int ori_x, int ori_z, int& cost, bool&, bool& )
{
	if ((x < 0) || (z < 0) || (x >= zone.sizeX()) || (z >= zone.sizeZ()))
		return false;

	TerrainType there( zone.terrainAt(x, z) );
	if (!terrain::isPassable(there))
	{
		if (terrain::Door == terrain::getType(there))
			cost += (COST_RADIX*3);
		else
			cost += (COST_RADIX*10);
	}

	int error = cost/(COST_RADIX*10);

	const ObjectList& objects( zone.objectsAt(x,z) );
	BOOST_FOREACH( DBKeyValue obj_key, objects )
	{
		if (world.objectExists(obj_key))
		{
			AHGameObject& game_obj( dynamic_cast<AHGameObject&>(world.object(obj_key)) );
			if (objects::Alien == game_obj.type())
			{
				Alien& alien_obj( dynamic_cast<Alien&>(game_obj) );
				alien_obj.soundAlert(world, zone, ori_x, ori_z, error);
			}
		}
	}

	return true;
}


void soundAlertAliens(AHGameModel& model, RL_shared::DBKeyValue zone_key, int x, int z, int range)
{
	World& world( model.world() );

	if (!world.zoneExists(zone_key))
		return;

	const Zone& zone( world.zone(zone_key) );
	explore( 0, world, zone, x, z, 2*range, soundExploreForAliens, 0 );

	//model.alertAliensInRadius(zone_key, x, z, range);
}


void alienMakesSound(AHGameModel& model, Alien& alien, int volume)
{
	WorldObject::WorldLocation loc( alien.location() );

	if (!alien.isInSubstructure())
		if (model.isVisible(loc.zone, loc.x, loc.z))
			return;

	World& world( model.world() );

	if (!world.objectExists( model.avatar() ))
		return;

	const PlayerCharacter& player( dynamic_cast< const PlayerCharacter& >( world.object( model.avatar() ) ) );

	boost::uint32_t hearing_val( model.hearingValue(loc.zone, loc.x, loc.z) );

	if (alien.isInSubstructure())
		hearing_val += (hearing_val >> 1);

	bool is_large( aliens::isLargeAlien(alien.alienType()) );
	if (is_large)
		volume <<= 1;

	unsigned int range = (unsigned int)(alien.isInSubstructure() ? player.getCeilingHearingRange() : player.getOpenHearingRange());
	range *= volume;

	if (range < hearing_val)
		return;

	unsigned int chance = (std::max)((int)0, player.getHearingChance());
	chance += (volume-1);
	if (hearing_val >= 4)
	{
		chance = (unsigned int)(std::max)( (int)0, (int)((int)chance - (rand() % ((hearing_val/4)+1) )) );
	}

	//Stealthy aliens
	if (aliens::isHugger( alien.alienType() ) || 
		( aliens::isSneaky( alien.alienType() )
		 && (!alien.isInSubstructure())
		 && (!alien.playerHasSeenMe())
		 )
		)
	{ //Trade speed for stealth.
		chance >>= 1;
	}

	bool made_sound(false);

	boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );

	if (alien.isInSubstructure())
	{
		if ( (is_large && (((unsigned int)rand() & 1) < chance)) || (((unsigned int)rand() & 3) < chance) )
		{
			made_sound = true;

			if (events)
				events->alienMakesSoundAbove(model, alien);
		}
	}
	else
	{
		if ( (is_large && (((unsigned int)rand() & 3) < chance)) || (((unsigned int)rand() & 7) < chance) )
		{
			made_sound = true;

			if (events)
				events->alienMakesSoundNearby(model, alien, (hearing_val*hearing_val));
		}
	}

	if (made_sound && (player.hasPerk(player_perks::Hearing1) || (rand()&1)))
	{
		WorldObject::WorldLocation sound_loc( loc );

		if (!player.hasPerk(player_perks::Hearing1))
		{
			sound_loc.x += (rand()%7) - 3;
			sound_loc.z += (rand()%7) - 3;
		}
		else if (((unsigned int)rand() & 7) > chance)
		{
			sound_loc.x += (rand()%3) - 1;
			sound_loc.z += (rand()%3) - 1;
		}

		shared_ptr< Ping > ping( new Ping( ping::Noise, 350 ) );
		DBKeyValue ping_key( world.addWorldObject( ping ) );
		ping->moveTo( model, sound_loc, true );
	}
}


struct ExploreForTarget
{
	int x, z;
	bool ignore_doors;
};
bool exploreForTarget( void * data, World& world, const Zone& zone, int x, int z, int ori_x, int ori_z, int& cost, bool& halt, bool& goal )
{
	if ((x < 0) || (z < 0) || (x >= zone.sizeX()) || (z >= zone.sizeZ()))
		return false;

	const ExploreForTarget& params( *(reinterpret_cast<const ExploreForTarget*>(data)) );

	if ((x == params.x) && (z == params.z))
	{
		halt = true;
		goal = true;
		return false;
	}

	TerrainType there( zone.terrainAt(x, z) );

	if (!terrain::isPassable(there, params.ignore_doors))
		return false;

	return true;
}
bool Alien::findPathToTarget(World& world, const Zone& zone, WorldObject::WorldLocation target_loc)
{
	const WorldLocation my_loc( location() );

	m_path_to_vent.clear();
	m_path_to_target.clear();

	ExploreForTarget params;
	params.x = target_loc.x;
	params.z = target_loc.z;
	params.ignore_doors = aliens::isLargeAlien( alienType() );
	explore((void*)&params, world, zone, my_loc.x, my_loc.z, 50, exploreForTarget, &m_path_to_target);

	if (!m_path_to_target.empty())
	{
		m_behaviour = alien_behaviour::path_to_target;
		return true;
	}
	return false;
}


bool exploreForVents( void * data, World& world, const Zone& zone, int x, int z, int ori_x, int ori_z, int& cost, bool& halt, bool& goal )
{
	if ((x < 0) || (z < 0) || (x >= zone.sizeX()) || (z >= zone.sizeZ()))
		return false;

	TerrainType there( zone.terrainAt(x, z) );

	if (terrain::accessSubstructure( there ))
	{
		halt = true;
		goal = true;
		return false;
	}

	if (!terrain::isPassable(there))
		return false;

	return true;
}

bool Alien::findVent(World& world, const Zone& zone, WorldObject::WorldLocation target_loc)
{
	const WorldLocation my_loc( location() );

	m_path_to_vent.clear();
	m_path_to_target.clear();

	if (aliens::isLargeAlien(alienType()))
		return false;

	explore(0, world, zone, my_loc.x, my_loc.z, 30, exploreForVents, &m_path_to_vent);

	if (!m_path_to_vent.empty())
	{
		if (findExitVent(world, zone, target_loc))
		{
			m_behaviour = alien_behaviour::path_to_vent;
			return true;
		}
	}
	return false;
}

bool Alien::findExitVent(World& world, const Zone& zone, WorldObject::WorldLocation target_loc)
{
	const WorldLocation my_loc( location() );

	Path out_vent_to_target;

	explore(0, world, zone, target_loc.x, target_loc.z, 30, exploreForVents, &out_vent_to_target);

	if (!out_vent_to_target.empty())
	{
		std::reverse( out_vent_to_target.begin(), out_vent_to_target.end() );
		m_target_out_vent = WorldObject::WorldLocation( my_loc.zone, out_vent_to_target.back().x, out_vent_to_target.back().z );

		m_path_to_target.clear();
		m_path_to_target.resize( out_vent_to_target.size() );
		std::copy( out_vent_to_target.begin(), out_vent_to_target.end(), m_path_to_target.begin() );

		return true;
	}

	return false;
}

bool Alien::pickExitVent(RL_shared::World& world, const RL_shared::Zone& zone, WorldObject::WorldLocation target_loc)
{
	ASSERT( isInSubstructure() );

	if (findExitVent(world, zone, target_loc))
	{
		m_behaviour = alien_behaviour::moving_in_substructure;
		return true;
	}

	return false;
}


bool Alien::queenJump( AHGameModel& model )
{
	const alien_data::AlienData& type_stats( alien_data::getAlienData( alienType() ) );

	World& world( model.world() );
	const WorldObject::WorldLocation my_loc( location() );
	const Zone& zone( world.zone( my_loc.zone ) );

	if (world.objectExists(model.avatar()))
	{
		const WorldObject& player_obj( world.object( model.avatar() ) );
		const WorldObject::WorldLocation player_loc( player_obj.location() );

		//If player tries to run away from queenie, she will try to cut them off.
		//Relies on her being updated at all times, regardless of whether she is in a zone that is being processed.
		if (world.zoneExists(player_loc.zone) && (player_loc.zone != my_loc.zone))
		{
			Zone& player_zone( world.zone( player_loc.zone ) );
			OverWorld& overworld( model.overworld() );
			OverWorld::BlockAndFloor bnf( overworld.getBlockAndFloor( player_loc.zone ) );

			if ((1 == bnf.block) && (0 == bnf.floor))
			{ //Wait by the colony exit.
				int tx = 0;
				for (; tx < player_zone.sizeX(); ++tx)
					if (terrain::Bridge == player_zone.terrainAt(tx, 0))
						if (moveTo(model, WorldLocation(player_loc.zone, tx, 1), true))
							goto Jumped;
			}

			struct LocalFunctions
			{
				static bool tryJump(AHGameModel& model, Alien& me, const WorldObject::WorldLocation& player_loc, int tx, int tz)
				{
					int dx = player_loc.x - tx;
					int dz = player_loc.z - tz;
					int distsq = (dx*dx)+(dz*dz);
					if (distsq > 100)
					{
						return me.moveTo(model, WorldLocation(player_loc.zone, tx, tz), true);
					}
					return false;
				}
			};

			if (rand() & 1)
			{ //Find a bridge for her to wait at
				int tx(0), tz(0);
				for (; tx < player_zone.sizeX(); ++tx)
				{
					if (terrain::Bridge == player_zone.terrainAt(tx, 0))
						if (0 == (rand() & 7))
							if (LocalFunctions::tryJump(model, *this, player_loc, tx, 1))
								goto Jumped;

					if (terrain::Bridge == player_zone.terrainAt(tx, player_zone.sizeZ()-1))
						if (0 == (rand() & 7))
							if (LocalFunctions::tryJump(model, *this, player_loc, tx, player_zone.sizeZ()-2))
								goto Jumped;
				}
				for (; tz < player_zone.sizeZ(); ++tz)
				{
					if (terrain::Bridge == player_zone.terrainAt(0, tz))
						if (0 == (rand() & 7))
							if (LocalFunctions::tryJump(model, *this, player_loc, 1, tz))
								goto Jumped;

					if (terrain::Bridge == player_zone.terrainAt(player_zone.sizeX()-1, tz))
						if (0 == (rand() & 7))
							if (LocalFunctions::tryJump(model, *this, player_loc, player_zone.sizeX()-2, tz))
								goto Jumped;
				}
			}

			//Put her at one of the stairwells, but not the one that the player just went down. 
			//(Unless it's the only one and they've moved away from it).
			int try_x[4] = {2, 2, player_zone.sizeX()-3, player_zone.sizeX()-3};
			int try_z[4] = {2, player_zone.sizeZ()-3, 2, player_zone.sizeZ()-3};
			int st = rand() & 3;
			int t = st;
			for (;;)
			{
				int tx = try_x[t];
				int tz = try_z[t];
				//Don't sit on completely ruined stairwells.
				for (int cz=-1; cz<2; ++cz)
					for (int cx=-1; cx<2; ++cx)
					{
						TerrainType there( player_zone.terrainAt(tx+cx, tz+cz) );
						if ((terrain::StairsDown == there) || (terrain::StairsUp == there))
						{
							if (LocalFunctions::tryJump(model, *this, player_loc, tx, tz))
								goto Jumped;
						}
					}

				++t;
				t &= 3;
				if (t == st)
					break;
			}
		}
	}

	return false;

Jumped:

	//Sit and wait here for the player.
	m_behaviour = alien_behaviour::ambushing;
	m_last_seen_player = 0;
	m_ai_timer = 0;
	m_player_has_seen_me = false;

	//also, she regains health.
	m_HP = (std::max)(m_HP, (std::min)(50, m_HP + 10));

	//Also, this happens.
	model.increaseEndgameDanger();

	return true;
}

std::string Alien::getThereIsString(bool) const
{
	return "";
}

std::string Alien::getSelectName(bool status) const
{
	using namespace aliens;

	std::string type_str;
	switch( alienType() )
	{
		case Juvenile:
			type_str = "Juvenile"; break;
		case Harrier:
			type_str = "Harrier"; break;
		case Worker:
			type_str = "Worker"; break;
		case Warrior:
			type_str = "Warrior"; break;
		case Hunter:
			type_str = "Hunter"; break;
		case Elite:
			type_str = "Elite"; break;
		case Slayer:
			type_str = "Slayer"; break;
		case Hulk:
			type_str = "Hulk"; break;
		case Queen:
			type_str = "Queen"; break;
		case Facehugger:
			type_str = "Facehugger"; break;
		case Fasthugger:
			type_str = "Fasthugger"; break;
		case Spitter:
			type_str = "Spitter"; break;
		case DeathSpitter:
			type_str = "Death Spitter"; break;
		case Gladiator:
			type_str = "Gladiator"; break;
		case Reaper:
			type_str = "Reaper"; break;
		default:
			type_str = "Alien";
	}

	if (status)
	{
		if (m_stun_timer > 0)
			type_str += " (stun)";

		type_str += " [";
		type_str += boost::lexical_cast<std::string>( m_HP );
		type_str += "]";
	}

	return type_str;
}



}

