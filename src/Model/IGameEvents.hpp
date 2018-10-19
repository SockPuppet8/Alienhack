#ifndef ALIENHACK_I_GAME_EVENTS_HPP
#define	ALIENHACK_I_GAME_EVENTS_HPP


#include "WorldObjects/WorldObject.hpp"
#include "Objects/AHGameObject.hpp" //For damage::Type
#include "Objects/PlayerCharacter.hpp" //For WeaponSlot
#include "Objects/TerrainType.hpp"


namespace AlienHack
{


class AHGameModel;
class AHGameObject;
class Pickup;
class Armour;
class Weapon;
class Alien;
class Explosive;


class IGameEvents
{
public:

	virtual ~IGameEvents()
	{
	}

	virtual void message(const std::string& ) = 0;
	virtual void debugMessage(const std::string& ) = 0;

	virtual void playerMoved(AHGameModel&, RL_shared::WorldObject::WorldLocation newloc) = 0;

	virtual void playerFiredRound(AHGameModel&, PlayerCharacter& player, Weapon& weapon) = 0;
	virtual void playerFiredRoundFailed(AHGameModel&, PlayerCharacter& player, Weapon& weapon) = 0;
	virtual void playerPickedUp(AHGameModel&, PlayerCharacter& player, Pickup& object) = 0;
	virtual void playerCouldNotPickUp(AHGameModel&, PlayerCharacter& player, Pickup& object) = 0;
	virtual void playerTookAmmo(AHGameModel&, PlayerCharacter& player, Weapon& object, int amount) = 0;
	virtual void playerCouldNotTakeAmmo(AHGameModel&, PlayerCharacter& player, Weapon& object) = 0;
	virtual void playerWearsArmour(AHGameModel&, PlayerCharacter& player, Armour&) = 0;
	virtual void playerDropsArmour(AHGameModel&, PlayerCharacter& player, Armour&) = 0;
	virtual void playerUsesWeapon(AHGameModel&, PlayerCharacter& player, Weapon&) = 0;
	virtual void playerDropsWeapon(AHGameModel&, PlayerCharacter& player, Weapon&) = 0;
	virtual void playerDropsItem(AHGameModel&, PlayerCharacter& player, Pickup&) = 0;
	virtual void playerSwitchedWeapon(AHGameModel&, PlayerCharacter&, PlayerCharacter::WeaponSlot from, PlayerCharacter::WeaponSlot to) = 0;
	virtual void playerUsedNeutraliser(AHGameModel&, PlayerCharacter& player) = 0;
	virtual void playerUsedMedkit(AHGameModel&, PlayerCharacter& player) = 0;
	virtual void playerUsedMedkitPlus(AHGameModel&, PlayerCharacter& player) = 0;

	virtual void playerDodgedAttack(AHGameModel&, PlayerCharacter& player, RL_shared::DBKeyValue attacker) = 0;
	virtual void playerDodgedAcid(AHGameModel&, PlayerCharacter& player) = 0;
	virtual void playerFendedAttacker(AHGameModel&, PlayerCharacter& player, AHGameObject& attacker) = 0;

	virtual void playerHit(AHGameModel&, PlayerCharacter& player, damage::Type type, int amount ) = 0;
	virtual void playerArmourHit(AHGameModel&, PlayerCharacter& player, damage::Type type, int amount ) = 0;
	virtual void playerDies(AHGameModel&, PlayerCharacter& player) = 0;
	virtual void playerHuggered(AHGameModel&, PlayerCharacter& player) = 0;
	virtual void playerDodgedHugger(AHGameModel&, PlayerCharacter& player) = 0;

	virtual void playerSeesAlien(AHGameModel&, PlayerCharacter&, Alien&) = 0;
	virtual void playerActionInterrupted(AHGameModel&) = 0;

	virtual void downloadedMap(AHGameModel&) = 0;

	virtual void placedDemoCharge(AHGameModel&, int fuse_seconds) = 0;
	virtual void failedPlacingDemoCharge(AHGameModel&) = 0;
	virtual void demoChargeDetonates(AHGameModel&, Explosive&) = 0;

	virtual void doorOpened(AHGameModel&, const RL_shared::WorldObject::WorldLocation&) = 0;
	virtual void doorClosed(AHGameModel&, const RL_shared::WorldObject::WorldLocation&) = 0;
	virtual void doorBlocked(AHGameModel&, const AHGameObject&) = 0;
	virtual void doorRammed(AHGameModel&, const AHGameObject&, const RL_shared::WorldObject::WorldLocation&) = 0;
	virtual void doorRammedOpen(AHGameModel&, const AHGameObject&, const RL_shared::WorldObject::WorldLocation&) = 0;

	virtual void actorStunned(AHGameModel&, const AHGameObject&, RL_shared::GameTimeCoordinate time) = 0;

	virtual void alienMakesSoundNearby(AHGameModel&, const Alien&, int distance_squared) = 0;
	virtual void alienMakesSoundAbove(AHGameModel&, const Alien&) = 0;
	virtual void alienMakesSoundAbove(AHGameModel&) = 0;
	virtual void alienEnterVent(AHGameModel&, const Alien&, TerrainType) = 0;
	virtual void alienExitVent(AHGameModel&, const Alien&, TerrainType) = 0;
	virtual void alienHit(AHGameModel&, RL_shared::DBKeyValue hit_alien, damage::Type type, int amount ) = 0;
	virtual void alienDies(AHGameModel&, RL_shared::DBKeyValue hit_alien) = 0;
	virtual void queenScreech(AHGameModel&, Alien&) = 0;
	virtual void huggerAttack(AHGameModel&, Alien&) = 0;
	virtual void spitterAttackBegin(AHGameModel&, Alien&) = 0;
	virtual void spitterAttackSpit(AHGameModel&, Alien&) = 0;

	virtual void armourDestroyed(AHGameModel&, Armour&, damage::Type) = 0;

	virtual void acidSplashed(AHGameModel&, AHGameObject& on) = 0;
	virtual void acidTransferred(AHGameModel&, AHGameObject& from, AHGameObject& to) = 0;

	virtual void objectDestroyed(AHGameModel&, AHGameObject&) = 0;

	virtual void objectAttacksAnother(AHGameModel&, AHGameObject& attacker, AHGameObject& attackee, damage::Type) = 0;
};

}


#endif
