#include "HuggerAttackAction.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"
#include "../Objects/PlayerCharacter.hpp"
#include "../Objects/Alien.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


void HuggerAttackAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );

	shared_ptr< Alien > attacker( m_attacker.lock() );
	if ((!attacker) || attacker->removeMe(model) || attacker->isStunned())
	{
		m_time_remaining = 0;
	}
	else
	{
		GameTimeCoordinate old_time( m_time_remaining );
		m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

		if ((old_time > m_commit_time) && (m_commit_time >= m_time_remaining))
		{
			shared_ptr< PlayerCharacter > attackee( m_attackee.lock() );
			if (attackee)
			{
				attackee->huggerAttack(model, *attacker);
			}
		}
	}

	if (0 == m_time_remaining)
		onActionComplete(model);
}


boost::shared_ptr< RL_shared::Actor > HuggerAttackAction::actor(void) const
{
	return m_attacker.lock();
}


}
