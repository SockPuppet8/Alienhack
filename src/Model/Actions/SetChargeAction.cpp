#include "SetChargeAction.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/IGameEvents.hpp"
#include "../../Model/Objects/Explosive.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "World-2DTiles/World.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


namespace
{
	const int FUSE_TIME = 5000;
}


void SetChargeAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	if (!m_committed)
	{
		if ((old_time > 0) && (0 >= m_time_remaining))
		{
			m_committed = true;

			shared_ptr< PlayerCharacter > player( m_player.lock() );
			if (player && player->canUseItem(pickup::DemoCharge))
			{
				shared_ptr<Explosive> charge( new Explosive(FUSE_TIME, m_dir_x, m_dir_z) );
				model.world().addWorldObject( charge );
				if (charge->moveTo(model, m_loc, true))
				{
					player->dec(pickup::DemoCharge);

					shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );
					if (game_events)
						game_events->placedDemoCharge(model, FUSE_TIME/1000);
				}
				else
				{
					model.world().removeWorldObject( charge->key() );
					shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );
					if (game_events)
						game_events->failedPlacingDemoCharge(model);
				}
			}
		}
	}
}

bool SetChargeAction::interrupt( AGameModel& in_model )
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

boost::shared_ptr< RL_shared::Actor > SetChargeAction::actor(void) const
{
	return m_player.lock();
}


}
