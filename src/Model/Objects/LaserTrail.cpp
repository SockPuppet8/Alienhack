#include "LaserTrail.hpp"
#include "ObjectType.hpp"
#include "../AHGameModel.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



LaserTrail::LaserTrail(
	AHGameModel& model, 
	int target_offs_x, int target_offs_z, 
	GameTimeCoordinate time
	)
	: AHGameObject( objects::LaserTrail )
	, m_target_offs_x( target_offs_x )
	, m_target_offs_z( target_offs_z )
	, m_time_full( time )
	, m_time_remaining( time )
{
	model.incFastMovingObjects();
}

//Constructor for serialization use.
LaserTrail::LaserTrail(void)
	: AHGameObject( objects::LaserTrail )
	, m_target_offs_x( 0 )
	, m_target_offs_z( 0 )
	, m_time_full( 0 )
	, m_time_remaining( 0 )
{
}

bool LaserTrail::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool LaserTrail::canOverlap(TerrainType) const
{
	return true;
}

void LaserTrail::notifyTimeStep( AHGameModel& model, GameTimeCoordinate dt )
{
	m_time_remaining -= dt;

	if (m_time_remaining <= 0)
		m_time_remaining = 0;
}

bool LaserTrail::removeMe(AHGameModel&) const
{
	return m_time_remaining <= 0;
}

bool LaserTrail::shouldDraw(void) const
{
	return (m_time_remaining > 0);
}

std::string LaserTrail::getThereIsString(bool) const
{
	return "";
}

std::string LaserTrail::getSelectName(bool) const
{
	return "";
}


}
