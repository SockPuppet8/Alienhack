#include "BreakDoorAction.hpp"
#include "../AHGameModel.hpp"
#include "../Objects/TerrainType.hpp"
#include "../Objects/Alien.hpp"
#include "../../Model/Objects/Ping.hpp"
#include "World-2DTiles/Zone.hpp"
#include "../IGameEvents.hpp"



namespace AlienHack
{


using namespace boost;
using namespace RL_shared;


void BreakDoorAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	shared_ptr<AHGameObject> obj( m_object.lock() );
	if ((!obj) || (obj->removeMe(model)))
	{
		m_time_remaining = 0;
	}
	else
	{
		shared_ptr< Alien > alien_obj( dynamic_pointer_cast<Alien>(obj) );
		if (alien_obj && alien_obj->isStunned())
		{
			m_time_remaining = 0;
		}
		else
		{

			World& world( model.world() );
			Zone& zone( world.zone( m_target.zone ) );
			TerrainType at( zone.terrainAt(m_target.x, m_target.z) );
			if (!(terrain::isBreakable(at) && (at & terrain::Door)))
			{
				m_time_remaining = 0;
			}
			else
			{
				shared_ptr<IGameEvents> events( model.gameEventsObserver() );

				GameTimeCoordinate point1( (3*m_time_full) / 4 );
				GameTimeCoordinate point2( m_time_full / 2 );
				GameTimeCoordinate point3( m_time_full / 4 );

				bool create_noise(false);

				if ((old_time >= point1) && (m_time_remaining < point1))
				{
					create_noise = true;
					if (events)
						events->doorRammed(model, *obj, m_target);
				}

				if ((old_time >= point2) && (m_time_remaining < point2))
				{
					create_noise = true;
					if (events)
						events->doorRammed(model, *obj, m_target);
				}

				if ((old_time >= point3) && (m_time_remaining < point3))
				{
					create_noise = true;

					zone.putTerrain( at | terrain::Broken, m_target.x, m_target.z );

					model.dirtyHearingField();

					if (events)
						events->doorRammedOpen(model, *obj, m_target);
				}

				if (create_noise)
				{
					shared_ptr< Ping > ping( new Ping( ping::Noise, 1000 ) );
					world.addWorldObject( ping );
					ping->moveTo( model, m_target, true );
				}
			}

		}
	}

	if (0 == m_time_remaining)
		onActionComplete(model);
}



}
