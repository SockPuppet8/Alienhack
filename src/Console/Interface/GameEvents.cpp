#include "GameEvents.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "../../Model/Objects/Alien.hpp"
#include "../../Model/Objects/Pickup.hpp"
#include "../../Model/Objects/Armour.hpp"
#include "../../Model/Objects/Weapon.hpp"
#include "../../Model/Objects/ObjectType.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include "Messages/Console/MessageDisplay.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>




namespace AlienHack
{


using namespace RL_shared;


class GameEvents : public IGameEvents
{
	RL_shared::MessageDisplay& m_msgs;

public:

	explicit GameEvents( RL_shared::MessageDisplay& msgs )
		: m_msgs( msgs )
	{
	}

	virtual void message(const std::string& text)
	{
		m_msgs.addString(text.c_str());
	}
	virtual void debugMessage(const std::string& text)
	{
		m_msgs.addString(text.c_str());
	}

	virtual void playerMoved(AHGameModel& model, RL_shared::WorldObject::WorldLocation newloc)
	{
		const RL_shared::World& world( model.world() );
		if (!world.zoneExists(newloc.zone))
			return;
		const RL_shared::Zone& zone( world.zone(newloc.zone) );
		if (!zone.isWithin(newloc.x, newloc.z))
			return;
		const RL_shared::ObjectList& objects( zone.objectsAt( newloc.x, newloc.z ) );
		BOOST_FOREACH( RL_shared::DBKeyValue obj_key, objects )
		{
			if (world.objectExists( obj_key ))
			{
				const AHGameObject& game_obj( dynamic_cast<const AHGameObject&>( world.object( obj_key ) ) );
				std::string obj_string( game_obj.getThereIsString(true) );

				if (game_obj.shouldDraw() && (!obj_string.empty()))
				{
					std::string there_is_str("There is " + obj_string + " here.");
					m_msgs.addString( there_is_str.c_str() );
				}
			}
		}
	}

	virtual void playerFiredRound(AHGameModel&, PlayerCharacter& player, Weapon& weapon)
	{
	}
	virtual void playerFiredRoundFailed(AHGameModel&, PlayerCharacter& player, Weapon& weapon)
	{
	}
	virtual void playerPickedUp(AHGameModel&, PlayerCharacter& /*player*/, Pickup& object)
	{
		std::string yes_str("You picked up " + std::string(object.getThereIsString(false)));
		if (object.pickupType() == pickup::CreditChip)
		{
			yes_str += " (+25 XP).";
		}
		else if (object.pickupType() == pickup::MegaCredChip)
		{
			yes_str += " (+100 XP).";
		}
		else
		{
			yes_str += ".";
		}
		m_msgs.addString(yes_str.c_str());
	}
	virtual void playerCouldNotPickUp(AHGameModel&, PlayerCharacter& /*player*/, Pickup& object)
	{
		std::string no_str("You can't carry any more of " + std::string(object.getSelectName(false)) + "!");
		m_msgs.addString(no_str.c_str());
	}
	virtual void playerCouldNotTakeAmmo(AHGameModel& model, PlayerCharacter& /*player*/, Weapon& object)
	{
		std::string msg_str("You couldn't take any ammo from " + object.getThereIsString(false) + ".");
		m_msgs.addString(msg_str.c_str());
	}
	virtual void playerTookAmmo(AHGameModel& model, PlayerCharacter& /*player*/, Weapon& object, int amount)
	{
		if (amount > 0)
		{
			std::string msg_str("You take " + boost::lexical_cast<std::string>(amount) + " ammo from " + object.getThereIsString(false) + ".");
			m_msgs.addString(msg_str.c_str());
		}
	}
	virtual void playerWearsArmour(AHGameModel&, PlayerCharacter& /*player*/, Armour& armour)
	{
		std::string str("You put on ");
		str += armour.getThereIsString(true);
		str += ".";
		m_msgs.addString( str.c_str() );
	}
	virtual void playerDropsArmour(AHGameModel&, PlayerCharacter& /*player*/, Armour& armour)
	{
		std::string str("You drop ");
		str += armour.getThereIsString(true);
		str += ".";
		m_msgs.addString( str.c_str() );
	}
	virtual void playerUsesWeapon(AHGameModel&, PlayerCharacter& player, Weapon& weapon)
	{
		std::string str("You pick up ");
		str += weapon.getThereIsString(false);
		str += ".";
		m_msgs.addString( str.c_str() );
	}
	virtual void playerDropsWeapon(AHGameModel&, PlayerCharacter& player, Weapon& weapon)
	{
		std::string str("You drop ");
		str += weapon.getThereIsString(false);
		str += ".";
		m_msgs.addString( str.c_str() );
	}
	virtual void playerDropsItem(AHGameModel&, PlayerCharacter& player, Pickup& item)
	{
		std::string str("You drop ");
		str += item.getThereIsString(false);
		str += ".";
		m_msgs.addString( str.c_str() );
	}
	virtual void playerSwitchedWeapon(AHGameModel& model, PlayerCharacter& player, PlayerCharacter::WeaponSlot from, PlayerCharacter::WeaponSlot to)
	{
		const RL_shared::World& world( model.world() );
		bool has_sidearm(world.objectExists(player.weapon( PlayerCharacter::Sidearm )));
		bool has_primary(world.objectExists(player.weapon( PlayerCharacter::Primary )));

		switch(from)
		{
			case PlayerCharacter::Sidearm:
			{
				if (has_sidearm)
					m_msgs.addString("You holster your sidearm."); 
				break;
			}
			case PlayerCharacter::Primary:
			{
				if (has_primary)
					m_msgs.addString("You let go of your primary weapon."); 
				break;
			}
			default:;
		}
		switch(to)
		{
			case PlayerCharacter::Sidearm:
			{
				if (has_sidearm)
					m_msgs.addString("You grab hold of your sidearm."); 
				break;
			}
			case PlayerCharacter::Primary:
			{
				if (has_primary)
					m_msgs.addString("You raise your primary weapon."); 
				break;
			}
			default:;
		}
	}
	virtual void playerUsedNeutraliser(AHGameModel&, PlayerCharacter& /*player*/)
	{
		m_msgs.addString("You use the molecular neutraliser. The acid stops reacting.");
	}
	virtual void playerUsedMedkit(AHGameModel&, PlayerCharacter& /*player*/)
	{
		m_msgs.addString("You shoot up the adrenamorph from the medkit. You feel ready to keep fighting!");
	}
	virtual void playerUsedMedkitPlus(AHGameModel&, PlayerCharacter& /*player*/)
	{
		m_msgs.addString("You use the medkit to patch up some of your wounds.");
	}

	virtual void playerDodgedAttack(AHGameModel&, PlayerCharacter& /*player*/, DBKeyValue /*attacker*/)
	{
		m_msgs.addString("You dodge the attack!");
	}
	virtual void playerDodgedAcid(AHGameModel&, PlayerCharacter& /*player*/)
	{
		m_msgs.addString("You manage to avoid the acid!");
	}
	virtual void playerFendedAttacker(AHGameModel& model, PlayerCharacter& player, AHGameObject& attacker)
	{
		int enemy_HP = 20;
		try
		{
			enemy_HP = (dynamic_cast<const Alien&>(attacker)).getHP();
		} catch(...)
		{
		}
		if ((-1 == player.getWeight(model)) //power armour!
			|| ( (rand() & 15) < (player.getHP() - enemy_HP) )
			)
		{
			m_msgs.addString( (std::string("You ") + ((rand() & 3) ? "knock" : "shove") + " the " + attacker.getSelectName(false) + " away!").c_str() );
			return;
		}
		m_msgs.addString( ("You slip away from the " + attacker.getSelectName(false) + "!").c_str() );
	}

	virtual void playerHit(AHGameModel&, PlayerCharacter& player, damage::Type type, int amount )
	{
		std::string msg("You take ");
		msg += boost::lexical_cast<std::string>(amount);
		msg += " damage!";
		m_msgs.addString(msg.c_str());
	}

	virtual void playerArmourHit(AHGameModel&, PlayerCharacter& player, damage::Type type, int amount )
	{
		std::string msg("Your armour takes ");
		msg += boost::lexical_cast<std::string>(amount);
		msg += " damage!";
		m_msgs.addString(msg.c_str());
	}

	virtual void playerDies(AHGameModel&, PlayerCharacter& player)
	{
		m_msgs.addString("You die...");
	}

	virtual void playerHuggered(AHGameModel&, PlayerCharacter&)
	{
		m_msgs.addString("The facehugger crawls onto your face! You choke on something slimy. You lose consciousness.");
	}
	virtual void playerDodgedHugger(AHGameModel&, PlayerCharacter&)
	{
		m_msgs.addString("You knock it away from you!");
	}

	virtual void playerSeesAlien(AHGameModel&, PlayerCharacter&, Alien& alien)
	{
		std::string msg( "You see a vicious alien " );
		msg += alien.getSelectName(false);
		msg += "!";
		m_msgs.addString(msg.c_str());
	}

	virtual void playerActionInterrupted(AHGameModel&)
	{
		m_msgs.addString("Your action is interrupted!");
	}

	virtual void downloadedMap(AHGameModel&)
	{
		m_msgs.addString("Map downloaded.");
	}

	virtual void placedDemoCharge(AHGameModel&, int fuse_seconds)
	{
		std::string msg;
		msg += "You place the demolition charge. It will detonate in ";
		msg += boost::lexical_cast<std::string>(fuse_seconds);
		msg += " seconds. You had better get clear.";
		m_msgs.addString(msg.c_str());
	}
	virtual void failedPlacingDemoCharge(AHGameModel&)
	{
		m_msgs.addString("You couldn't place the demolition charge.");
	}
	virtual void demoChargeDetonates(AHGameModel&, Explosive&)
	{
		m_msgs.addString("The demolition charge detonates!");
	}

	virtual void doorOpened(AHGameModel&, const RL_shared::WorldObject::WorldLocation&)
	{
		m_msgs.addString("The door opens.");
	}
	virtual void doorClosed(AHGameModel&, const RL_shared::WorldObject::WorldLocation&)
	{
		m_msgs.addString("The door closes.");
	}
	virtual void doorBlocked(AHGameModel&, const AHGameObject&)
	{
		m_msgs.addString("The door is blocked!");
	}
	virtual void doorRammed(AHGameModel&, const AHGameObject&, const RL_shared::WorldObject::WorldLocation&)
	{
		m_msgs.addString("You hear the mighty thump of a door being rammed!");
	}
	virtual void doorRammedOpen(AHGameModel&, const AHGameObject&, const RL_shared::WorldObject::WorldLocation&)
	{
		m_msgs.addString("You hear a shuddering crash as a door is breached!");
	}

	virtual void actorStunned(AHGameModel& model, const AHGameObject& actor, GameTimeCoordinate time)
	{
		m_msgs.addString( ("The " + actor.getSelectName(false) + " is " + (time <= 1000 ? "briefly " : "") + "stunned!").c_str() );
	}

	virtual void alienMakesSoundNearby(AHGameModel&, const Alien& alien, int distance_squared)
	{
		if (aliens::isLargeAlien(alien.alienType()))
		{
			if (distance_squared <= 9)
				m_msgs.addString("You hear something large moving right next to you!");
			else
				m_msgs.addString("You hear something large moving nearby...");
		}
		else
		{
			if (distance_squared <= 9)
				m_msgs.addString("You hear something right next to you!");
			else
				m_msgs.addString("You hear a noise nearby.");
		}
	}
	virtual void alienMakesSoundAbove(AHGameModel&, const Alien& alien)
	{
		m_msgs.addString("You hear something moving in the ceiling.");
	}
	virtual void alienMakesSoundAbove(AHGameModel&)
	{
		m_msgs.addString("You hear something moving in the ceiling.");
	}
	virtual void alienEnterVent(AHGameModel&, const Alien&, TerrainType type)
	{
		if (terrain::Vent == type)
			m_msgs.addString("You see an alien disappear into a ventilation shaft.");
		else if (terrain::Hole == type)
			m_msgs.addString("You see an alien disappear into a hole in the ceiling.");
	}
	virtual void alienExitVent(AHGameModel&, const Alien&, TerrainType type)
	{
		if (terrain::Vent == type)
			m_msgs.addString("An alien crawls out of a ventilation shaft!");
		else if (terrain::Hole == type)
			m_msgs.addString("An alien crawls out of a hole in the ceiling!");
	}

	virtual void alienHit(AHGameModel& model, RL_shared::DBKeyValue hit_alien, damage::Type type, int amount )
	{
		using namespace RL_shared;
		std::string alien_str("alien");
		const World& world( model.world() );
		if (world.objectExists(hit_alien))
		{
			const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( world.object(hit_alien) ) );
			alien_str = game_obj.getSelectName(false);
		}
#if 1
		std::string hit_msg;
		hit_msg += "The " + alien_str + " takes " + boost::lexical_cast<std::string>(amount) + " damage!";
		m_msgs.addString( hit_msg.c_str() );
#else
		static const char * hits[7] = {"barely scratch", "scratch", "barely hit", "lightly hit", "hit", "squarely hit", "heavily hit"};
		std::string hit_msg("You ");
		int amount_idx = (std::max)(0, (std::min)(6, amount));
		hit_msg += hits[amount_idx];
		hit_msg += " the ";
		hit_msg += alien_str;
		hit_msg += " (";
		hit_msg += boost::lexical_cast<std::string>(amount);
		hit_msg += ")!";
		m_msgs.addString( hit_msg.c_str() );
#endif
	}

	virtual void alienDies(AHGameModel& model, RL_shared::DBKeyValue hit_alien)
	{
		using namespace RL_shared;
		std::string alien_str("alien");
		const World& world( model.world() );
		if (world.objectExists(hit_alien))
		{
			const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( world.object(hit_alien) ) );
			alien_str = game_obj.getSelectName(false);
		}
		std::string msg_str;
		msg_str += "The ";
		msg_str += alien_str;
		msg_str += " dies.";
		m_msgs.addString(msg_str.c_str());
	}

	virtual void queenScreech(AHGameModel&, Alien&)
	{
		m_msgs.addString("The queen lets out a hideous screeching cry!");
	}

	virtual void huggerAttack(AHGameModel&, Alien&)
	{
		m_msgs.addString("The facehugger leaps at you!");
	}

	virtual void spitterAttackBegin(AHGameModel&, Alien& alien)
	{
		std::string msg("The ");
		msg += alien.getSelectName(false);
		msg += " rears back!";
		m_msgs.addString(msg.c_str());
	}
	virtual void spitterAttackSpit(AHGameModel&, Alien& alien)
	{
		std::string msg("The ");
		msg += alien.getSelectName(false);
		msg += " spits acid!";
		m_msgs.addString(msg.c_str());
	}

	virtual void armourDestroyed(AHGameModel& model, Armour& armour, damage::Type type)
	{
		if (damage::Acid == type)
		{
			if (armour.wearer() == model.avatar())
			{
				if (armour::PowerArmour == (armour.armourType() & armour::BaseField))
					m_msgs.addString("Your armour is junked!");
				else
					m_msgs.addString("Your armour disintegrates!");
			}
			else
			{
				RL_shared::WorldObject::WorldLocation loc(armour.location());
				if (model.isVisible(loc.zone, loc.x, loc.z))
					m_msgs.addString("The armour disintegrates.");
			}
		}
		else
		{
			if (armour.wearer() == model.avatar())
				m_msgs.addString("Your armour breaks!");
			else
			{
				RL_shared::WorldObject::WorldLocation loc(armour.location());
				if (model.isVisible(loc.zone, loc.x, loc.z))
					m_msgs.addString("The armour breaks.");
			}
		}
	}

	virtual void acidSplashed(AHGameModel& model, AHGameObject& on)
	{
		std::string msg;
		if (on.key() == model.avatar())
			msg += "You are";
		else
			msg += "The " + on.getSelectName(false) + " is";

		msg += " splashed with acid!";
		m_msgs.addString( msg.c_str() );
	}
	virtual void acidTransferred(AHGameModel& model, AHGameObject& from, AHGameObject& to)
	{
		m_msgs.addString("The acid is now on you!");
	}

	virtual void objectDestroyed(AHGameModel& model, AHGameObject& obj)
	{
		std::string msg;
		msg += "The " + obj.getSelectName(false) + " is destroyed.";
		m_msgs.addString( msg.c_str() );
	}

	virtual void objectAttacksAnother(AHGameModel& model, AHGameObject& attacker, AHGameObject& attackee, damage::Type type)
	{
		std::string msg("The ");
		msg += attacker.getSelectName(false);
		if ((type == damage::Melee) && (attacker.type() == objects::Alien))
			msg += " slashes";
		else
			msg += " attacks";
		if (attackee.type() == objects::Player)
			msg += " you!";
		else
		{
			msg += " the ";
			msg += attackee.getSelectName(false);
			msg += "!";
		}
		m_msgs.addString( msg.c_str() );
	}
};



boost::shared_ptr< IGameEvents > makeGameEvents( RL_shared::MessageDisplay& msgs )
{
	boost::shared_ptr< IGameEvents > events( new GameEvents(msgs) );
	return events;
}


}

