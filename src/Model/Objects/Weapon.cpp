#include "Weapon.hpp"
#include "TerrainType.hpp"
#include "ObjectType.hpp"
#include "WeaponData.hpp"
#include "../IGameEvents.hpp"
#include "../AHGameModel.hpp"
#include <boost/lexical_cast.hpp>



namespace AlienHack
{


namespace weapon
{
	int getClipSize( Type type, Special spec )
	{
		if (Type_None == type)
			return 0;
		return weapon_data::getWeaponData(type, spec).clip_size;
	}

	Class getClass( Type type, Special spec )
	{
		if (Type_None == type)
			return Class_None;
		return weapon_data::getWeaponData(type, spec).weapon_class;
	}

	pickup::Type getAmmoType( Type type )
	{
		if (Type_None == type)
			return pickup::None;
		return weapon_data::getWeaponData(type, Special_None).ammo_type;
	}

	int getWeight( Type type, Special spec )
	{
		if (Type_None == type)
			return 0;
		return weapon_data::getWeaponData(type, spec).weight;
	}

	const char * getSpecialString(weapon::Special spec)
	{
		switch(spec)
		{
			case Special_None: return "none";
			case Clip:		return "(+clip)";
			case Weight:	return "(-wgt)";
			case Damage:	return "(+dmg)";
			case Accuracy:	return "(+acc)";
			case Rate:		return "(+rate)";
			case Reload:	return "(-rld)";
			case Clip|Weight:		return "(+clip, -wgt)";
			case Clip|Damage:		return "(+clip, +dmg)";
			case Clip|Accuracy:		return "(+clip, +acc)";
			case Clip|Rate:			return "(+clip, +rate)";
			case Clip|Reload:		return "(+clip, -rld)";
			case Clip|Weight|Damage:		return "(+clip, -wgt, +dmg)";
			case Clip|Weight|Accuracy:		return "(+clip, -wgt, +acc)";
			case Clip|Weight|Rate:			return "(+clip, -wgt, +rate)";
			case Clip|Weight|Reload:		return "(+clip, -wgt, -rld)";
			case Clip|Weight|Damage|Accuracy:		return "(+clip, -wgt, +dmg, +acc)";
			case Clip|Weight|Damage|Rate:			return "(+clip, -wgt, +dmg, +rate)";
			case Clip|Weight|Damage|Reload:			return "(+clip, -wgt, +dmg, -rld)";
			case Clip|Weight|Damage|Accuracy|Rate:		return "(+clip, -wgt, +dmg, +acc, +rate)";
			case Clip|Weight|Damage|Accuracy|Reload:		return "(+clip, -wgt, +dmg, +acc, -rld)";
			case Clip|Weight|Damage|Accuracy|Rate|Reload:		return "(ULTIMATE!)";//return "(+clip, -wgt, +dmg, +acc, +rate, -rld)";
			case Clip|Weight|Damage|Rate|Reload:		return "(+clip, -wgt, +dmg, +rate, -rld)";
			case Clip|Weight|Accuracy|Rate:		return "(+clip, -wgt, +acc, +rate)";
			case Clip|Weight|Accuracy|Reload:		return "(+clip, -wgt, +acc, -rld)";
			case Clip|Weight|Accuracy|Rate|Reload:		return "(+clip, -wgt, +acc, +rate, -rld)";
			case Clip|Weight|Rate|Reload:		return "(+clip, -wgt, +rate, -rld)";
			case Clip|Damage|Accuracy:		return "(+clip, +dmg, +acc)";
			case Clip|Damage|Rate:			return "(+clip, +dmg, +rate)";
			case Clip|Damage|Reload:		return "(+clip, +dmg, -rld)";
			case Clip|Damage|Accuracy|Rate:		return "(+clip, +dmg, +acc, +rate)";
			case Clip|Damage|Accuracy|Reload:		return "(+clip, +dmg, +acc, -rld)";
			case Clip|Damage|Accuracy|Rate|Reload:		return "(+clip, +dmg, +acc, +rate, -rld)";
			case Clip|Damage|Rate|Reload:		return "(+clip, +dmg, +rate, -rld)";
			case Clip|Accuracy|Rate:		return "(+clip, +acc, +rate)";
			case Clip|Accuracy|Reload:		return "(+clip, +acc, -rld)";
			case Clip|Accuracy|Rate|Reload:		return "(+clip, +acc, +rate, -rld)";
			case Clip|Rate|Reload:			return "(+clip, +rate, -rld)";
			case Weight|Damage:		return "(-wgt, +dmg)";
			case Weight|Accuracy:	return "(-wgt, +acc)";
			case Weight|Rate:		return "(-wgt, +rate)";
			case Weight|Reload:		return "(-wgt, -rld)";
			case Weight|Damage|Accuracy:	return "(-wgt, +dmg, +acc)";
			case Weight|Damage|Rate:		return "(-wgt, +dmg, +rate)";
			case Weight|Damage|Reload:		return "(-wgt, +dmg, -rld)";
			case Weight|Damage|Accuracy|Rate:	return "(-wgt, +dmg, +acc, +rate)";
			case Weight|Damage|Accuracy|Reload:	return "(-wgt, +dmg, +acc, -rld)";
			case Weight|Damage|Accuracy|Rate|Reload:	return "(-wgt, +dmg, +acc, +rate, -rld)";
			case Weight|Damage|Rate|Reload:		return "(-wgt, +dmg, +rate, -rld)";
			case Weight|Accuracy|Rate:	return "(-wgt, +acc, +rate)";
			case Weight|Accuracy|Reload:	return "(-wgt, -rld)";
			case Weight|Accuracy|Rate|Reload:	return "(-wgt, +rate, -rld)";
			case Weight|Rate|Reload:	return "(-wgt, +rate, -rld)";
			case Damage|Accuracy:	return "(+dmg, +acc)";
			case Damage|Rate:	return "(+dmg, +rate)";
			case Damage|Reload:	return "(+dmg, -rld)";
			case Damage|Accuracy|Rate:	return "(+dmg, +acc, +rate)";
			case Damage|Accuracy|Reload:	return "(+dmg, +acc, -rld)";
			case Damage|Accuracy|Rate|Reload:	return "(+dmg, +acc, +rate, -rld)";
			case Damage|Rate|Reload:	return "(+dmg, +rate, -rld)";
			case Accuracy|Rate:	return "(+acc, +rate)";
			case Accuracy|Reload:	return "(+acc, -rld)";
			case Accuracy|Rate|Reload:	return "(+acc, +rate, -rld)";
			case Rate|Reload:	return "(+rate, -rld)";
			default:;
		}
		return "(error!)";
	}
}


using namespace weapon;


Weapon::Weapon( weapon::Type type, weapon::Special mods, int ammo )
: AHGameObject(objects::Weapon)
, m_weapon_type(type)
, m_special(mods)
, m_wielder(RL_shared::INVALID_KEY)
, m_ammo(ammo)
, m_disappear(false)
{
	m_ammo = (std::min)(m_ammo, getClipSize(type, m_special));
}

//Constructor for serialization use
Weapon::Weapon( void )
: AHGameObject(objects::Weapon)
, m_weapon_type(weapon::Type_None)
, m_special(weapon::Special_None)
, m_wielder(RL_shared::INVALID_KEY)
, m_ammo(0)
, m_disappear(false)
{
}

const WeaponData& Weapon::weaponData(void) const
{
	return weapon_data::getWeaponData(weaponType(), m_special);
}
const SplashDamage& Weapon::splashDamage(void) const
{
	return weapon_data::getWeaponSplashDamage(weaponType(), m_special);
}

bool Weapon::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Weapon::canOverlap(TerrainType type) const
{
	return terrain::isPassable(type);
}

bool Weapon::removeMe(AHGameModel&) const
{
	return m_disappear;
}
bool Weapon::shouldDraw(void) const
{
	return true;
}

bool Weapon::blocksDoor(void) const
{
	return false;
}

void Weapon::setAmmo(int ammo)
{
	m_ammo = (std::max)(0, (std::min)( getClipSize(weaponType(), m_special), ammo ));
}
int Weapon::decrementAmmo(int amount)
{
	if (amount > m_ammo)
		amount = m_ammo;
	m_ammo -= amount;
	return amount;
}

//TODO put these methods in base class common to Weapon and Pickup (and maybe others)
void Weapon::acidSplash( AHGameModel& model, int amount, int )
{
	if ((!m_disappear) && (RL_shared::INVALID_KEY == wielder()))
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

int Weapon::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	if ((!m_disappear) && (RL_shared::INVALID_KEY == wielder()))
	{
		if ((damage::Explosion == type) || (damage::Fire == type))
		{
			m_disappear = true;
			boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
				events->objectDestroyed(model, *this);
			return amount;
		}
	}
	return 0;
}

std::string Weapon::getThereIsString(bool status) const
{
	std::string weapon_str;

	if (m_special != Special_None)
	{
		switch( weaponType() )
		{
			case Pistol: 
				weapon_str = "a modified M43A pistol "; break;
			case SMG:
				weapon_str = "a modified MP5 sub-machine gun "; break;
			case PulseRifle:
				weapon_str = "a modified M41A pulse rifle "; break;
			case SawnoffShotgun:
				weapon_str = "a modified sawnoff shotgun "; break;
			case CombatShotgun:
				weapon_str = "a modified M37A2 combat shotgun "; break;
			case Colt:
				weapon_str = "a modified .44 Colt "; break;
			case Flamer:
				weapon_str = "a modified M240A1 Incinerator "; break;
			case SmartGun:
				weapon_str = "a modified M56A2 SmartGun "; break;
			case Laser:	
				weapon_str = "a modified M51A phased-plasma Combat Laser "; break;
			case CanisterRifle:	
				weapon_str = "a modified M44A HEDP canister rifle "; break;
			case MachinePistol:
				weapon_str = "a modified MP9 machine-pistol "; break;
			default:;
		}

		weapon_str += getSpecialString(m_special);
	}
	else
	{
		switch( weaponType() )
		{
			case Pistol: 
				weapon_str = "an M43A pistol"; break;
			case SMG:
				weapon_str = "an MP5 sub-machine gun"; break;
			case PulseRifle:
				weapon_str = "an M41A pulse rifle"; break;
			case SawnoffShotgun:
				weapon_str = "a sawnoff shotgun"; break;
			case CombatShotgun:
				weapon_str = "an M37A2 combat shotgun"; break;
			case Colt:
				weapon_str = "a .44 Colt"; break;
			case Flamer:
				weapon_str = "an M240A1 Incinerator"; break;
			case SmartGun:
				weapon_str = "an M56A2 SmartGun"; break;
			case Laser:	
				weapon_str = "an M51A phased-plasma Combat Laser"; break;
			case CanisterRifle:	
				weapon_str = "an M44A HEDP canister rifle"; break;
			case MachinePistol:
				weapon_str = "an MP9 machine-pistol"; break;
			default:;
		}
	}

	if (!weapon_str.empty())
	{
		if (status)
		{
			weapon_str += " ";
			weapon_str += getStatusString();
		}
		return weapon_str;
	}

	return "an unknown object";
}

std::string Weapon::getSelectName(bool status) const
{
	std::string weapon_str;

	//if (status && (m_special != Special_None))
	//{
	//	switch( weaponType() )
	//	{
	//		case Pistol: 
	//			weapon_str = "Pistol"; break;
	//		case SMG:
	//			weapon_str = "SMG"; break;
	//		case PulseRifle:
	//			weapon_str = "Pulse"; break;
	//		case SawnoffShotgun:
	//			weapon_str = "Sawnoff"; break;
	//		case CombatShotgun:
	//			weapon_str = "Shotgun"; break;
	//		case Colt:
	//			weapon_str = "Colt"; break;
	//		case Flamer:
	//			weapon_str = "Flamer"; break;
	//		case SmartGun:
	//			weapon_str = "SmartGun"; break;
	//		case Laser: 
	//			weapon_str = "Laser"; break;
	//		case CanisterRifle:
	//			weapon_str = "Canister"; break;
	//		case MachinePistol:
	//			weapon_str = "MP9"; break;
	//		default:;
	//	}

	//	weapon_str += " ";
	//	weapon_str += getSpecialString(m_special);
	//}
	//else
	{
		switch( weaponType() )
		{
			case Pistol: 
				weapon_str = "Pistol"; break;
			case SMG:
				weapon_str = "MP5 SMG"; break;
			case PulseRifle:
				weapon_str = "Pulse Rifle"; break;
			case SawnoffShotgun:
				weapon_str = "Sawnoff Shotgun"; break;
			case CombatShotgun:
				weapon_str = "Combat Shotgun"; break;
			case Colt:
				weapon_str = ".44 Colt"; break;
			case Flamer:
				weapon_str = "Flamer"; break;
			case SmartGun:
				weapon_str = "SmartGun"; break;
			case Laser: 
				weapon_str = "Combat Laser"; break;
			case CanisterRifle:
				weapon_str = "Canister Rifle"; break;
			case MachinePistol:
				weapon_str = "MP9 MP"; break;
			default:;
		}
	}

	if (!weapon_str.empty())
	{
		if (status)
		{
			if (m_special != Special_None)
			{
				weapon_str += "+";
			}
			weapon_str += " ";
			weapon_str += getStatusString();
		}
		return weapon_str;
	}

	return "Unknown";
}

std::string Weapon::getStatusString(void) const
{
	std::string status_str;

	status_str += "(";
	status_str += boost::lexical_cast<std::string>(m_ammo);
	status_str += "/";
	status_str += boost::lexical_cast<std::string>(getClipSize(weaponType(), m_special));
	status_str += ")";

	return status_str;
}


}
