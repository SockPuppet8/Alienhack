#include "Armour.hpp"
#include "TerrainType.hpp"
#include "ObjectType.hpp"
#include "../IGameEvents.hpp"
#include "../AHGameModel.hpp"
#include <boost/lexical_cast.hpp>



namespace AlienHack
{


namespace armour
{

int getArmourBaseValue( Type type )
{
	switch(type & BaseField)
	{
		case FlakJacket:		return 1;
		case RiotArmour:		return 2;
		case CombatArmour:		return 3;
		case PlasteelArmour:	return 4;
		case AdvCombatArmour:	return 5;
		case PowerArmour:		return 6;
		case ChemhazardSuit:	return 0;
		case ChemhazardArmour:	return 2;
		default:;
	}
	return 0;
}
int getArmourValue( Type type )
{
	return getArmourBaseValue(type) + ((type & Value)?1:0);
}

int getArmourHP( Type type )
{
	switch(type & (BaseField | HP))
	{
		case FlakJacket:			return 20;
		case FlakJacket|HP:			return 30;
		case RiotArmour:			return 30;
		case RiotArmour|HP:			return 45;
		case CombatArmour:			return 40;
		case CombatArmour|HP:		return 60;
		case PlasteelArmour:		return 50;
		case PlasteelArmour|HP:		return 75;
		case AdvCombatArmour:		return 50;
		case AdvCombatArmour|HP:	return 75;
		case PowerArmour:			return 75;
		case PowerArmour|HP:		return 100;
		case ChemhazardSuit:		return 8;
		case ChemhazardSuit|HP:		return 15;
		case ChemhazardArmour:		return 20;
		case ChemhazardArmour|HP:	return 30;
		default:;
	}
	return 0;
}

bool hasAcidImmunity( Type type )
{
	return (ChemhazardSuit == (type & BaseField)) || (ChemhazardArmour == (type & BaseField));
}

bool hasWeightImmunity(Type type)
{
	return (PowerArmour == (type & BaseField));
}

int getWeight( Type type )
{
	switch(type & (BaseField | Weight))
	{
		case FlakJacket:				return 5000;
		case FlakJacket|Weight:			return 4000;
		case RiotArmour:				return 8000;
		case RiotArmour|Weight:			return 6000;
		case CombatArmour:				return 12000;
		case CombatArmour|Weight:		return 10000;
		case PlasteelArmour:			return 10000;
		case PlasteelArmour|Weight:		return 8000;
		case AdvCombatArmour:			return 13500;
		case AdvCombatArmour|Weight:	return 11000;
		case PowerArmour:				return 30000;
		case PowerArmour|Weight:		return 20000;
		case ChemhazardSuit:			return 4000;
		case ChemhazardSuit|Weight:		return 3000;
		case ChemhazardArmour:			return 10000;
		case ChemhazardArmour|Weight:	return 8000;
		default:;
	}
	return 0;
}

}


using namespace armour;


Armour::Armour( armour::Type type, int condition_percent )
: AHGameObject(objects::Armour), m_armour_type(type), m_wearer(RL_shared::INVALID_KEY), m_HP(0)
{
	int max = getArmourHP(type);
	m_HP = ((max * condition_percent) / 100)+1;
	m_HP = (std::min)(max, m_HP);
}

//Constructor for serialization use.
Armour::Armour( void )
: AHGameObject(objects::Armour), m_armour_type(armour::None), m_wearer(RL_shared::INVALID_KEY), m_HP(0)
{
}

bool Armour::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Armour::canOverlap(TerrainType type) const
{
	return terrain::isPassable(type);
}

void Armour::notifyTimeStep( AHGameModel& model, RL_shared::GameTimeCoordinate dt )
{
	if (m_HP > 0)
	{
		AcidSplashable::notifyTimeStep( model, dt, *this );
	}
}
bool Armour::removeMe(AHGameModel&) const
{
	return (m_HP <= 0);
}
bool Armour::shouldDraw(void) const
{
	return true;
}

int Armour::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	if ((damage::Acid == type) && (hasAcidImmunity(armourType())))
		return 0;

	int damage_done(amount);

	if ((damage::Bullet == type) || (damage::Explosion == type) || (damage::Melee == type))
	{
		int deduction = (std::max)(0, getArmourValue(m_armour_type) - penetration);
		damage_done -= deduction;
	}

	damage_done = (std::min)(m_HP, (std::max)(0, damage_done));

	int old_HP = m_HP;
	m_HP = (std::max)(0, m_HP-damage_done);

	using namespace boost;

	RL_shared::World& world( model.world() );
	shared_ptr< IGameEvents > events( model.gameEventsObserver() );

	if (events && (world.objectExists(m_wearer)))
	{
		AHGameObject& game_obj( dynamic_cast< AHGameObject& >(world.object(m_wearer)) );
		if (game_obj.type() == objects::Player)
			events->playerArmourHit(model, dynamic_cast< PlayerCharacter& >(game_obj), type, damage_done);
	}

	if ((0 >= m_HP) && (0 < old_HP))
	{
		if (events)
			events->armourDestroyed(model, *this, type);

		if (world.objectExists(m_wearer) && (!hasAcidImmunity(armourType())))
		{
			shared_ptr< AHGameObject > wearer( dynamic_pointer_cast<AHGameObject>( world.objectPtr(m_wearer) ) );
			shared_ptr< AcidSplashable > lucky_recipient( dynamic_pointer_cast<AcidSplashable>( wearer ) );
			if (lucky_recipient)
			{
				int acid = getAcid();
				if (acid > 0)
				{
					lucky_recipient->addAcid( acid );
					if (events)
						events->acidTransferred(model, *this, *wearer);
				}
				setAcid(0);
			}
		}
	}

	return damage_done;
}

bool Armour::blocksDoor(void) const
{
	return true;
}

void Armour::acidSplash( AHGameModel& model, int amount, int )
{
	if (RL_shared::INVALID_KEY == wearer())
	{
		addAcid( amount );
		boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
		if (events)
			events->acidSplashed(model, *this);
	}
}

std::string getSpecialString(armour::Type type)
{
	using namespace armour;
	switch( type & SpecialFlags )
	{
		case 0: 
			return "";
		case HP: 
			return "(+HP)";
		case Value: 
			return "(+DR)";
		case Weight: 
			return "(-wgt)";
		case HP|Value: 
			return "(+HP, +DR)";
		case HP|Weight: 
			return "(+HP, -wgt)";
		case Value|Weight: 
			return "(+DR, -wgt)";
		case HP|Value|Weight: 
			return "(+HP, +DR, -wgt)";
		default:;
	}
	return "";
}

std::string Armour::getThereIsString(bool status) const
{
	std::string armour_str;

	if (status && (0!=(armourType() & SpecialFlags)))
	{
		switch( armourType() & BaseField )
		{
			case FlakJacket: 
				armour_str = "a modified flak jacket"; break;
			case RiotArmour: 
				armour_str = "modified riot armour"; break;
			case CombatArmour: 
				armour_str = "modified combat armour"; break;
			case PlasteelArmour: 
				armour_str = "modified plasteel armour"; break;
			case AdvCombatArmour: 
				armour_str = "modified heavy combat armour"; break;
			case PowerArmour: 
				armour_str = "a modified powered exoskeleton"; break;
			case ChemhazardSuit:
				armour_str = "a modified chemhazard suit"; break;
			case ChemhazardArmour:
				armour_str = "modified chemhazard armour"; break;
			default:;
		}

		armour_str += " ";
		armour_str += getSpecialString( armourType() );
	}
	else
	{
		switch( armourType() & BaseField )
		{
			case FlakJacket: 
				armour_str = "a flak jacket"; break;
			case RiotArmour: 
				armour_str = "riot armour"; break;
			case CombatArmour: 
				armour_str = "combat armour"; break;
			case PlasteelArmour: 
				armour_str = "plasteel armour"; break;
			case AdvCombatArmour: 
				armour_str = "heavy combat armour"; break;
			case PowerArmour: 
				armour_str = "a powered fighting exoskeleton"; break;
			case ChemhazardSuit:
				armour_str = "a chemhazard suit"; break;
			case ChemhazardArmour:
				armour_str = "chemhazard armour"; break;
			default:;
		}
	}
	
	if (!armour_str.empty())
	{
		if (status)
		{
			armour_str += " [";
			armour_str += boost::lexical_cast<std::string>(m_HP);
			armour_str += "]";
		}
		return armour_str;
	}

	return "an unknown object";
}

std::string Armour::getSelectName(bool status) const
{
	std::string armour_str;

	{
		switch( armourType() & BaseField )
		{
			case FlakJacket: 
				armour_str = "Flak Jacket"; break;
			case RiotArmour: 
				armour_str = "Riot Armour"; break;
			case CombatArmour: 
				armour_str = "Combat Armour"; break;
			case PlasteelArmour: 
				armour_str = "Plasteel Armour"; break;
			case AdvCombatArmour: 
				armour_str = "Heavy Armour"; break;
			case PowerArmour: 
				armour_str = "Power Armour"; break;
			case ChemhazardSuit:
				armour_str = "Chemhazard Suit"; break;
			case ChemhazardArmour:
				armour_str = "Chemhazard Armour"; break;
			default:;
		}
	}

	if (!armour_str.empty())
	{
		if (status)
		{
			if (armourType() & SpecialFlags)
			{
				armour_str += "+";
			}
			armour_str += " [";
			armour_str += boost::lexical_cast<std::string>(m_HP);
			armour_str += "]";
		}
		return armour_str;
	}

	return "Unknown";
}


}
