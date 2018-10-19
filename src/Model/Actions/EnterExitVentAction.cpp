#include "EnterExitVentAction.hpp"
#include "../AHGameModel.hpp"
#include "../Objects/TerrainType.hpp"
#include "../Objects/Alien.hpp"
#include "../IGameEvents.hpp"
#include "World-2DTiles/Zone.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


void EnterExitVentAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );

	shared_ptr< Alien > alien( m_alien.lock() );
	if (!alien)
	{
		m_time_remaining = 0;
	}
	else 
	{
		if (alien->isStunned() && m_entering)
		{
			m_time_remaining = 0;
		}
		else
		{
			GameTimeCoordinate old_time( m_time_remaining );
			m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);
			GameTimeCoordinate half_time( m_time_full / 2 );

			if ((old_time >= half_time) && (m_time_remaining < half_time))
			{
				bool success(false);

				if (m_entering)
					success = alien->enterVent(model);
				else
					success = alien->exitVent(model);

				if (success)
				{
					alienMakesSound( model, *alien, 2 );
		
					shared_ptr<IGameEvents> events( model.gameEventsObserver() );
					if (events)
					{
						WorldObject::WorldLocation alien_loc( alien->location() );
						if (model.isVisible(alien_loc.zone, alien_loc.x, alien_loc.z))
						{
							World& world( model.world() );
							Zone& zone( world.zone( alien_loc.zone ) );
							TerrainType at( zone.terrainAt(alien_loc.x, alien_loc.z) );

							if (m_entering)
								events->alienEnterVent(model, *alien, at);
							else
								events->alienExitVent(model, *alien, at);
						}
					}
				}
			}
		}
	}

	if (0 == m_time_remaining)
		onActionComplete(model);
}


boost::shared_ptr< RL_shared::Actor > EnterExitVentAction::actor(void) const
{
	return m_alien.lock();
}


}
