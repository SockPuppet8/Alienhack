#include "PlayerCharacter.hpp"
#include "PlayerStats.hpp"
#include "Alien.hpp"
#include "Armour.hpp"
#include "Weapon.hpp"
#include "WeaponData.hpp"
#include "Decoration.hpp"
#include "Explosion.hpp"
#include "ObjectType.hpp"
#include "TerrainType.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"
#include "World-2DTiles/Zone.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>



namespace AlienHack
{


using namespace RL_shared;


namespace player_perks
{
	const char * perkName( Perk perk )
	{
		switch (perk)
		{
			case SingleShotAccuracy:			return "Marksman";
			case MultiShotAccuracy1:			return "Gunner (I)";
			case MultiShotAccuracy2:			return "Gunner (II)";
			case GrenadeAccuracy1:				return "Grenadier (I)";
			case GrenadeAccuracy2:				return "Grenadier (II)";
			case SingleShotCriticalBonus1:		return "Sharpshooter (I)";
			case SingleShotCriticalBonus2:		return "Sharpshooter (II)";
			case LowWeightMoveSpeed1:			return "Lightfoot (I)";
			case LowWeightMoveSpeed2:			return "Lightfoot (II)";
			case HighWeightMoveSpeed1:			return "Heavy Unit (I)";
			case HighWeightMoveSpeed2:			return "Heavy Unit (II)";
			case ReloadAndWeaponSwitchSpeed1:	return "Gunslinger (I)";
			case ReloadAndWeaponSwitchSpeed2:	return "Gunslinger (II)";
			case ItemCapacity1:					return "Utility Belt (I)";
			case ItemCapacity2:					return "Utility Belt (II)";
			case AmmoCapacity:					return "Bandolier";
			case Dodge1:						return "Artful Dodger (I)";
			case Dodge2:						return "Artful Dodger (II)";
			case DarkSightRange:				return "Night Vision";
			case FlameDamage:					return "Burninator";
			case FlameBoost:					return "Pyromaniac";
			case Hearing1:						return "Keen Hearing (I)";
			case Hearing2:						return "Keen Hearing (II)";
			case AgileStunAttacker:				return "Slippery Customer";
			case LoseLessMaxHP:					return "Field Surgeon";
			case ArmourBonus1:					return "Tank Person (I)";
			case ArmourBonus2:					return "Tank Person (II)";
			case HigherWeightThreshold:			return "GM Joe";
			default:;
		};
		return "None";
	};

	const char * perkDescription( Perk perk )
	{
		switch (perk)
		{
			case SingleShotAccuracy:			return "Increased accuracy with all single-shot weapons";
			case MultiShotAccuracy1:			return "Increased accuracy with multi-shot bullet weapons";
			case MultiShotAccuracy2:			return "Increased accuracy and speed with multi-shot bullet weapons";
			case GrenadeAccuracy1:
			case GrenadeAccuracy2:				return "Increased accuracy when throwing grenades";
			case SingleShotCriticalBonus1:
			case SingleShotCriticalBonus2:		return "Increased critical hit rate with single-shot bullet and laser weapons";
			case LowWeightMoveSpeed1:
			case LowWeightMoveSpeed2:			return "Increased speed when carrying low weight";
			case HighWeightMoveSpeed1:
			case HighWeightMoveSpeed2:			return "Reduced speed loss when carrying high weight";
			case ReloadAndWeaponSwitchSpeed1:
			case ReloadAndWeaponSwitchSpeed2:	return "Decreased reload times and weapon switch times";
			case ItemCapacity1:
			case ItemCapacity2:					return "Carry more grenades and items";
			case AmmoCapacity:					return "Carry more ammo";
			case Dodge1:
			case Dodge2:						return "Better chance of dodging enemy attacks and acid. Affected by weight";
			case DarkSightRange:				return "See further in darkness";
			case FlameDamage:					return "Double damage with flame weapons";
			case FlameBoost:					return "Reduced flamer fuel use, increased inc grenade blast range";
			case Hearing1:
			case Hearing2:						return "Just what it says!";
			case AgileStunAttacker:				return "Better chance to fend off enemies at close range. Affected by weight";
			case LoseLessMaxHP:					return "More effective medkit use";
			case ArmourBonus1:
			case ArmourBonus2:					return "Increased protection from armour";
			case HigherWeightThreshold:			return "Carry 5kg extra with no penalty";
			default:;
		};
		return "None";
	};

	Perk perkRequires( Perk perk )
	{
		switch (perk)
		{
			case SingleShotAccuracy:			return None;
			case MultiShotAccuracy1:			return None;
			case MultiShotAccuracy2:			return MultiShotAccuracy1;
			case GrenadeAccuracy1:				return None;
			case GrenadeAccuracy2:				return GrenadeAccuracy1;
			case SingleShotCriticalBonus1:		return None;
			case SingleShotCriticalBonus2:		return SingleShotCriticalBonus1;
			case LowWeightMoveSpeed1:			return None;
			case LowWeightMoveSpeed2:			return LowWeightMoveSpeed1;
			case HighWeightMoveSpeed1:			return None;
			case HighWeightMoveSpeed2:			return HighWeightMoveSpeed1;
			case ReloadAndWeaponSwitchSpeed1:	return None;
			case ReloadAndWeaponSwitchSpeed2:	return ReloadAndWeaponSwitchSpeed1;
			case ItemCapacity1:					return None;
			case ItemCapacity2:					return ItemCapacity1;
			case AmmoCapacity:					return None;
			case Dodge1:						return None;
			case Dodge2:						return Dodge1;
			case DarkSightRange:				return None;
			case FlameDamage:					return None;
			case FlameBoost:					return None;
			case Hearing1:						return None;
			case Hearing2:						return Hearing1;
			case AgileStunAttacker:				return None;
			case LoseLessMaxHP:					return None;
			case ArmourBonus1:					return None;
			case ArmourBonus2:					return ArmourBonus1;
			case HigherWeightThreshold:			return None;
			default:;
		};
		return None;
	};

	int perkXPCost( Perk perk )
	{
		switch (perk)
		{
			case SingleShotAccuracy:			return 250;
			case MultiShotAccuracy1:			return 375;
			case MultiShotAccuracy2:			return 500;
			case GrenadeAccuracy1:				return 375;
			case GrenadeAccuracy2:				return 625;
			case SingleShotCriticalBonus1:		return 375;
			case SingleShotCriticalBonus2:		return 500;
			case LowWeightMoveSpeed1:			return 375;
			case LowWeightMoveSpeed2:			return 750;
			case HighWeightMoveSpeed1:			return 250;
			case HighWeightMoveSpeed2:			return 375;
			case ReloadAndWeaponSwitchSpeed1:	return 250;
			case ReloadAndWeaponSwitchSpeed2:	return 500;
			case ItemCapacity1:					return 500;
			case ItemCapacity2:					return 500;
			case AmmoCapacity:					return 875;
			case Dodge1:						return 250;
			case Dodge2:						return 750;
			case DarkSightRange:				return 250;
			case FlameDamage:					return 375;
			case FlameBoost:					return 500;
			case Hearing1:						return 375;
			case Hearing2:						return 375;
			case AgileStunAttacker:				return 375;
			case LoseLessMaxHP:					return 175;
			case ArmourBonus1:					return 375;
			case ArmourBonus2:					return 375;
			case HigherWeightThreshold:			return 1000;
			default:;
		};
		return 0;
	}

	boost::uint32_t perkBitMask( Perk perk )
	{
		return (perk > None) ? 1 << ((boost::uint32_t)perk-1) : 0;
	}
}



namespace
{
	int maxCarryable( pickup::Type type, int item_expand, int ammo_expand )
	{
		using namespace pickup;
		switch(type)
		{
			case Medkit:			return 3 + item_expand;
			case Neutraliser:		return 5 + (2*item_expand);
			case Ammo_9mm:			return 300 + (100*ammo_expand);
			case Ammo_shotgun:		return 100 + (50*ammo_expand);
			case Ammo_colt:			return (20 + (10*ammo_expand)) * weapon::getClipSize( weapon::Colt, weapon::Special_None );
			case Ammo_pulse:		return (3+ammo_expand) * weapon::getClipSize( weapon::PulseRifle, weapon::Special_None );
			case Ammo_flame:		return (4+(3*ammo_expand)) * weapon::getClipSize( weapon::Flamer, weapon::Special_None );
			case Ammo_smartgun:		return (1+ammo_expand) * weapon::getClipSize( weapon::SmartGun, weapon::Special_None );
			case Ammo_laser:		return (4+(2*ammo_expand)) * weapon::getClipSize( weapon::Laser, weapon::Special_None );
			case Ammo_HEDP30mm:		return (2+(2*ammo_expand)) * weapon::getClipSize( weapon::CanisterRifle, weapon::Special_None );
			case StunGrenade:		return 5 + (2*item_expand);
			case FragGrenade:		return 5 + (2*item_expand);
			case IncGrenade:		return 5 + (2*item_expand);
			case KrakGrenade:		return 5 + (2*item_expand);
			case MotionTracker:		return 1;
			case CreditChip:		return 999;
			case DemoCharge:		return 3 + item_expand;
			case MegaCredChip:		return 999;
			default:;
		}
		return 0;
	}
	int numPerPickup( pickup::Type type )
	{
		using namespace pickup;
		switch(type)
		{
			case Medkit:			return 1;
			case Neutraliser:		return 1;
			case Ammo_9mm:			return 10;//weapon::getClipSize( weapon::Pistol );
			case Ammo_shotgun:		return 6;
			case Ammo_colt:			return weapon::getClipSize( weapon::Colt, weapon::Special_None );
			case Ammo_pulse:		return weapon::getClipSize( weapon::PulseRifle, weapon::Special_None );
			case Ammo_flame:		return weapon::getClipSize( weapon::Flamer, weapon::Special_None );
			case Ammo_smartgun:		return weapon::getClipSize( weapon::SmartGun, weapon::Special_None );
			case Ammo_laser:		return weapon::getClipSize( weapon::Laser, weapon::Special_None );
			case Ammo_HEDP30mm:		return weapon::getClipSize( weapon::CanisterRifle, weapon::Special_None );
			case StunGrenade:		return 1;
			case FragGrenade:		return 1;
			case IncGrenade:		return 1;
			case KrakGrenade:		return 1;
			case MotionTracker:		return 1;
			case CreditChip:		return 1;
			case DemoCharge:		return 1;
			case MegaCredChip:		return 1;
			default:;
		}
		return 0;
	}
	//TODO move this into Pickup
	bool isItem( pickup::Type type )
	{
		using namespace pickup;
		switch(type)
		{
			case Medkit:
			case Neutraliser:
			case DemoCharge:
				return true;
			default:;
		}
		return false;
	}

	GameTimeCoordinate getBaseActionTime( player_actions::Action action )
	{
		using namespace player_actions;
		switch(action)
		{
			case OpenDoor:				return 500;
			case CloseDoor:				return 500;
			case Move:					return 400;
			case MoveDiagonal:			return 566;
			case Turn:					return 200;
			case Wait:					return 150;
			case PickupObject:			return 1000;
			case DropArmour:			return 1500;
			case PickupArmour:			return 6000;
			case DropWeapon:			return 400;
			case DropItem:		    	return 1000;
			case PickupWeapon:			return 1500;
			case SwitchToSidearm:		return 300;
			case SwitchToPrimary:		return 800;
			case ReloadWeapon:			return 0; //from weapon data
			case UseNeutraliser:		return 2000;
			case UseMedkit:				return 30000;
			case UseMedkitPlus:		    return 60000;
			case ThrowGrenade:			return 500;
			case UseTerminal:			return 5000;
			case TakeAmmo:				return 2000;
			case SetCharge:				return 3000;
			default:;
		}
		return 0;
	}

	bool weightAffectsAction( player_actions::Action action )
	{
		using namespace player_actions;
		switch(action)
		{
			case OpenDoor:				return false;
			case CloseDoor:				return false;
			case Move:					return true;
			case MoveDiagonal:			return true;
			case Turn:					return true;
			case Wait:					return false;
			case PickupObject:			return true;
			case DropArmour:			return true;
			case PickupArmour:			return true;
			case DropWeapon:			return false;
			case DropItem:  			return false;
			case PickupWeapon:			return true;
			case SwitchToSidearm:		return false;
			case SwitchToPrimary:		return false;
			case ReloadWeapon:			return false;
			case UseNeutraliser:		return false;
			case UseMedkit:				return false;
			case UseMedkitPlus:			return false;
			case ThrowGrenade:			return false;
			case UseTerminal:			return false;
			case TakeAmmo:				return true;
			case SetCharge:				return false;
			default:;
		}
		return false;
	}
}




PlayerCharacter::PlayerCharacter( AHGameModel& model )
: AHGameObject(objects::Player)
, m_name("John Doe")
, m_armour(INVALID_KEY)
, m_sidearm(INVALID_KEY)
, m_primary(INVALID_KEY)
, m_held_weapon(Primary)
, m_XP(0)
, m_spent_XP(0)
, m_perks(0)
, m_heading(90)
, m_HP(getTopHP())
, m_max_HP(getTopHP())
{
	for (int ti(0); ti < pickup::NumTypes; ++ti)
		m_pickups[ti] = 0;

	m_stats.reset( new PlayerStats() );
	model.registerPlayerStats( m_stats );
}

PlayerCharacter::PlayerCharacter(void)
	: AHGameObject(objects::Player), m_held_weapon(WeaponSlot_None), m_XP(0), m_spent_XP(0), m_perks(0), m_heading(0), m_HP(0), m_max_HP(0)
{
	for (int ti(0); ti < pickup::NumTypes; ++ti)
		m_pickups[ti] = 0;
}

bool PlayerCharacter::canOverlap(const WorldObject& other) const
{
	return true;
}
bool PlayerCharacter::canOverlap(TerrainType what) const
{
	return terrain::isPassable(what);
}

bool PlayerCharacter::removeMe(AHGameModel&) const
{
	return (m_HP <= 0);
}

bool PlayerCharacter::blocksDoor(void) const
{
	return true;
}

int PlayerCharacter::getDodgeChance(const AHGameModel& model) const
{
	long base_chance = hasPerk(player_perks::Dodge2) ? 45 : hasPerk(player_perks::Dodge1) ? 25 : 10;
	long speed_adjust = -getWeightPenalty(model, getWeight(model));

	if (!hasPerk(player_perks::Dodge1))
		if (speed_adjust > 0)
			speed_adjust /= 3;

	speed_adjust *= 2;

	base_chance += ((base_chance * speed_adjust) / 1000000L);
	base_chance = (std::min)(75L, (std::max)(0L, base_chance));
	return base_chance;
}

int PlayerCharacter::takeDamage( AHGameModel& model, DBKeyValue attacker_key, damage::Type type, int amount, int penetration )
{
	boost::shared_ptr<IGameEvents> events( model.gameEventsObserver() );

	bool dodged(false);

	if (damage::Melee == type)
	{
		int dodge = getDodgeChance(model);
		if (0 != dodge)
		{
			//NOTE this calculation has a slight bias (1 part in 656) to values under 35 due to the 35 remainder from 65535/100
			if ((rand() % 100) < dodge)
			{
				if (events)
					events->playerDodgedAttack(model, *this, attacker_key);

				dodged = true;
			}
		}
	}

	int damage(0);

	if (!dodged)
		damage = takeDamage(model, type, amount, penetration);

	if (damage::Melee == type)
	{
		if ((m_HP > 0) && (model.world().objectExists(attacker_key)))
		{
			bool has_perk = hasPerk(player_perks::AgileStunAttacker);
			long chance = has_perk ? 300000 : 100000;
			chance += (-1 == getWeight(model)) ? 200000 : 0; //bonus for wearing power armour
			long penalty = getWeightPenalty(model, getWeight(model));
			if ((penalty > 0) && (!has_perk))
				penalty /= 2;
			chance = (std::max)(0L, (std::min)(750000L, chance-penalty));
			boost::uint32_t test( (rand() & 0xffff) | (rand() << 16) );
			if ((long)(test % 1000000) < chance) //some bias here
			{
				AHGameObject& attacker( dynamic_cast< AHGameObject& >( model.world().object(attacker_key) ) );

				if (events)
					events->playerFendedAttacker(model, *this, attacker);

				attacker.stun(model, 750);
			}
		}
	}

	//TODO: move this stuff somewhere else and improve it.
	if (m_HP <= 0)
	{
		if (damage::Acid == type)
			writeOutcome(model, "Killed by acid");
		else if (damage::Fire == type)
			writeOutcome(model, "Incinerated by flames");
		else if (damage::Explosion == type)
			writeOutcome(model, "Killed in an explosion");
		else
		{
			std::string death_str;
			if (!model.world().objectExists(attacker_key))
			{
				death_str = "Killed by the dying blow of an enemy";
			}
			else
			{
				AHGameObject& attacker( dynamic_cast< AHGameObject& >( model.world().object(attacker_key) ) );
				
				if (objects::Alien == attacker.type())
				{
					//TODO move this stuff to Alien class?
					const Alien& alien( dynamic_cast< const Alien& >( attacker ) );
					if (aliens::isLargeAlien( alien.alienType() ))
					{
						death_str = "Torn apart by ";
						if (aliens::Queen == alien.alienType())
							death_str += "the ";
						else
							death_str += "a ";

						death_str += alien.getSelectName(false);
					}
					else if (aliens::isSmallAlien( alien.alienType() ))
					{
						death_str = "Killed and eaten by a ";
						death_str += alien.getSelectName(false);
					}
					else
					{
						death_str = "Crippled by a ";
						death_str += alien.getSelectName(false);
						death_str += " and dragged away to the alien hive";
					}
				}
				else 
				{
					death_str = "Killed by a ";
					death_str += attacker.getSelectName(false);
				}
			}

			writeOutcome( model, death_str.c_str() );
		}
	}

	return damage;
}

int PlayerCharacter::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	World& world( model.world() );

	boost::shared_ptr<IGameEvents> events( model.gameEventsObserver() );

	if (world.objectExists(m_armour))
	{
		Armour& my_armour( dynamic_cast< Armour& >( world.object(m_armour) ) );

		if (my_armour.getHP() > 0)
		{
			int armour_level( armour::getArmourValue( my_armour.armourType() ) );

			if (hasPerk(player_perks::ArmourBonus1))
				{++armour_level; --amount;}
			if (hasPerk(player_perks::ArmourBonus2))
				{++armour_level; --amount;}

			armour_level = (std::max)(0, armour_level - penetration);

			int through_damage((std::max)(0, amount - armour_level));

			if (damage::Acid != type)
			{
			    int armour_absorb(armour_level + 5);
			    int temp = through_damage;
				for (int i=0; i < temp; ++i)
					if (0 != (rand() % armour_absorb))
						--through_damage;
			}
			else
			    armour_level = 0;

			int damage_taken = my_armour.takeDamage(model, type, amount-through_damage, penetration);

			if ((damage::Acid != type) && (damage_taken > 0))
				interruptActions(model);

            amount = through_damage;

            if (amount <= 0)
                return damage_taken;
		}
	}

	if (m_HP <= 0)
		return 0;

	amount = (std::max)(0, amount);
	amount = (std::min)((int)m_HP, amount);

	if ((damage::Acid != type) && (amount > 0))
		interruptActions(model);

	m_HP -= amount;
	m_HP = (std::max)(0, (int)m_HP);

	if (events)
	{
		events->playerHit(model, *this, type, amount);
		if (m_HP <= 0)
			events->playerDies(model, *this);
	}

	WorldLocation my_loc( location() );

	if (m_HP <= 0)
	{
		World& world( model.world() );
		if (world.zoneExists(my_loc.zone))
		{
			Zone& zone( world.zone( my_loc.zone ) );
			boost::shared_ptr< Decoration > corpse( new Decoration( decoration::Corpse ) );
			DBKeyValue corpse_key( world.addWorldObject( corpse ) );
			corpse->moveTo( model, my_loc, true );
		}
	}

	return amount;
}

void PlayerCharacter::notifyTimeStep( AHGameModel& model, RL_shared::GameTimeCoordinate dt )
{
	if (m_HP > 0)
	{
		AcidSplashable::notifyTimeStep( model, dt, *this );
	}

	World& world( model.world() );
	if (world.objectExists(m_armour))
	{
		Armour& my_armour( dynamic_cast< Armour& >( world.object(m_armour) ) );
		if (WorldObject::WorldLocation::INVALID_ZONE == my_armour.location().zone)
			my_armour.notifyTimeStep(model, dt);

//		if (my_armour.getHP() <= 0)
//			m_armour = INVALID_KEY;
	}
}

std::string PlayerCharacter::getThereIsString(bool) const
{
	return "";
}

std::string PlayerCharacter::getSelectName(bool) const
{
	return "You";
}

int PlayerCharacter::num( pickup::Type type ) const
{
	ASSERT(type < pickup::NumTypes);
	if (type < 0)
	{
		ASSERT( pickup::None == type );
		return 0;
	}
	return m_pickups[type];
}

int PlayerCharacter::max( pickup::Type type ) const
{
	ASSERT(type < pickup::NumTypes);
	if (type < 0)
	{
		ASSERT( pickup::None == type );
		return 0;
	}
	int item_expand = hasPerk(player_perks::ItemCapacity2) ? 2 : 
					  hasPerk(player_perks::ItemCapacity1) ? 1 : 0;
	return maxCarryable(type, item_expand, (hasPerk(player_perks::AmmoCapacity) ? 1 : 0));
}

void PlayerCharacter::dec( pickup::Type type, bool used )
{
	ASSERT(type < pickup::NumTypes);
	if (type < 0)
	{
		ASSERT( pickup::None == type );
		return;
	}
	if (m_pickups[type] > 0)
	{
		m_pickups[type] -= 1;

		if (used && m_stats)
			++(m_stats->pickupsUsed(type));
	}
}

bool PlayerCharacter::canPickUp( pickup::Type type )
{
	ASSERT(type < pickup::NumTypes);
	if (type < 0)
	{
		ASSERT( pickup::None == type );
		return false;
	}
	int max = this->max(type);
	int num = numPerPickup(type);
	if (m_pickups[type] >= max)
		return false;
	return true;
}

bool PlayerCharacter::pickUp( Pickup& item )
{
	pickup::Type type = item.pickupType();
	if (type < 0)
	{
		ASSERT( pickup::None == type );
		return false;
	}

	int num = numPerPickup(type);

	if (addPickup(type, num))
	{
		item.disappear();

		if (pickup::CreditChip == type)
			m_XP += 25;
		else if (pickup::MegaCredChip == type)
			m_XP += 100;

		return true;
	} 
	return false;
}

bool PlayerCharacter::addPickup(pickup::Type type, int num)
{
	if (!canPickUp(type))
		return false;

	m_pickups[type] += num;
	m_pickups[type] = (std::min)(m_pickups[type], (boost::uint16_t)max(type));
	return true;
}

bool PlayerCharacter::canUseItem( pickup::Type type )
{
	ASSERT( type < pickup::NumTypes );
	if (type < 0)
	{
		ASSERT( pickup::None == type );
		return false;
	}
	return isItem(type) && (m_pickups[type] > 0);
}

bool PlayerCharacter::useItem( AHGameModel& model, pickup::Type type )
{
	ASSERT( type < pickup::NumTypes );
	if (type < 0)
	{
		ASSERT( pickup::None == type );
		return false;
	}

	boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );

	if (!isItem(type))
		throw std::runtime_error(std::string("Type ") + boost::lexical_cast<std::string>(type) + " is not an item in PlayerCharacter::useItem");

	if (0 >= m_pickups[type])
		return false;

	switch( type )
	{
		case pickup::Neutraliser: 
		{
			neutraliseAcid(model);
			--m_pickups[pickup::Neutraliser];
			if (events)
				events->playerUsedNeutraliser(model, *this);
			if (m_stats)
				++(m_stats->pickupsUsed( pickup::Neutraliser ));
			return true;
		}
		case pickup::Medkit:
		{
		    if (m_HP < (m_max_HP-1))
		    {
			    if (hasPerk(player_perks::LoseLessMaxHP))
				    m_max_HP = (std::min)(m_max_HP, (boost::int16_t)(((m_HP + m_max_HP) / 2)+2));
			    else
				    m_max_HP = (m_HP + m_max_HP) / 2;
			    m_HP = m_max_HP;
			    --m_pickups[pickup::Medkit];
			    if (events)
				    events->playerUsedMedkit(model, *this);
			} 
			else if (m_max_HP < getTopHP()) 
			{
			    m_max_HP += hasPerk(player_perks::LoseLessMaxHP) ? 2 : 1;
				m_max_HP = (std::min)(m_max_HP, (boost::int16_t)getTopHP());
			    m_HP = m_max_HP;
			    --m_pickups[pickup::Medkit];
			    if (events)
				    events->playerUsedMedkitPlus(model, *this);
			} 
			else 
			    return false;
			if (m_stats)
				++(m_stats->pickupsUsed( pickup::Medkit ));
            return true;
		}
		default:;
	}
	return false;
}

void PlayerCharacter::wearArmour( AHGameModel& model, Armour& armour )
{
	World& world( model.world() );
	if (world.objectExists(m_armour))
		dropArmour(model);

	if (armour.moveTo(model, WorldObject::WorldLocation(), true))
	{
		armour.setWearer(key());
		m_armour = armour.key();

		boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
		if (events)
			events->playerWearsArmour(model, *this, armour);
	}
	else
	{
		//TODO do something here?
	}
}

void PlayerCharacter::dropArmour( AHGameModel& model )
{
	World& world( model.world() );
	if (world.objectExists(m_armour))
	{
		Armour& armour( dynamic_cast<Armour&>( world.object(m_armour) ) );

		//TODO: handle the case where this call fails. 
		//Currently, the armour will stay in limbo (INVALID_KEY == zone) and become inaccessible. 
		armour.moveTo( model, location(), true );
		armour.setWearer(INVALID_KEY);

		boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
		if (events)
			events->playerDropsArmour(model, *this, armour);
	}

	m_armour = INVALID_KEY;
}

void PlayerCharacter::useWeapon( AHGameModel& model, Weapon& weapon_obj, WeaponSlot slot )
{
	World& world( model.world() );

	DBKeyValue weapon_key( weapon(slot) );

	if (world.objectExists( weapon_key ))
		dropWeapon(model, slot);

	if (weapon_obj.moveTo(model, WorldObject::WorldLocation(), true))
	{
		weapon_obj.setWielder(key());
		setWeapon( slot, weapon_obj.key() );

		boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
		if (events)
			events->playerUsesWeapon(model, *this, weapon_obj);
	}
	else
	{
		//TODO do something here?
	}
}

void PlayerCharacter::dropWeapon( AHGameModel& model, WeaponSlot slot )
{
	World& world( model.world() );

	DBKeyValue weapon_key( weapon(slot) );

	if (world.objectExists(weapon_key))
	{
		Weapon& weapon_obj( dynamic_cast<Weapon&>( world.object(weapon_key) ) );

		takeAmmo( model, weapon_obj, false );

		//TODO: handle the case where this call fails. 
		//Currently, the weapon will stay in limbo (INVALID_KEY == zone) and become inaccessible. 
		weapon_obj.moveTo( model, location(), true );
		weapon_obj.setWielder(INVALID_KEY);

		boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
		if (events)
			events->playerDropsWeapon(model, *this, weapon_obj);
	}

	setWeapon( slot, INVALID_KEY );
}

int PlayerCharacter::takeAmmo( AHGameModel& model, Weapon& weapon_obj, bool specific_action )
{
	//Take any ammo out of the weapon, if it can be carried
	pickup::Type ammo_type = weapon::getAmmoType(weapon_obj.weaponType());
	ASSERT( ammo_type < pickup::End);
	if (ammo_type >= 0)
	{
		boost::shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

		int weapon_ammo = weapon_obj.getAmmo();
		int max_keep = max(ammo_type) - num(ammo_type);

		if ((max_keep < 1) || ((weapon_ammo < 1) && specific_action))
		{
			if (game_events)
				game_events->playerCouldNotTakeAmmo( model, *this, weapon_obj );
			return 0;
		}

		int keep = (std::min)(weapon_ammo, max_keep);
		m_pickups[ammo_type] += keep;
		weapon_obj.decrementAmmo( keep );

		if (game_events)
			game_events->playerTookAmmo( model, *this, weapon_obj, keep );

		return keep;
	}
	return 0;
}

int PlayerCharacter::getArmourHP( const AHGameModel& model ) const
{
	const World& world( model.world() );
	if (!world.objectExists(m_armour))
		return 0;

	const Armour& my_armour( dynamic_cast< const Armour& >( world.object(m_armour) ) );
	return my_armour.getHP();
}
int PlayerCharacter::getArmourMaxHP( const AHGameModel& model ) const
{
	const World& world( model.world() );
	if (!world.objectExists(m_armour))
		return 0;

	const Armour& my_armour( dynamic_cast< const Armour& >( world.object(m_armour) ) );
	return armour::getArmourHP( my_armour.armourType() );
}

weapon::Type PlayerCharacter::getHeldWeaponType( const AHGameModel& model ) const
{
	const World& world( model.world() );
	DBKeyValue weapon_key( weapon(m_held_weapon) );

	if (!world.objectExists( weapon_key ))
		return weapon::Type_None;

	const Weapon& my_weapon( dynamic_cast< const Weapon& >( world.object( weapon_key ) ) );
	return my_weapon.weaponType();
}
int PlayerCharacter::getHeldWeaponAmmoLoaded( const AHGameModel& model ) const
{
	const World& world( model.world() );
	DBKeyValue weapon_key( weapon(m_held_weapon) );

	if (!world.objectExists( weapon_key ))
		return 0;

	const Weapon& my_weapon( dynamic_cast< const Weapon& >( world.object( weapon_key ) ) );
	return my_weapon.getAmmo();
}
int PlayerCharacter::getHeldWeaponClipSize( const AHGameModel& model ) const
{
	const World& world( model.world() );
	DBKeyValue weapon_key( weapon(m_held_weapon) );

	if (!world.objectExists( weapon_key ))
		return 0;

	const Weapon& my_weapon( dynamic_cast< const Weapon& >( world.object( weapon_key ) ) );
	return my_weapon.weaponData().clip_size;
}
int PlayerCharacter::getHeldWeaponAmmoStock( const AHGameModel& model ) const
{
	return num( weapon::getAmmoType( getHeldWeaponType(model) ) );
}
int PlayerCharacter::getHeldWeaponAmmoStockMax( const AHGameModel& model ) const
{
	return max( weapon::getAmmoType( getHeldWeaponType(model) ) );
}

void PlayerCharacter::huggerAttack( AHGameModel& model, Alien& attacker )
{
	World& world( model.world() );

	boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
	if (events)
		events->huggerAttack(model, attacker);

	bool huggered( true );

	int dodge = getDodgeChance(model);
	if (0 != dodge)
	{
		//NOTE this calculation has a slight bias (1 part in 656) to values under 35 due to the 35 remainder from 65535/100
		if ((rand() % 100) < dodge)
		{
			huggered = false;
		}
	}
	if (huggered)
		if (rand() & 3)
			huggered = false;

	if (huggered)
	{
		m_HP = 0;

		if (events)
			events->playerHuggered(model, *this);

		writeOutcome(model, "Implanted by an alien parasite");
	}
	else
	{
		if (events)
			events->playerDodgedHugger(model, *this);
		if (m_stats)
			++(m_stats->huggerAttacksSurvived());
		attacker.stun(model, 500+(rand()%100));
	}

	interruptActions( model );
}

void PlayerCharacter::acidSplash( AHGameModel& model, int amount, int max )
{
	World& world( model.world() );

	if (amount < 1)
		return;

	int dodge = getDodgeChance(model);
	if (0 != dodge)
	{
		//NOTE this calculation has a slight bias (1 part in 656) to values under 35 due to the 35 remainder from 65535/100
		if ((rand() % 100) < dodge)
		{
			boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
			if (events)
				events->playerDodgedAcid(model, *this);

			return;
		}
	}

	boost::shared_ptr< IGameEvents > events( model.gameEventsObserver() );
	if (events)
		events->acidSplashed(model, *this);

	bool interrupt(true);

	if (world.objectExists(m_armour))
	{
		Armour& my_armour( dynamic_cast< Armour& >( world.object(m_armour) ) );

		if (armour::hasAcidImmunity(my_armour.armourType()))
			interrupt = false;

		if (my_armour.getHP() > 0)
		{
			amount = (std::max)(0, (std::min)( amount, max - my_armour.getAcid() ));

			my_armour.addAcid( amount );

			if (m_stats)
				m_stats->acid() += amount;

			return;
		}
	}

	if (interrupt)
		interruptActions(model);

	amount = (std::max)(0, (std::min)( amount, max - getAcid() ));
	addAcid(amount);

	if (m_stats)
		m_stats->acid() += amount;
}

void PlayerCharacter::neutraliseAcid( AHGameModel& model )
{
	World& world( model.world() );

	setAcid(0);

	if (world.objectExists(m_armour))
	{
		Armour& my_armour( dynamic_cast< Armour& >( world.object(m_armour) ) );
		my_armour.setAcid( 0 );
	}
}

bool PlayerCharacter::isAcidSplashed( const AHGameModel& model ) const
{
	if (getAcid() > 0)
		return true;

	const World& world( model.world() );
	if (world.objectExists(m_armour))
	{
		const Armour& my_armour( dynamic_cast< const Armour& >( world.object(m_armour) ) );
		if (my_armour.getAcid() > 0)
			return true;
	}

	return false;	
}

void PlayerCharacter::setCurrentWeapon(AHGameModel& model, WeaponSlot slot) 
{
	boost::shared_ptr< IGameEvents > game_events( model.gameEventsObserver() );

	if (slot != m_held_weapon)
	{
		if (game_events)
			game_events->playerSwitchedWeapon(model, *this, m_held_weapon, slot);

		m_held_weapon = slot;
	}
}

void PlayerCharacter::reloadWeapon(AHGameModel& model)
{
	World& world( model.world() );
	DBKeyValue weapon_key( weapon( currentWeapon() ) );
	if (world.objectExists(weapon_key))
	{
		Weapon& weapon_obj( dynamic_cast< Weapon& >( world.object(weapon_key) ) );
		weapon::Type weapon_type( weapon_obj.weaponType() );
		int empty_space = weapon_obj.weaponData().clip_size - weapon_obj.getAmmo();
		pickup::Type ammo_type( weapon::getAmmoType(weapon_type) );
		if ((ammo_type >= 0) && (ammo_type < pickup::End))
		{
			int available = m_pickups[ammo_type];
			int num_to_reload = (std::min)( available, empty_space );
			weapon_obj.setAmmo( weapon_obj.getAmmo() + num_to_reload );
			m_pickups[ammo_type] -= num_to_reload;
		}
	}
}


bool PlayerCharacter::hasMotionDetection( const AHGameModel& model ) const
{
	bool has_power_armour(-1 == getWeight(model));

	return (num( pickup::MotionTracker ) > 0) && 
		((m_held_weapon != Primary) || (weapon::SmartGun == getHeldWeaponType(model)) || has_power_armour);
}


bool PlayerCharacter::isValidTargetOffset( int dx, int dz )
{
	int heading = m_heading;
	while (heading < -180)
		heading += 360;
	while (heading > 180)
		heading -= 360;

	if ((heading >= -23) && (heading <= 23))
	{
		return dx >= 0;
	}
	else if ((heading >= 23) && (heading <= 67))
	{
		return ((dx >= 0) && (dx >= -dz)) || ((dz >= 0) && (dz >= -dx));
	}
	else if ((heading >= 67) && (heading <= 113))
	{
		return dz >= 0;
	}
	else if ((heading >= 113) && (heading <= 157))
	{
		return ((dz >= 0) && (dz >= dx)) || ((dx <= 0) && (dx <= dz));
	}
	else if ((heading >= 157) || (heading <= -157))
	{
		return dx <= 0;
	}
	else if ((heading >= -157) && (heading <= -113))
	{
		return ((dx <= 0) && (dx <= -dz)) || ((dz <= 0) && (dz <= -dx));
	}
	else if ((heading >= -113) && (heading <= -67))
	{
		return dz <= 0;
	}

	return ((dx >= 0) && (dx >= dz)) || ((dz <= 0) && (dz <= dx));
}

bool PlayerCharacter::isSuicidalGrenadeTarget( int dx, int dz, pickup::Type grenade )
{
	if (pickup::StunGrenade == grenade)
		return false;

	using namespace explosion;
	int min_range = getExplosionRange( getExplosionType( grenade ) )+1;

	return (dx*dx)+(dz*dz) < (min_range*min_range);
}


int PlayerCharacter::getWeight( const AHGameModel& model ) const
{
	const World& world( model.world() );
	int weight(0);

	if (world.objectExists( m_armour ))
	{
		const Armour& armour( dynamic_cast< const Armour& >( world.object(m_armour) ) );
		if (armour::hasWeightImmunity(armour.armourType()) && (armour.getHP()>0))
			return -1;
		weight += armour::getWeight(armour.armourType());
	}

	if (world.objectExists( m_sidearm ))
	{
		const Weapon& weapon( dynamic_cast< const Weapon& >( world.object(m_sidearm) ) );
		weight += weapon.weaponData().weight;
	}

	if (world.objectExists( m_primary ))
	{
		const Weapon& weapon( dynamic_cast< const Weapon& >( world.object(m_primary) ) );
		weight += weapon.weaponData().weight;
	}

	return weight;
}

int PlayerCharacter::getWeightPenalty( const AHGameModel& model, int weight ) const
{
	if (weight <= 0)
		if (model.world().objectExists(m_armour))
			return 0; //Powered armour. No weight penalties or bonuses.

	int threshold = hasPerk(player_perks::HigherWeightThreshold) ? 20000 : 15000;

	if (weight > threshold)
	{
		//Penalty 25% per 5000g over 15000g without perks.
		unsigned long penalty = 
			hasPerk(player_perks::HighWeightMoveSpeed2) ? 50L : 
				hasPerk(player_perks::HighWeightMoveSpeed1) ? 125L : 250L;
		long excess = (std::max)(0, weight-threshold);
		return ((excess / 5L) * penalty);
	}

	//Perks give up to 25% bonus per 5000g under 15000g, max bonus 50%.
	long bonus = 
		hasPerk(player_perks::LowWeightMoveSpeed2) ? 250L : 
			hasPerk(player_perks::LowWeightMoveSpeed1) ? 125L : 0L;
	long excess = weight-threshold;
	return (std::max)(-500000L, ((excess / 5L) * bonus));
}

RL_shared::GameTimeCoordinate PlayerCharacter::getActionTime( const AHGameModel& model, player_actions::Action action ) const
{
	GameTimeCoordinate time( getBaseActionTime( action ) );

	if ( player_actions::ReloadWeapon == action )
	{
		const World& world( model.world() );
		RL_shared::DBKeyValue weapon_key( weapon( currentWeapon() ) );
		if (world.objectExists( weapon_key ))
		{
			const Weapon& weapon_obj( dynamic_cast< const Weapon& >( world.object( weapon_key ) ) );
			time = weapon_obj.weaponData().reload_time;
		}
	}

	if (( player_actions::ReloadWeapon == action ) || ( player_actions::SwitchToSidearm == action ) || ( player_actions::SwitchToPrimary == action ))
		if ( hasPerk( player_perks::ReloadAndWeaponSwitchSpeed2 ) )
			time /= 2;
		else if ( hasPerk( player_perks::ReloadAndWeaponSwitchSpeed1 ) )
			time = (time*3)/4;

	if (weightAffectsAction(action))
	{
		time += ((long int)getWeightPenalty( model, getWeight(model) ) * (long int)time) / 1000000L;
	}
	return time;
}

bool PlayerCharacter::hasPerk( player_perks::Perk perk ) const
{
	using namespace player_perks;
	if (None == perk)
		return true; //to make it a little simpler to test for pre-requisites.
	return (0 != (m_perks & perkBitMask(perk)));
}

bool PlayerCharacter::canBuyPerk( player_perks::Perk perk ) const
{
	using namespace player_perks;
	if (hasPerk(perk))
		return false;
	int cost = perkXPCost(perk);
	if (cost > m_XP)
		return false;
	return hasPerk( perkRequires(perk) );
}

bool PlayerCharacter::buyPerk( player_perks::Perk perk )
{
	using namespace player_perks;

	if (!canBuyPerk(perk))
		return false;

	int cost = perkXPCost(perk);
	m_XP -= cost;
	m_spent_XP += cost;
	m_perks |= perkBitMask(perk);
	return true;
}

int PlayerCharacter::getOpenHearingRange(void) const
{
	if (hasPerk(player_perks::Hearing2))
		return 15;
	return 10;
}

int PlayerCharacter::getCeilingHearingRange(void) const
{
	if (hasPerk(player_perks::Hearing2))
		return 12;
	if (hasPerk(player_perks::Hearing1))
		return 6;
	return 3;
}

int PlayerCharacter::getHearingChance(void) const
{
	if (hasPerk(player_perks::Hearing2))
		return 5;
	if (hasPerk(player_perks::Hearing1))
		return 3;
	return 2;
}

void PlayerCharacter::interruptActions( AHGameModel& model )
{
	const Actions& my_actions( actions() );
	ActionEngine& action_engine( model.actionEngine() );

	BOOST_FOREACH( DBKeyValue action_key, my_actions )
	{
		boost::shared_ptr< BaseAction > action_obj( action_engine.getAction(action_key) );
		action_obj->interrupt(model);
	}
}

void PlayerCharacter::youCanSeeMe( AHGameModel& model, Alien& who, bool seen_before )
{
	if (who.alienType() == aliens::Worker)
		return; //not scared. 

	const Actions& my_actions( actions() );
	ActionEngine& action_engine( model.actionEngine() );

	boost::shared_ptr<IGameEvents> events( model.gameEventsObserver() );

	bool shown_msg = false;

	BOOST_FOREACH( DBKeyValue action_key, my_actions )
	{
		boost::shared_ptr< BaseAction > action_obj( action_engine.getAction(action_key) );
		if (action_obj->isLongAction(model))
		{
			if ((!shown_msg) && events)
			{
				events->playerSeesAlien(model, *this, who);
				shown_msg = true;
			}
			action_obj->interrupt(model);			
		}
	}

	//This doesn't actually tell the player when they can see an alien; 
	//rather, it tells them when the alien has seen them.
	//if (events)
	//	if (!(shown_msg || seen_before))
	//	{
	//		events->playerSeesAlien(model, *this, who);
	//	}
}

void PlayerCharacter::writeOutcome( AHGameModel& model, const char * outcome )
{
	if (m_stats)
	{
		const RL_shared::World& world( model.world() );

		m_stats->location() = location();
		m_stats->outcome() = outcome;
		m_stats->name() = m_name;

		if (world.objectExists(m_armour))
			m_stats->armour() = dynamic_cast< const AHGameObject& >( world.object(m_armour) ).getSelectName(true);
		else
			m_stats->armour() = "None";

		if (world.objectExists(m_primary))
			m_stats->primary() = dynamic_cast< const AHGameObject& >( world.object(m_primary) ).getSelectName(true);
		else
			m_stats->primary() = "None";

		if (world.objectExists(m_sidearm))
			m_stats->sidearm() = dynamic_cast< const AHGameObject& >( world.object(m_sidearm) ).getSelectName(true);
		else
			m_stats->sidearm() = "None";

		m_stats->perks() = m_perks;

		m_stats->XP() = m_XP;
		m_stats->spentXP() = m_spent_XP;
		m_stats->HP() = m_HP;
		m_stats->maxHP() = m_max_HP;

		for (int i=0; i<pickup::NumTypes; ++i)
			m_stats->pickups((pickup::Type)i) = m_pickups[i];
	}
}


}
