#include "Projectile.hpp"
#include "ObjectType.hpp"
#include "../AHGameModel.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


const GameTimeCoordinate IMPACT_TIME = 125;


Projectile::Projectile(
	AHGameModel& model, 
	projectile::Type type, DBKeyValue zone, 
	int start_x, int start_z, 
	int finish_x, int finish_z, 
	RL_shared::GameTimeCoordinate time, 
	RL_shared::GameTimeCoordinate delay, 
	impact::Type impact, 
	shared_ptr< IExecuteImpactAction > action
	)
	: AHGameObject( objects::Projectile )
	, m_action( action )
	, m_projectile_type( type )
	, m_impact_type( impact )
	, m_zone( zone )
	, m_start_x( start_x )
	, m_start_z( start_z )
	, m_finish_x( finish_x )
	, m_finish_z( finish_z )
	, m_time_full( time )
	, m_time_remaining( time )
	, m_delay( delay )
{
	model.incFastMovingObjects();
}

//Constructor for serialization use
Projectile::Projectile(void)
	: AHGameObject( objects::Projectile )
	, m_projectile_type( projectile::None )
	, m_impact_type( impact::None )
	, m_zone( RL_shared::INVALID_KEY )
	, m_start_x( 0 )
	, m_start_z( 0 )
	, m_finish_x( 0 )
	, m_finish_z( 0 )
	, m_time_full( 0 )
	, m_time_remaining( 0 )
	, m_delay( 0 )
{
}

bool Projectile::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Projectile::canOverlap(TerrainType) const
{
	return true;
}

void Projectile::notifyTimeStep( AHGameModel& model, GameTimeCoordinate dt )
{
	if (m_delay > dt)
	{
		m_delay -= dt;
		return;
	}

	m_time_remaining -= (dt-m_delay);
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining);
	m_delay = 0;

	if (projectile::Impact != m_projectile_type)
	{
		if (m_time_remaining > 0)
		{
			double frac = ((double)m_time_remaining) / ((double)m_time_full);
			double fpos_x = 0.5 + (((double)m_start_x) * frac) + (((double)m_finish_x) * (1.0-frac)); //+0.5 to start and finish in centre of tile
			double fpos_z = 0.5 + (((double)m_start_z) * frac) + (((double)m_finish_z) * (1.0-frac)); //ditto
			int pos_x = (int)floor(fpos_x);
			int pos_z = (int)floor(fpos_z);
			if (!moveTo( model, WorldLocation( m_zone, pos_x, pos_z ), false ))
				m_time_remaining = 0;
		}
		else
		{
			if (moveTo( model, WorldLocation( m_zone, m_finish_x, m_finish_z ), false ))
			{
				if (impact::None != m_impact_type)
				{
					m_projectile_type = projectile::Impact;
					m_time_remaining = IMPACT_TIME;
				}
			}
			if (m_action)
			{
				m_action->execute();
				m_action.reset();
			}
		}
	}
}

bool Projectile::removeMe(AHGameModel&) const
{
	return m_time_remaining <= 0;
}

bool Projectile::shouldDraw(void) const
{
	return (m_delay <= 0) && (m_time_remaining > 0) && (projectile::Shot != projectileType());
}

std::string Projectile::getThereIsString(bool) const
{
	return "";
}

std::string Projectile::getSelectName(bool) const
{
	return "";
}


}
