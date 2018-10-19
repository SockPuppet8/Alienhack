#include "SpitterAttackAction.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"
#include "../Objects/PlayerCharacter.hpp"
#include "../Objects/Alien.hpp"
#include "../LoSSampler.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


SpitterAttackAction::SpitterAttackAction( 
	shared_ptr< Alien > attacker, 
	shared_ptr< PlayerCharacter > attackee, 
	int amount, int range_squared, 
	GameTimeCoordinate time
	)
	: m_source(attacker->location()), m_attacker(attacker), m_attackee(attackee), m_amount(amount), m_range_squared(range_squared), 
	m_time_full(time), m_time_remaining(time), m_commit_time(0)
{
}


void SpitterAttackAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	GameTimeCoordinate attack_begin = (3*(m_time_full/4));
	GameTimeCoordinate attack_spit = (m_time_full/2);
	GameTimeCoordinate attack_hit = (3*(m_time_full/8));

	bool attack_launched = ( old_time > attack_spit) && (attack_spit >= m_time_remaining);
	bool attack_landed = (old_time > attack_hit) && (attack_hit >= m_time_remaining);

	shared_ptr< Alien > attacker( m_attacker.lock() );
	if ((!attack_launched) && ((!attacker) || attacker->removeMe(model) || attacker->isStunned()))
	{
		m_time_remaining = 0;
	}
	else
	{
		shared_ptr< PlayerCharacter > attackee( m_attackee.lock() );
		if (attackee)
		{
			shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
			{
				if ((old_time > attack_begin) && (attack_begin >= m_time_remaining))
				{
					events->spitterAttackBegin(model, *attacker);
				}
				if (attack_launched)
				{
					events->spitterAttackSpit(model, *attacker);
				}
			}

			if (attack_landed)
			{
				WorldObject::WorldLocation my_loc( m_source );
				WorldObject::WorldLocation your_loc( attackee->location() );

				int dx = my_loc.x - your_loc.x;
				int dz = my_loc.z - your_loc.z;
				int ds = (dx*dx) + (dz*dz);

				if ((my_loc.zone == your_loc.zone) && (ds <= m_range_squared))
				{
					//bool has_los( false );

					//if (model.isVisible(my_loc.zone, my_loc.x, my_loc.z))
					//{ //player can see me...
					//	has_los = true;
					//}
					//else if (model.world().zoneExists( my_loc.zone ))
					//{
					//	const Zone& zone( model.world().zone( my_loc.zone ) );
					//	LineOfFireObjectTester obj_tester( model.world() );
					//	LoSSampler los_test( zone, &obj_tester, your_loc.x, your_loc.z );
					//	lineCast(my_loc.x, my_loc.z, your_loc.x, your_loc.z, los_test, VisitCellFunctor());
					//	has_los = !los_test.hit;
					//}

					//if (has_los)

					if (model.avatarIsVisibleFrom(my_loc.zone, my_loc.x, my_loc.z))
					{
						attackee->acidSplash(model, m_amount, 3*m_amount);
					}
				}
			}
		}
	}

	if (0 == m_time_remaining)
		onActionComplete(model);
}


boost::shared_ptr< RL_shared::Actor > SpitterAttackAction::actor(void) const
{
	return m_attacker.lock();
}


}
