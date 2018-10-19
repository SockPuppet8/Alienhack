#include "CloseDoorAction.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/IGameEvents.hpp"
#include "../../Model/Objects/TerrainType.hpp"
//#include "../../Model/Objects/Alien.hpp"
#include "World-2DTiles/Zone.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


void CloseDoorAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);
	GameTimeCoordinate half_time( m_time_full / 2 );
	if ((old_time >= half_time) && (m_time_remaining < half_time))
	{
		World& world( model.world() );
		Zone& zone( world.zone( m_target.zone ) );
		if (zone.isWithin(m_target.x, m_target.z))
		{
			TerrainType at( zone.terrainAt(m_target.x, m_target.z) );
			if ((terrain::Door == terrain::getType(at)) && (!terrain::isBroken(at)))
			{
				const ObjectList& objects( zone.objectsAt(m_target.x, m_target.z) );

				BOOST_FOREACH( DBKeyValue obj_key, objects )
				{
					if (world.objectExists(obj_key))
					{
						const AHGameObject& game_obj( dynamic_cast< AHGameObject& >( world.object(obj_key) ) );
						if (game_obj.blocksDoor())
						{
							shared_ptr<IGameEvents> events( model.gameEventsObserver() );
							if (events)
								events->doorBlocked(model, game_obj);
							return; //cannot close
						}
					}
				}

				zone.putTerrain( at & (~terrain::Open), m_target.x, m_target.z );

				model.dirtyHearingField();

				shared_ptr<IGameEvents> events( model.gameEventsObserver() );
				if (events)
					events->doorClosed(model, m_target);
			}
		}
	}
}


}
