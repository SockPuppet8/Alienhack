#include "UseTerminalAction.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/IGameEvents.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


namespace
{
	const int COMMIT_POINT = 0;
}


void UseTerminalAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	if (!m_committed)
	{
		if ((old_time > COMMIT_POINT) && (COMMIT_POINT >= m_time_remaining))
		{
			m_committed = true;
			//if (terminal::Info == m_object)
			{
				model.mapCurrentZone();
				if (game_events)
					game_events->downloadedMap(model);
			}
			if (terminal::Master == m_object)
			{
				model.notifyUsedMasterTerminal();
			}
		}
	}
}

bool UseTerminalAction::interrupt( AGameModel& in_model )
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

boost::shared_ptr< RL_shared::Actor > UseTerminalAction::actor(void) const
{
	return m_player.lock();
}


}
