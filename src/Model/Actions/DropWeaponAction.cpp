#include "DropWeaponAction.hpp"
#include "../AHGameModel.hpp"
//#include "../IGameEvents.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


void DropWeaponAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	RL_shared::GameTimeCoordinate half_time( m_time_full / 2 );
	if ((old_time >= half_time) && (m_time_remaining < half_time))
	{
		shared_ptr< PlayerCharacter > player( m_player.lock() );
		if (player)
		{
			player->dropWeapon(model, m_slot);
		}
	}
}


boost::shared_ptr< RL_shared::Actor > DropWeaponAction::actor(void) const
{
	return m_player.lock();
}


}
