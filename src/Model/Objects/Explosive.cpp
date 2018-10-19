#include "Explosive.hpp"
#include "ObjectType.hpp"
#include "Explosion.hpp"
#include "Ping.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"
#include "World-2DTiles/World.hpp"
//#include "World-2DTiles/Zone.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



Explosive::Explosive( 
	GameTimeCoordinate fuse_time, 
	int direction_x, int direction_z
	)
	: AHGameObject( objects::Explosive )
	, m_time_full( fuse_time )
	, m_time_remaining( fuse_time )
	, m_dir_x( direction_x )
	, m_dir_z( direction_z )
{
}

//Constructor for serialization use
Explosive::Explosive( void )
	: AHGameObject( objects::Explosive )
	, m_time_full( 0 )
	, m_time_remaining( 0 )
	, m_dir_x( 0 )
	, m_dir_z( 0 )
{
}

bool Explosive::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Explosive::canOverlap(TerrainType) const
{
	return true;
}

void Explosive::notifyTimeStep( AHGameModel& model, GameTimeCoordinate dt )
{
	World& world( model.world() );

	GameTimeCoordinate old_time = m_time_remaining;

	m_time_remaining -= dt;
	if (m_time_remaining <= 0)
		m_time_remaining = 0;

	if ((old_time > 0) && (m_time_remaining <= 0))
	{
		shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );
		if (game_events)
			game_events->demoChargeDetonates(model, *this);

		WorldLocation loc( location() );

		explosion::makeDemoExplosion(model, INVALID_KEY, loc, m_dir_x, m_dir_z);

		if (!model.isVisible(loc.zone, loc.x, loc.z))
		{
			boost::shared_ptr< Ping > ping( new Ping( ping::Noise, 1000 ) );
			world.addWorldObject( ping );
			ping->moveTo( model, loc, true );
		}
	}
}

bool Explosive::removeMe(AHGameModel&) const
{
	return m_time_remaining <= 0;
}

bool Explosive::shouldDraw(void) const
{
	return (m_time_remaining > 0);
}

int Explosive::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	if (m_time_remaining > 0)
	{
		if ((damage::Explosion == type) || (damage::Fire == type))
		{
			m_time_remaining = 0;

			boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
				events->objectDestroyed(model, *this);

			return amount;
		}
	}
	return 0;
}

std::string Explosive::getThereIsString(bool) const
{
	return "an ACTIVATED demolition charge";
}

std::string Explosive::getSelectName(bool) const
{
	return "Demo Charge (active)";
}


}
