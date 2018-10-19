#include "Barrel.hpp"
#include "TerrainType.hpp"
#include "ObjectType.hpp"
#include "Explosion.hpp"
#include "../IGameEvents.hpp"
#include "../AHGameModel.hpp"



namespace AlienHack
{


Barrel::Barrel(void)
: AHGameObject(objects::Barrel), m_disappear(false)
{
}

bool Barrel::canOverlap(const RL_shared::WorldObject& other) const
{
	const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >(other) );

	if (objects::Alien == game_obj.type())
		return true;

	return game_obj.type() >= objects::First_Transient;
}
bool Barrel::canOverlap(TerrainType type) const
{
	//if (terrain::Wall == type)
	//	return false;

	//if (terrain::Door == (type & terrain::Types))
	//	if (0 == (type & terrain::Open))
	//		return false;

	return terrain::isPassable(type);
}

void Barrel::notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate )
{
}
bool Barrel::removeMe(AHGameModel&) const
{
	return m_disappear;
}
bool Barrel::shouldDraw(void) const
{
	return true;
}
bool Barrel::blocksDoor(void) const
{
	return true;
}

void Barrel::acidSplash( AHGameModel& model, int /*amount*/, int )
{
	if (!m_disappear)
	{
		//How should barrels react to acid?
		//Would be nice if this could cause a pool of fuel which would then ignite if provoked. 
		//Too complicated though, and the player would want to be able to deliberately create spills. 
		//if ((rand() % 6) < amount)
		//{
		//	m_disappear = true;
		//	boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
		//	if (events)
		//	{
		//		events->acidSplashed(model, *this);
		//		events->objectDestroyed(model, *this);
		//	}
		//}
	}
}

int Barrel::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	if (!m_disappear)
	{
		if (   (damage::Explosion == type) || (damage::Fire == type) || (damage::Bullet == type) 
			|| (damage::Lance == type) || (damage::Laser == type)
			)
		{
			m_disappear = true;
			boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
				events->objectDestroyed(model, *this);

			explosion::makeExplosion( model, RL_shared::DBKeyValue(), location(), explosion::BarrelBlast );

			return amount;
		}
	}
	return 0;
}

std::string Barrel::getThereIsString(bool) const
{
	return "";
}

std::string Barrel::getSelectName(bool) const
{
	return "Fuel Barrel";
}


}
