#include "LoSSampler.hpp"
#include "Objects/TerrainType.hpp"
#include "Objects/AHGameObject.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include <boost/foreach.hpp>



namespace AlienHack
{


using namespace RL_shared;


bool LoSSampler::operator()(int x, int y) 
{
	if (!m_zone.isWithin(x, y))
		return false;

	TerrainType there( m_zone.terrainAt(x,y) );

	if (!terrain::isPassable(there))
		hit = true;

	if (m_object_tester)
	{
		const ObjectList& objects( m_zone.objectsAt(x,y) );
		BOOST_FOREACH( DBKeyValue obj, objects )
		{
			if (m_object_tester->isBlocking(obj))
			{
				hit = true;
				hit_obj = obj;
				break;
			}
		}
	}

	if (hit)
	{
		finish_x = x;
		finish_z = y;
		return true;
	}

	last_clear_x = x;
	last_clear_z = y;

	return false;
}



bool LineOfSightObjectTester::isBlocking( RL_shared::DBKeyValue key ) const
{
	if (m_world.objectExists(key))
	{
		const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( m_world.object(key) ) );
		return game_obj.blocksVision();
	}
	return false;
}

bool LineOfFireObjectTester::isBlocking( RL_shared::DBKeyValue key ) const
{
	if (m_world.objectExists(key))
	{
		const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( m_world.object(key) ) );
		return game_obj.blocksProjectiles();
	}
	return false;
}



}
