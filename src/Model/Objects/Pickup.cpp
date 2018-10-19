#include "Pickup.hpp"
#include "TerrainType.hpp"
#include "ObjectType.hpp"
#include "Explosion.hpp"
#include "../IGameEvents.hpp"
#include "../AHGameModel.hpp"



namespace AlienHack
{


Pickup::Pickup( pickup::Type type )
: AHGameObject(objects::Pickup), m_pickup_type(type), m_disappear(false)
{
}

//Constructor for serialization use.
Pickup::Pickup( void )
: AHGameObject(objects::Pickup), m_pickup_type(pickup::None), m_disappear(false)
{
}

bool Pickup::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Pickup::canOverlap(TerrainType type) const
{
	//if (terrain::Wall == type)
	//	return false;

	//if (terrain::Door == (type & terrain::Types))
	//	if (0 == (type & terrain::Open))
	//		return false;

	return terrain::isPassable(type);
}

void Pickup::notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate )
{
}
bool Pickup::removeMe(AHGameModel&) const
{
	return m_disappear;
}
bool Pickup::shouldDraw(void) const
{
	return true;
}

bool Pickup::blocksDoor(void) const
{
	return false;
}

void Pickup::disappear(void)
{
	m_disappear = true;
}

void Pickup::acidSplash( AHGameModel& model, int amount, int )
{
	if (!m_disappear)
	{
		if ((rand() % 6) < amount)
		{
			m_disappear = true;
			boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
			{
				events->acidSplashed(model, *this);
				events->objectDestroyed(model, *this);
			}
		}
	}
}

int Pickup::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	if (!m_disappear)
	{
		if ((damage::Explosion == type) || (damage::Fire == type))
		{
			m_disappear = true;
			boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
				events->objectDestroyed(model, *this);

			if (pickupType() == pickup::KrakGrenade)
				explosion::makeExplosion( model, RL_shared::DBKeyValue(), location(), explosion::KrakBlast );
			else if (pickupType() == pickup::FragGrenade)
				explosion::makeExplosion( model, RL_shared::DBKeyValue(), location(), explosion::FragBlast );
			else if (pickupType() == pickup::IncGrenade)
				explosion::makeExplosion( model, RL_shared::DBKeyValue(), location(), explosion::IncBlast );
			else if (pickupType() == pickup::StunGrenade)
				explosion::makeExplosion( model, RL_shared::DBKeyValue(), location(), explosion::StunFlash );

			return amount;
		}
	}
	return 0;
}

std::string Pickup::getThereIsString(bool) const
{
	switch( pickupType() )
	{
		case pickup::Medkit:
			return "a medkit";
		case pickup::Neutraliser: 
			return "a container of molecular neutraliser";
		case pickup::Ammo_9mm:
			return "a 9mm clip";
		case pickup::Ammo_shotgun: 
			return "12g shotgun shells";
		case pickup::Ammo_colt: 
			return "a .44 ammo clip";
		case pickup::Ammo_pulse:
			return "an M309 ammo clip";
		case pickup::Ammo_flame:
			return "a canister of flamer fuel";
		case pickup::Ammo_smartgun:
			return "an M250 clip";
		case pickup::Ammo_laser:
			return "a combat laser ammo clip";
		case pickup::Ammo_HEDP30mm:
			return "a 30mm HEDP canister drum";
		case pickup::StunGrenade:
			return "a stun grenade";
		case pickup::FragGrenade:
			return "a frag grenade";
		case pickup::IncGrenade:
			return "an incendiary grenade";
		case pickup::KrakGrenade:
			return "a krak grenade";
		case pickup::MotionTracker:
			return "a Motion Tracking Sensor Unit";
		case pickup::CreditChip:
			return "a credit chip";
		case pickup::DemoCharge:
			return "a demolition charge";
		case pickup::MegaCredChip:
			return "a mega credit chip";
		default:;
	}
	return "an unknown object";
}

std::string Pickup::getSelectName(bool) const
{
	switch( pickupType() )
	{
		case pickup::Medkit:
			return "Medkit";
		case pickup::Neutraliser: 
			return "Neutraliser";
		case pickup::Ammo_9mm:
			return "9mm clip";
		case pickup::Ammo_shotgun: 
			return "12g shells";
		case pickup::Ammo_colt: 
			return ".44 clip";
		case pickup::Ammo_pulse:
			return "M309 clip";
		case pickup::Ammo_flame:
			return "Flamer canister";
		case pickup::Ammo_smartgun:
			return "M250 clip";
		case pickup::Ammo_laser:
			return "Laser clip";
		case pickup::Ammo_HEDP30mm:
			return "30mm HEDP rounds";
		case pickup::StunGrenade:
			return "Stun grenade";
		case pickup::FragGrenade:
			return "Frag grenade";
		case pickup::IncGrenade:
			return "Inc grenade";
		case pickup::KrakGrenade:
			return "Krak grenade";
		case pickup::MotionTracker:
			return "Motion Tracker";
		case pickup::CreditChip:
			return "Cred Chip";
		case pickup::DemoCharge:
			return "Demo Charge";
		case pickup::MegaCredChip:
			return "Mega Cred Chip";
		default:;
	}
	return "Unknown";
}



}
