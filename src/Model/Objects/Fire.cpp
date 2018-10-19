#include "Fire.hpp"
#include "ObjectType.hpp"
#include "../AHGameModel.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



namespace
{
	const int FIRE_DAMAGE_INTERVAL = 100;
}



Fire::Fire( 
	RL_shared::GameTimeCoordinate time, 
	RL_shared::GameTimeCoordinate smoke_time, 
	int damage 
	)
	: AHGameObject( objects::Fire )
	, m_time_full( time )
	, m_time_remaining( time )
	, m_smoke_time( smoke_time )
	, m_damage( damage )
{
}

//Constructor for serialization use
Fire::Fire( void )
	: AHGameObject( objects::Fire )
	, m_time_full( 0 )
	, m_time_remaining( 0 )
	, m_smoke_time( 0 )
	, m_damage( 0 )
{
}

bool Fire::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Fire::canOverlap(TerrainType) const
{
	return true;
}

void Fire::notifyTimeStep( AHGameModel& model, GameTimeCoordinate dt )
{
	GameTimeCoordinate old_time = m_time_remaining;

	m_time_remaining -= dt;
	if (m_time_remaining <= 0)
		m_time_remaining = 0;

	if ((m_time_remaining > m_smoke_time) 
		&& ((int)(old_time/FIRE_DAMAGE_INTERVAL) != (int)(m_time_remaining/FIRE_DAMAGE_INTERVAL)))
	{
		World& world( model.world() );
		WorldLocation my_loc( location() );
		if (world.zoneExists(my_loc.zone))
		{
			Zone& zone( world.zone( my_loc.zone ) );
			if (zone.isWithin(my_loc.x, my_loc.z))
			{
				const ObjectList& objects( zone.objectsAt( my_loc.x, my_loc.z ) );
				BOOST_FOREACH( DBKeyValue obj_key, objects )
				{
					if (world.objectExists(obj_key) && (obj_key != key()))
					{
						AHGameObject& game_obj( dynamic_cast< AHGameObject& >( world.object(obj_key) ) );
						game_obj.takeDamage(model, RL_shared::INVALID_KEY, damage::Fire, m_damage, 0);
					}
				}
			}
		}
	}
}

bool Fire::removeMe(AHGameModel&) const
{
	return m_time_remaining <= 0;
}

bool Fire::shouldDraw(void) const
{
	return (m_time_remaining > 0);
}

bool Fire::blocksVision(void) const
{
	return (rand() & 1);
}

std::string Fire::getThereIsString(bool) const
{
	return isSmoke() ? "thick, acrid smoke" : "a hellish inferno";
}

std::string Fire::getSelectName(bool) const
{
	if (isSmoke())
		return "Smoke";
	return "Fire";
}


}
