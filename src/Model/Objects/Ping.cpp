#include "Ping.hpp"
#include "ObjectType.hpp"
#include "../AHGameModel.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;




Ping::Ping( ping::Type type, RL_shared::GameTimeCoordinate time )
	: AHGameObject( objects::Ping )
	, m_time_full( time )
	, m_time_remaining( time )
	, m_ping_type( type )
{
}

//Constructor for serialization use
Ping::Ping( void )
	: AHGameObject( objects::Ping )
	, m_time_full( 0 )
	, m_time_remaining( 0 )
	, m_ping_type( ping::None )
{
}

bool Ping::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Ping::canOverlap(TerrainType) const
{
	return true;
}

void Ping::notifyTimeStep( AHGameModel& model, GameTimeCoordinate dt )
{
	m_time_remaining -= dt;
	if (m_time_remaining <= 0)
		m_time_remaining = 0;
}

bool Ping::removeMe(AHGameModel&) const
{
	return m_time_remaining <= 0;
}

bool Ping::shouldDraw(void) const
{
	return (m_time_remaining > 0);
}

std::string Ping::getThereIsString(bool) const
{
	return "";
}

std::string Ping::getSelectName(bool) const
{
	switch (m_ping_type)
	{
		case ping::Motion:
			return "Motion Trace";
		case ping::Noise:
			return "Noise";
		case ping::Danger:
			return "Danger";
		default:;
	}

	return "Unknown";
}



}
