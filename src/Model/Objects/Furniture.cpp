#include "Furniture.hpp"
#include "TerrainType.hpp"
#include "ObjectType.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"



namespace AlienHack
{


Furniture::Furniture( furniture::Type type, bool broken )
: AHGameObject(objects::Furniture), m_furn_type(type), m_broken(broken)
{
}

//Constructor for serialization use.
Furniture::Furniture( void )
: AHGameObject(objects::Furniture), m_furn_type(furniture::None), m_broken(false)
{
}

bool Furniture::canOverlap(const RL_shared::WorldObject& object) const
{
	const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >(object) );

	return (objects::Furniture != game_obj.type()) 
		&& (objects::Barrel != game_obj.type())
		&& (objects::Egg != game_obj.type())
		;
}
bool Furniture::canOverlap(TerrainType type) const
{
	return terrain::isPassable(type);
}

void Furniture::notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate )
{
}
bool Furniture::removeMe(AHGameModel&) const
{
	return false;
}
bool Furniture::shouldDraw(void) const
{
	return true;
}

bool Furniture::blocksDoor(void) const
{
	return true;
}

bool Furniture::blocksVision(void) const
{
	return ((furniture::CrateStack == furnitureType()) && (!isBroken())) || (furniture::Machinery == furnitureType());
}
bool Furniture::blocksProjectiles(void) const
{
	return false;
}

int Furniture::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	if (!m_broken)
		if ((amount >= 1) && (type != damage::Bullet))
			if ((rand() & 7) < amount)
			{
				boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
				if (events)
				{
					events->objectDestroyed(model, *this);
				}
				model.destroyedSomething(*this);

				m_broken = true;
			}

	return amount;
}

void Furniture::acidSplash( AHGameModel& model, int amount, int )
{
	if (amount >= 2)
	{
		if (!m_broken)
		{
			boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
			{
				events->acidSplashed(model, *this);
				events->objectDestroyed(model, *this);
			}
		}
		m_broken = true;
	}
}

std::string Furniture::getThereIsString(bool) const
{
	return "";
}

std::string Furniture::getSelectName(bool) const
{
	using namespace furniture;

	if (m_broken)
	{
		switch( furnitureType() )
		{
			case Junk:
				return "Junk";
			case Machinery:
				return "Broken Machinery";
			case Piping:
				return "Damaged Pipes";
			default:;
		}
		return "Broken Furniture";
	}

	switch( furnitureType() )
	{
		case Crate:
			return "Crate";
		case CrateStack:
			return "Crate Stack";
		case Locker:
			return "Locker";
		case Table:
			return "Table";
		case Chair:
			return "Chair";
		case Desk:
			return "Desk";
		case Sofa:
			return "Sofa";
		case Junk:
			return "Junk";
		case Machinery:
			return "Machinery";
		case Piping:
			return "Pipes";
		case Bed:
			return "Bed";
		case Workbench:
			return "Workbench";
		case Hopper:
			return "Waste Hopper";
		default:;
	}
	return "Unknown";
}



}
