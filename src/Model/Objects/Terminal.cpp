#include "Terminal.hpp"
#include "TerrainType.hpp"
#include "ObjectType.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"



namespace AlienHack
{


Terminal::Terminal( terminal::Type type, bool broken )
: AHGameObject(objects::Terminal), m_term_type(type), m_broken(broken)
{
}

//Constructor for serialization use
Terminal::Terminal( void )
: AHGameObject(objects::Terminal), m_term_type(terminal::None), m_broken(false)
{
}

bool Terminal::canOverlap(const RL_shared::WorldObject& object) const
{
	const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >(object) );

	return (objects::Furniture != game_obj.type()) 
		&& (objects::Barrel != game_obj.type())
		&& (objects::Egg != game_obj.type())
		;
}
bool Terminal::canOverlap(TerrainType type) const
{
	//if (terrain::Wall == type)
	//	return false;

	//if (terrain::Door == (type & terrain::Types))
	//	if (0 == (type & terrain::Open))
	//		return false;

	return terrain::isPassable(type);
}

void Terminal::notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate )
{
}
bool Terminal::removeMe(AHGameModel&) const
{
	return false;
}
bool Terminal::shouldDraw(void) const
{
	return true;
}

bool Terminal::blocksDoor(void) const
{
	return true;
}

int Terminal::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	if (amount >= 1)
	{
		if (!m_broken)
		{
			boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
			{
				events->objectDestroyed(model, *this);
			}
		}
		m_broken = true;
	}

	return amount;
}

void Terminal::acidSplash( AHGameModel& model, int amount, int )
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

std::string Terminal::getThereIsString(bool) const
{
	if (m_broken)
		return "";

	using namespace terminal;
	switch( terminalType() )
	{
		case Info:
			return "an information terminal";
		case Master:
			return "a master terminal";
		default:;
	}

	return "";
}

std::string Terminal::getSelectName(bool) const
{
	if (m_broken)
		return "Broken Terminal";

	using namespace terminal;
	switch( terminalType() )
	{
		case Info:
			return "Info Terminal";
		case Master:
			return "Master Terminal";
		default:;
	}
	return "Unknown";
}



}
