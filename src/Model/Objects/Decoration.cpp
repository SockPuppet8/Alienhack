#include "Decoration.hpp"
#include "TerrainType.hpp"
#include "ObjectType.hpp"



namespace AlienHack
{


Decoration::Decoration( decoration::Type type )
: AHGameObject(objects::Decoration), m_decor_type(type), m_disappear(false)
{
}

//Constructor for serialization use.
Decoration::Decoration( void )
: AHGameObject(objects::Decoration), m_decor_type(decoration::None), m_disappear(false)
{
}

bool Decoration::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Decoration::canOverlap(TerrainType type) const
{
	//if (terrain::Wall == type)
	//	return false;

	//if (terrain::Door == (type & terrain::Types))
	//	if (0 == (type & terrain::Open))
	//		return false;

	return terrain::isPassable(type);
}

void Decoration::notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate )
{
}
bool Decoration::removeMe(AHGameModel&) const
{
	return m_disappear;
}
bool Decoration::shouldDraw(void) const
{
	return true;
}

bool Decoration::blocksDoor(void) const
{
	return false;
}

void Decoration::disappear(void)
{
	m_disappear = true;
}

std::string Decoration::getThereIsString(bool) const
{
	return "";
}

std::string Decoration::getSelectName(bool) const
{
	using namespace decoration;
	switch( decorationType() )
	{
		case Corpse:
			return "Corpse";
		case AlienCorpse:
			return "Alien Corpse";
		case Blood:
			return "Blood";
		default:;
	}
	return "Unknown";
}



}
