#include "DirectAttackAction.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/IGameEvents.hpp"
#include "../../Model/Objects/Alien.hpp"


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


void DirectAttackAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	shared_ptr< AHGameObject > attacker( m_attacker.lock() );
	if ((!attacker) || attacker->removeMe(model))
	{
		m_time_remaining = 0;
	}
	else
	{
		shared_ptr< Alien > alien( dynamic_pointer_cast< Alien >(attacker) );
		if (alien && alien->isStunned())
		{
			m_time_remaining = 0;
		}
		else
		{
			GameTimeCoordinate old_time( m_time_remaining );
			m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

			if ((old_time > m_commit_time) && (m_commit_time >= m_time_remaining))
			{
				shared_ptr< AHGameObject > attackee( m_attackee.lock() );
				if (attackee && attacker && (!attacker->removeMe(model)))
				{
					if (game_events)
					{
						game_events->objectAttacksAnother(model, *attacker, *attackee, m_dmg_type);
					}

					attackee->takeDamage(model, attacker->key(), m_dmg_type, m_dmg_amount, m_dmg_penetration);
				}
			}
		}
	}

	if (0 == m_time_remaining)
		onActionComplete(model);
}


boost::shared_ptr< RL_shared::Actor > DirectAttackAction::actor(void) const
{
	return m_attacker.lock();
}


}
