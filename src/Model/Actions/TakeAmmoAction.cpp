#include "TakeAmmoAction.hpp"
#include "../Objects/Weapon.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"
#include "assert.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


void TakeAmmoAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	World& world( model.world() );

	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	if (!m_committed)
	{
		GameTimeCoordinate commit_time( m_time_full/2 );
		if ((old_time > commit_time) && (commit_time >= m_time_remaining))
		{
			shared_ptr< PlayerCharacter > player( m_player.lock() );
			shared_ptr< Weapon > object( m_object.lock() );
			if (object && player)
			{
				player->takeAmmo(model, *object, true);
				m_committed = true;
			}
		}
	}
}

bool TakeAmmoAction::interrupt( AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );

	if (!m_committed)
	{
		m_committed = true;
		m_time_remaining = 0;

		shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );
		if (game_events)
			game_events->playerActionInterrupted(model);

		return true;
	}

	return false;
}

boost::shared_ptr< RL_shared::Actor > TakeAmmoAction::actor(void) const
{
	return m_player.lock();
}


}
