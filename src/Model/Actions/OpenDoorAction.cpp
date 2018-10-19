#include "OpenDoorAction.hpp"
#include "../AHGameModel.hpp"
#include "../Objects/TerrainType.hpp"
//#include "../../Model/Objects/Alien.hpp"
#include "World-2DTiles/Zone.hpp"
#include "../IGameEvents.hpp"



namespace AlienHack
{



void OpenDoorAction::advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	RL_shared::GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((RL_shared::GameTimeCoordinate)0, m_time_remaining-t);
	RL_shared::GameTimeCoordinate half_time( m_time_full / 2 );
	if ((old_time >= half_time) && (m_time_remaining < half_time))
	{
		RL_shared::World& world( model.world() );
		RL_shared::Zone& zone( world.zone( m_target.zone ) );
		TerrainType at( zone.terrainAt(m_target.x, m_target.z) );
		if ((terrain::Door == terrain::getType(at)) && (!terrain::isBroken(at)))
		{
			zone.putTerrain( at | terrain::Open, m_target.x, m_target.z );

			model.dirtyHearingField();

			boost::shared_ptr<IGameEvents> events( model.gameEventsObserver() );
			if (events)
				events->doorOpened(model, m_target);
		}
	}

	if (0 == m_time_remaining)
		onActionComplete(model);
}



}
