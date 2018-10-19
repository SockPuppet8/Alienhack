#include "PickupAction.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/IGameEvents.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


void PickupAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	if (!m_committed)
	{
		if ((old_time > 0) && (0 >= m_time_remaining))
		{
			shared_ptr< PlayerCharacter > player( m_player.lock() );
			shared_ptr< Pickup > object( m_object.lock() );
			if (player && object)
			{
				m_committed = true;

				if (player->pickUp(*object))
				{
					if (game_events)
						game_events->playerPickedUp(model, *player, *object);
				}
				else
					if (game_events)
						game_events->playerCouldNotPickUp(model, *player, *object);
			}
		}
	}
}

bool PickupAction::interrupt( AGameModel& in_model )
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

boost::shared_ptr< RL_shared::Actor > PickupAction::actor(void) const
{
	return m_player.lock();
}


}
