#include "ThrowGrenadeAction.hpp"
#include "../Objects/PlayerCharacter.hpp"
#include "../Objects/Explosion.hpp"
#include "../Objects/Fire.hpp"
#include "../Objects/Alien.hpp"
#include "../AHGameModel.hpp"
#include "../LoSSampler.hpp"
#include "World-2DTiles/Zone.hpp"
#include "Algorithm/Algorithms-2D.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


namespace
{
	const int GRENADE_SPEED = 30;
}










void GrenadeExplode::execute(void)
{
	explosion::makeExplosion(*m_model, m_thrower, m_hit_loc, 
		explosion::getExplosionType(m_grenade_type), 
		m_flame_special_1, m_flame_special_2);
}










bool throwGrenade(AHGameModel& model, PlayerCharacter& player, const WorldObject::WorldLocation& from, pickup::Type type, int to_offs_x, int to_offs_z)
{
	World& world( model.world() );
	ASSERT( WorldObject::WorldLocation::INVALID_ZONE != from.zone );
	Zone& zone( world.zone(from.zone) );
	int targ_x, targ_z, initial_dist, hit_x, hit_z, otx, otz;

	// If player throws directly at a wall, let it hit the wall. 
	// Otherwise, find a target (with applied error) which the grenade can reach without hitting any walls. 
	for (int attempts = 5; ; --attempts) {

		targ_x = otx = from.x + to_offs_x;
		targ_z = otz = from.z + to_offs_z;
		initial_dist = (int)(isqrt((to_offs_x*to_offs_x) + (to_offs_z*to_offs_z)));

		if (!player.hasPerk(player_perks::GrenadeAccuracy2) && (attempts > 0))
		{
			int decay = player.hasPerk(player_perks::GrenadeAccuracy1) ? 10 : 5;
			int error = 1 + (rand() % (1 + (initial_dist / decay)));
			int error_chance = (std::max)(2, 8 - initial_dist);
			bool tong = player.hasPerk(player_perks::GrenadeAccuracy1) ? (0 == (rand() % error_chance)) : true;
			if (tong)
			{
				targ_x += -error + (rand() % ((2 * error) + 1));
				targ_z += -error + (rand() % ((2 * error) + 1));
			}
		}

		LoSSampler hit_test(zone, 0, targ_x, targ_z);
		lineCast(from.x, from.z, targ_x, targ_z, hit_test, VisitCellFunctor());

		hit_x = hit_test.hit ? hit_test.last_clear_x : hit_test.finish_x;
		hit_z = hit_test.hit ? hit_test.last_clear_z : hit_test.finish_z;

		if ((!hit_test.hit) || (0 == attempts) || ((otx == targ_x) && (otz == targ_z)))
			break;
	}

	int hit_offs_x = hit_x - from.x;
	int hit_offs_z = hit_z - from.z;

	WorldObject::WorldLocation hit_loc( from.zone, hit_x, hit_z );

	int dist = (int)(isqrt( (hit_offs_x*hit_offs_x) + (hit_offs_z*hit_offs_z) ));
	int shot_time = (int)(dist*((double)GRENADE_SPEED));

	//int mindist =	(pickup::StunGrenade == type) ? 0 : 
	//				(explosion::getExplosionRange(explosion::getExplosionType(type))+1);
	//if (dist < mindist)
	//{
	//	//don't allow accidental instant suicide due to grenade error.
	//	return false;
	//}

	shared_ptr<IExecuteImpactAction> impact_action;
	impact_action.reset( new GrenadeExplode( model, hit_loc, type, player.key(), 
		player.hasPerk(player_perks::FlameDamage), player.hasPerk(player_perks::FlameBoost) ) );

	shared_ptr< Projectile > projectile( 
		new Projectile( model, projectile::Grenade, from.zone, 
			from.x, from.z, hit_x, hit_z, shot_time, 0, 
			impact::Puff, impact_action )
		);

	DBKeyValue proj_key = world.addWorldObject( projectile );
	projectile->moveTo( model, from, true );

	return true;
}






ThrowGrenadeAction::ThrowGrenadeAction( 
	AHGameModel& model, 
	boost::shared_ptr< PlayerCharacter > player, 
	pickup::Type type, 
	int target_offs_x, int target_offs_z, 
	RL_shared::GameTimeCoordinate time
	)
	: RL_shared::FastAction(model)
	, m_player(player)
	, m_type(type) 
	, m_target_x( target_offs_x )
	, m_target_z( target_offs_z )
	, m_time_full(time)
	, m_time_remaining(time)
	, m_commit_time( m_time_full / 2 )
{
}


void ThrowGrenadeAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	if ((old_time >= m_commit_time) && (m_time_remaining < m_commit_time))
	{
		shared_ptr< PlayerCharacter > player( m_player.lock() );
		if (player)
		{
			int attempts = 10;
			while (attempts--)
				if (throwGrenade(model, *player, player->location(), m_type, m_target_x, m_target_z))
					break;

			if (attempts > 0)
			{
				player->dec(m_type);
			}
		}
	}
}


boost::shared_ptr< RL_shared::Actor > ThrowGrenadeAction::actor(void) const
{
	return m_player.lock();
}


}
