#include "ReloadWeaponAction.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"
#include "../Objects/PlayerCharacter.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


namespace
{
	GameTimeCoordinate getCommitTime( GameTimeCoordinate full_time )
	{
		return full_time/4;
	}
}


void ReloadWeaponAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	RL_shared::GameTimeCoordinate commit_time( getCommitTime(m_time_full) );
	if ((old_time >= commit_time) && (m_time_remaining < commit_time))
	{
		shared_ptr< PlayerCharacter > player( m_player.lock() );
		if (player)
		{
			player->reloadWeapon(model);
		}
	}
}


bool ReloadWeaponAction::interrupt( AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );

	if (m_time_remaining > getCommitTime(m_time_full))
	{
		m_time_remaining = 0;

		shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );
		if (game_events)
			game_events->playerActionInterrupted(model);

		return true;
	}

	return false;
}


boost::shared_ptr< RL_shared::Actor > ReloadWeaponAction::actor(void) const
{
	return m_player.lock();
}


}
