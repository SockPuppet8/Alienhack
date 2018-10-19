#include "WeaponAttackAction.hpp"
#include "../Objects/LaserTrail.hpp"
#include "../Objects/ObjectType.hpp"
#include "../Objects/Alien.hpp"
#include "../Objects/Weapon.hpp"
#include "../Objects/PlayerCharacter.hpp"
#include "../Objects/PlayerStats.hpp"
#include "../Objects/WeaponData.hpp"
#include "../Objects/Explosion.hpp"
#include "../LoSSampler.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"
#include "World-2DTiles/Zone.hpp"
#include "Algorithm/Algorithms-2D.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>



namespace AlienHack
{



using namespace RL_shared;
using namespace boost;




WeaponAttackAction::WeaponAttackAction( 
	AHGameModel& model, 
	World& world, 
	boost::shared_ptr< PlayerCharacter > player, 
	boost::shared_ptr< Weapon > weapon, 
	int target_offs_x, int target_offs_z
	)
	: RL_shared::FastAction(model)
	, m_player( player ) 
	, m_weapon( weapon )
	, m_time_remaining( 0 ) 
	, m_num_shots( 0 )
	, m_shot_interval( 0 )
	, m_shot_timer( 0 )
	, m_target_x( target_offs_x )
	, m_target_z( target_offs_z )
	, m_target_obj( INVALID_KEY )
	, m_first_shot( true )
{
	ASSERT( m_player.lock() );
	ASSERT( m_weapon.lock() );

	const WeaponData& weapon_stats( weapon->weaponData() );
	m_time_remaining	= weapon_stats.fire_time;
	m_num_shots			= weapon_stats.num_shots;
	m_shot_interval		= weapon_stats.shot_interval;
	m_shot_timer		= 0;//weapon_stats.shot_interval;

	if (player->hasPerk(player_perks::MultiShotAccuracy2) && (weapon_stats.num_shots > 1) && (damage::Bullet == weapon_stats.shot_damage_type))
	{
		m_time_remaining = (3 * m_time_remaining) / 4;
		m_shot_interval = (3 * m_shot_interval) / 4;
	}

	WorldObject::WorldLocation player_loc( player->location() );
	if (WorldObject::WorldLocation::INVALID_ZONE != player_loc.zone)
	{
		Zone& zone( world.zone( player_loc.zone ) );
		int tx = player_loc.x + target_offs_x; 
		int tz = player_loc.z + target_offs_z;
		if (zone.isWithin(tx,tz))
		{
			const ObjectList& objects( zone.objectsAt( tx, tz ) );
			BOOST_FOREACH( DBKeyValue obj_key, objects )
			{
				if (world.objectExists(obj_key))
				{
					const AHGameObject& game_obj( dynamic_cast<const AHGameObject&>(world.object(obj_key)) );
					if (objects::Alien == game_obj.type())
					{
						m_target_obj = obj_key;
						break;
					}
				}
			}
		}
	}
}


//This is the test to determine what the player is aiming at. 
class GetFirstTarget : public IIsBlockingObject
{
	const World& m_world;
	mutable DBKeyValue m_found_alien;

public:

	GetFirstTarget( const World& world ) : m_world( world ), m_found_alien(INVALID_KEY)
	{
	}

	virtual bool isBlocking( DBKeyValue obj_key ) const
	{
		if (INVALID_KEY == obj_key)
			return false;
		if (!m_world.objectExists(obj_key))
			return false;
		const AHGameObject& game_obj( dynamic_cast<const AHGameObject&>(m_world.object(obj_key)) );
		if (game_obj.blocksProjectiles())
			return true;
		WorldObjectType type( game_obj.type() );
		if (objects::Alien == type)
		{
			const Alien& alien( dynamic_cast<const Alien&>(game_obj) );
			if (alien.isInSubstructure())
				return false;
			if (alien.getHP() <= 0)
				return false;

			m_found_alien = obj_key;
			return true;
		}

		return false;
	}
	DBKeyValue foundAlien(void) const
	{
		return m_found_alien;
	}
};

int adjustAccuracyForDistance( int accuracy, int accuracy_falloff, int dist )
{
	accuracy -= (dist / accuracy_falloff);
	//accuracy = (std::max)(2, accuracy);
	return accuracy;
}

int adjustAccuracyForAlien( int accuracy, int accuracy_falloff, int dist, const Alien& alien, bool single_shot )
{
	int base_evasion = aliens::getEvasion(alien.alienType());
	int evasion = base_evasion * (single_shot ? 2 : 1);

	if (single_shot && (dist < 3))
	{
		int point_blank_bonus = (std::max)(2, 4-base_evasion);
		accuracy = point_blank_bonus*accuracy;
	}

	accuracy -= (dist / accuracy_falloff);

	if (aliens::isLargeAlien(alien.alienType()))
		accuracy += 6;

	accuracy -= evasion;

	//accuracy = (std::max)(2, accuracy);
	return accuracy;
}

bool accuracyTest( int accuracy, bool is_spread_weapon )
{
	if (is_spread_weapon)
		accuracy = (std::max)(2, accuracy);

	if (accuracy >= 2)
		return (0 != (rand() % accuracy));

	accuracy = -(accuracy-4);
	//accuracy = (std::max)(2, accuracy); //This line unnecessary given the above lines, but left as a reminder that the below statement is invalid for (accuracy < 2).
	return (0 == (rand() % accuracy));
}

//This is the test to determine what is actually hit in the direction that the projectile actually goes. 
class HitTestObject : public IIsBlockingObject
{
	const AHGameModel& m_model;
	const World& m_world;
	int m_accuracy;
	int m_accuracy_falloff;
	int sx, sz;
	int tx, tz;
	mutable DBKeyValue m_first_target;
	bool m_is_single_shot;
	bool m_is_spread;
public: 
	HitTestObject( const AHGameModel& model, const World& world, 
				   int accuracy, int accuracy_falloff, bool is_single_shot, bool is_spread, 
				   int startx, int startz, int target_x, int target_z 
				   ) 
		: m_model(model), m_world( world ), m_accuracy(accuracy), m_accuracy_falloff( accuracy_falloff ), 
		sx(startx), sz(startz), tx(target_x), tz(target_z), m_first_target(INVALID_KEY), 
		m_is_single_shot(is_single_shot), m_is_spread( is_spread )
	{
		m_accuracy = (std::max)(1, m_accuracy);
	}
	virtual bool isBlocking( DBKeyValue obj_key ) const
	{
		if (INVALID_KEY == obj_key)
			return false;
		if (!m_world.objectExists(obj_key))
			return false;
		const AHGameObject& game_obj( dynamic_cast<const AHGameObject&>(m_world.object(obj_key)) );
		if (game_obj.blocksProjectiles())
			return true;

		WorldObject::WorldLocation loc( game_obj.location() );
		if (!m_model.isVisible(loc.zone, loc.x, loc.z))
			return 0 != (rand()&1);

		WorldObjectType type( game_obj.type() );
		if (objects::Alien == type)
		{
			const Alien& alien( dynamic_cast<const Alien&>(game_obj) );
			if (alien.isInSubstructure())
				return false;
			if (alien.getHP() <= 0)
				return false;

			if (INVALID_KEY == m_first_target)
				m_first_target = obj_key;

			int dx = loc.x - sx;
			int dz = loc.z - sz;
			int dist = (int)(isqrt( (dx*dx) + (dz*dz) ));

			int accuracy = adjustAccuracyForAlien(m_accuracy, m_accuracy_falloff, dist, alien, m_is_single_shot);

			return accuracyTest(accuracy, m_is_spread);
		}
		else if (objects::isPassiveTarget((objects::type)type))
		{
			//Barrels etc are usually hit if you're specifically aiming for them, but otherwise only accidentally. 
			WorldObject::WorldLocation loc( game_obj.location() );
			bool avoid_on_success = (!objects::isPreferredTarget((objects::type)type)) && ((tx != loc.x) || (tz != loc.z));

			if ((!avoid_on_success) && (INVALID_KEY == m_first_target))
				m_first_target = obj_key;

			int dx = loc.x - sx;
			int dz = loc.z - sz;
			int dist = (int)(isqrt( (dx*dx) + (dz*dz) ));

			int accuracy = adjustAccuracyForDistance(m_accuracy, m_accuracy_falloff, dist);

			bool success = accuracyTest(accuracy, m_is_spread);
			return success ? !avoid_on_success : avoid_on_success;
		}
		return false;
	}
	DBKeyValue getFirstTarget(void) const
	{
		return m_first_target;
	}
};







void DoWeaponDamage::execute(void)
{
	AHGameModel& model(*m_model);
	World& world(model.world());
	if (world.objectExists(m_hit_obj))
	{
		AHGameObject& game_obj( dynamic_cast< AHGameObject& >( world.object( m_hit_obj ) ) );
		game_obj.takeDamage( model, m_source_obj, m_damage_type, m_amount, m_penetration );
	}
	//TODO move this effect to Explosion
	if ((weapon::CanisterRifle == m_weapon_type) && world.zoneExists(m_hit_loc.zone))
	{
		// TODO mods might affect this
		const SplashDamage& splash( weapon_data::getWeaponSplashDamage( weapon::CanisterRifle, weapon::Special_None ) );
		Zone& zone( world.zone(m_hit_loc.zone) );

		//TODO add alert aliens here

		shared_ptr< Explosion > expl_obj( new Explosion( model, explosion::KrakBlast ) );
		DBKeyValue expl_key( world.addWorldObject( expl_obj ) );
		expl_obj->moveTo( model, m_hit_loc, true );

		for (int offz=-1; offz < 2; ++offz)
			for (int offx=-1; offx < 2; ++offx)
			{
				int tx = m_hit_loc.x + offx;
				int tz = m_hit_loc.z + offz;
				if (zone.isWithin(tx,tz))
				{
					const ObjectList& objects( zone.objectsAt(tx, tz) );
					BOOST_FOREACH( DBKeyValue object_key, objects )
					{
						if (world.objectExists(object_key))
						{
							AHGameObject& game_obj( dynamic_cast< AHGameObject& >( world.object(object_key) ) );
							game_obj.takeDamage( model, m_source_obj, splash.type, splash.amount, splash.penetration );
						}
					}
				}
			}
	}
}






void adjustWeaponStatsForPerks( WeaponData& stats, const PlayerCharacter& player )
{
	using namespace player_perks;

	bool is_single_shot_weapon( (1 == stats.num_shots) && (1 == stats.shots_per_round) );

	if (is_single_shot_weapon)
		if (player.hasPerk(SingleShotAccuracy))
		{
			stats.shot_accuracy *= 2;
			stats.shot_damage_falloff += 3;
			stats.shot_accuracy_falloff += 3;
		}

	if ((stats.num_shots > 1) && (projectile::Bullet == stats.shot_type))
		if (player.hasPerk(MultiShotAccuracy2))
		{
			stats.shot_accuracy *= 2;
			stats.shot_damage_falloff += 3;
			stats.shot_accuracy_falloff += 3;
		}
		else if (player.hasPerk(MultiShotAccuracy1))
		{
			stats.shot_accuracy += 3;
			stats.shot_damage_falloff += 1;
			stats.shot_accuracy_falloff += 1;
		}

	if (player.hasPerk(FlameDamage) && (projectile::Flame == stats.shot_type))
	{
		stats.shot_damage_amount *= 2;
	}
}




bool fireShot( 
	AHGameModel& model, const WorldObject::WorldLocation& from, 
	int to_offs_x, int to_offs_z, 
	const Weapon& weapon, PlayerCharacter& attacker, 
	bool is_first_shot
	)
{
	if ((0 == to_offs_x) && (0 == to_offs_z))
		return false;

	int orig_targ_x = from.x + to_offs_x;
	int orig_targ_z = from.z + to_offs_z;

	World& world( model.world() );
	ASSERT( WorldObject::WorldLocation::INVALID_ZONE != from.zone );
	Zone& zone( world.zone(from.zone) );

	bool direct_select_target( false );
	BOOST_FOREACH( DBKeyValue obj_key, zone.objectsAt(orig_targ_x, orig_targ_z) )
	{
		if (world.objectExists(obj_key))
		{
			const AHGameObject& obj( dynamic_cast<const AHGameObject&>( world.object(obj_key) ) );

			if ((objects::Alien == obj.type()) || (objects::isPassiveTarget((objects::type)obj.type())))
			{
				direct_select_target = true;
				break;
			}
		}
	}

	WeaponData weapon_stats( weapon.weaponData() );

	adjustWeaponStatsForPerks(weapon_stats, attacker);

	bool is_single_shot_weapon( (1 == weapon_stats.num_shots) && (1 == weapon_stats.shots_per_round) );
	bool is_spread_weapon( (projectile::Flame == weapon_stats.shot_type) || (projectile::Shot == weapon_stats.shot_type) );

	//Limit to the maximum distance
	double fdist = sqrt((double)((to_offs_x*to_offs_x) + (to_offs_z*to_offs_z)));
	double ffrac = ((double)weapon_stats.shot_range) / fdist;
	int orig_to_offs_x = to_offs_x;
	int orig_to_offs_z = to_offs_z;
	to_offs_x = (int)(ffrac * ((double)to_offs_x));
	to_offs_z = (int)(ffrac * ((double)to_offs_z));

	int spread = 0;
	if (projectile::Flame == weapon_stats.shot_type)
		spread = 1;
	else if (projectile::Shot == weapon_stats.shot_type)
	{
		int dist = (int)fdist;
		spread = (weapon_stats.shot_range/3);
	}
	else
	{	//Work out if the shot goes 'wild'
		//Decide first what they're actually aiming at - we use the distance to that object so that there is no advantage to always selecting a nearer tile as a target
		int targ_x = from.x + to_offs_x;
		int targ_z = from.z + to_offs_z;
		GetFirstTarget obj_tester( world );
		LoSSampler hit_test(zone, &obj_tester, targ_x, targ_z);
		lineCast( from.x, from.z, targ_x, targ_z, hit_test, VisitCellFunctor() );

		if (world.objectExists( obj_tester.foundAlien() ))
		{
			const Alien& alien( dynamic_cast<const Alien&>(world.object(obj_tester.foundAlien())) );
			int hit_x = hit_test.finish_x;
			int hit_z = hit_test.finish_z;
			int hit_offs_x = hit_x - from.x;
			int hit_offs_z = hit_z - from.z;
			int dist = (int)(isqrt( (hit_offs_x*hit_offs_x) + (hit_offs_z*hit_offs_z) ));

			int accuracy = adjustAccuracyForAlien(weapon_stats.shot_accuracy, weapon_stats.shot_accuracy_falloff, dist, alien, is_single_shot_weapon);
			if (!accuracyTest(accuracy, is_spread_weapon))
				if (rand() & 1)
					spread = (weapon_stats.shot_range/(is_single_shot_weapon ? 8 : 4));
		}
		else
		{
			spread = (0 == (rand() & 7));
		}
	}

	if (spread > 0)
	{
		to_offs_x = to_offs_x - spread + (rand() % (1+(2*spread)));
		to_offs_z = to_offs_z - spread + (rand() % (1+(2*spread)));
		double fdist = sqrt((double)((to_offs_x*to_offs_x) + (to_offs_z*to_offs_z)));
		double ffrac = ((double)weapon_stats.shot_range) / fdist;
		to_offs_x = (int)(ffrac * ((double)to_offs_x));
		to_offs_z = (int)(ffrac * ((double)to_offs_z));
	}

	int targ_x = from.x + to_offs_x;
	int targ_z = from.z + to_offs_z;

	HitTestObject obj_tester( model, world, 
		weapon_stats.shot_accuracy, weapon_stats.shot_accuracy_falloff, is_single_shot_weapon, is_spread_weapon, 
		from.x, from.z, orig_targ_x, orig_targ_z );
	LoSSampler hit_test(zone, &obj_tester, targ_x, targ_z);
	lineCast( from.x, from.z, targ_x, targ_z, hit_test, VisitCellFunctor() );

	int hit_x = hit_test.finish_x;
	int hit_z = hit_test.finish_z;
	int hit_offs_x = hit_x - from.x;
	int hit_offs_z = hit_z - from.z;

	int dist = (int)(isqrt( (hit_offs_x*hit_offs_x) + (hit_offs_z*hit_offs_z) ));
	int shot_time = (int)(dist*((double)weapon_stats.shot_speed));
	int penetration = weapon_stats.shot_penetration;
	int critical_chance = 15;
	if (is_single_shot_weapon && ((projectile::Bullet == weapon_stats.shot_type) || (projectile::Laser == weapon_stats.shot_type)))
		if (attacker.hasPerk(player_perks::SingleShotCriticalBonus2))
			critical_chance = 1;
		else if (attacker.hasPerk(player_perks::SingleShotCriticalBonus1))
			critical_chance = 3;
	int damage_base = weapon_stats.shot_damage_amount;
	if (1 == (rand() & critical_chance))
	{
		damage_base *= 2;
		penetration += 2;
	}
	int damage = damage_base;
	int adjusted_dist = dist;

	impact::Type impact_type = weapon_stats.impact_type;

	if (INVALID_KEY != hit_test.hit_obj)
	{
		AHGameObject& hit_obj( dynamic_cast< AHGameObject& >( world.object( hit_test.hit_obj ) ) );

		if (objects::Alien == hit_obj.type())
		{
			const Alien& alien( dynamic_cast<const Alien&>(hit_obj) );
			int adj = adjustAccuracyForAlien(
				weapon_stats.shot_accuracy, weapon_stats.shot_accuracy_falloff, dist, alien, is_single_shot_weapon);
			adjusted_dist += (9 - adj);

			//if (model.gameEventsObserver())
			//	model.gameEventsObserver()->debugMessage("[adj:" + boost::lexical_cast<std::string>(adj) + "]");
		}

		if (((projectile::Bullet == weapon_stats.shot_type) || ((projectile::Shot == weapon_stats.shot_type)))
			&& (objects::Alien == hit_obj.type()))
		{
			impact_type = impact::AlienBlood;
		}
	}

	if (adjusted_dist >= weapon_stats.shot_damage_falloff)
		damage -= (rand() % (1+(adjusted_dist/weapon_stats.shot_damage_falloff)));
	damage = (std::max)(0, damage);

	//if (model.gameEventsObserver())
	//{
	//	std::string msg("(");
	//	msg += "dist:" + boost::lexical_cast<std::string>(dist);
	//	msg += ",adj_dist:" + boost::lexical_cast<std::string>(adjusted_dist);
	//	msg += ",sdf:" + boost::lexical_cast<std::string>(weapon_stats.shot_damage_falloff);
	//	msg += ",acc:" + boost::lexical_cast<std::string>(weapon_stats.shot_accuracy);
	//	msg += ",saf:" + boost::lexical_cast<std::string>(weapon_stats.shot_accuracy_falloff);
	//	msg += ",dmg:" + boost::lexical_cast<std::string>(damage);
	//	msg += ")";
	//	model.gameEventsObserver()->debugMessage(msg);
	//}

	if (projectile::Laser == weapon_stats.shot_type)
	{
		if (INVALID_KEY != hit_test.hit_obj)
		{
			AHGameObject& hit_obj( dynamic_cast< AHGameObject& >( world.object( hit_test.hit_obj ) ) );
			hit_obj.takeDamage( model, attacker.key(), weapon_stats.shot_damage_type, damage, penetration );
		}

		shared_ptr< LaserTrail > trail( new LaserTrail( model, hit_offs_x, hit_offs_z, weapon_stats.fire_time ) );
		DBKeyValue trail_key( world.addWorldObject( trail ) );
		trail->moveTo(model, from, true);
	}
	else 
	{
		WorldObject::WorldLocation hit_loc( from.zone, hit_x, hit_z );

		shared_ptr<IExecuteImpactAction> impact_action
					( new DoWeaponDamage( model, hit_test.hit_obj, attacker.key(), hit_loc, 
						weapon_stats.shot_damage_type, weapon.weaponType(), 
							damage, penetration ) );

		shared_ptr< Projectile > projectile( 
			new Projectile( model, weapon_stats.shot_type, from.zone, 
				from.x, from.z, hit_x, hit_z, shot_time, 0, 
				impact_type, impact_action )
			);

		DBKeyValue proj_key = world.addWorldObject( projectile );
		projectile->moveTo(model, from, true);
	}

	return true;
}

void WeaponAttackAction::advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr< IGameEvents > events( model.gameEventsObserver() );

	m_time_remaining -= t;
	if (m_time_remaining <= 0)
		m_time_remaining = 0;

	if (m_num_shots > 0)
	{
		m_shot_timer -= t;
		if (m_shot_timer <= 0)
		{
			int num_shots(0);
			while ((m_shot_timer <= 0) && (num_shots < m_num_shots))
			{
				m_shot_timer += m_shot_interval;
				++num_shots;
			}

			num_shots = (std::min)(m_num_shots, num_shots);
			m_num_shots -= num_shots;

			World& world( model.world() );
			boost::shared_ptr< PlayerCharacter > player_obj( m_player.lock() );
			boost::shared_ptr< Weapon > weapon_obj( m_weapon.lock() );

			if (player_obj && weapon_obj)
			{
				WorldObject::WorldLocation loc( player_obj->location() );

				if ((INVALID_KEY != m_target_obj) && (world.objectExists(m_target_obj)))
				{
					const WorldObject::WorldLocation& targ_loc( world.object(m_target_obj).location() );
					if (targ_loc.zone == loc.zone)
					{
						m_target_x = targ_loc.x - loc.x;
						m_target_z = targ_loc.z - loc.z;
					}
				}

				
				int shots_fired = 0;

				while( num_shots > 0 )
				{
					--num_shots;

					//weapon::Type weapon_type( weapon_obj->weaponType() );
					const WeaponData& weapon_stats( weapon_obj->weaponData() );

					if (0 >= weapon_obj->getAmmo())
					{
						if (events)
							events->playerFiredRoundFailed(model, *player_obj, *weapon_obj);
					}
					else 
					{
						bool fired(false);

						for (int shot=0; shot < weapon_stats.shots_per_round; ++shot)
						{
							fired |= fireShot(model, loc, m_target_x, m_target_z, *weapon_obj, *player_obj, m_first_shot);
						}

						if (fired)
						{
							++shots_fired;

							bool ammo_save( false );
							if (player_obj->hasPerk(player_perks::FlameBoost))
								if (pickup::Ammo_flame == weapon_stats.ammo_type)
									if (rand()&1)
										ammo_save = true;

							if (!ammo_save)
								weapon_obj->decrementAmmo(1);

							if (events)
								events->playerFiredRound(model, *player_obj, *weapon_obj);

							boost::shared_ptr< PlayerStats > stats( player_obj->stats() );
							if (stats)
							{
								++(stats->pickupsUsed( weapon::getAmmoType( weapon_obj->weaponType() ) ));
							}

							if (m_first_shot)
								soundAlertAliens(model, loc.zone, loc.x, loc.z, weapon_stats.shot_noise);

							m_first_shot = false;
						}
					}
				}
			}
		}
	}
}


}
