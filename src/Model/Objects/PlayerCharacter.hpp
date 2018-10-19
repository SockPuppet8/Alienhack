#ifndef ALIENHACK_PLAYER_CHARACTER_HPP
#define	ALIENHACK_PLAYER_CHARACTER_HPP


#include "AHGameObject.hpp"
#include "Pickup.hpp"
#include "Weapon.hpp"
#include "AcidSplashable.hpp"
#include <boost/cstdint.hpp>


namespace AlienHack
{


namespace player_actions
{
	enum Action
	{
		None = 0,
		OpenDoor,
		CloseDoor,
		Move,
		MoveDiagonal,
		Turn,
		Wait,
		PickupObject,
		DropArmour,
		PickupArmour,
		DropWeapon,
		PickupWeapon,
		SwitchToSidearm,
		SwitchToPrimary,
		ReloadWeapon,
		UseNeutraliser,
		UseMedkit,
		ThrowGrenade,
		UseTerminal,
		TakeAmmo,
		SetCharge,
		UseMedkitPlus,
		DropItem
	};
}

namespace player_perks
{
	enum Perk
	{
		None = 0,
		First = 1,
		SingleShotAccuracy = First,		
		MultiShotAccuracy1,	
		MultiShotAccuracy2,	
		GrenadeAccuracy1, 
		GrenadeAccuracy2, 
		SingleShotCriticalBonus1, 
		SingleShotCriticalBonus2, 
		LowWeightMoveSpeed1, 
		LowWeightMoveSpeed2, 
		HighWeightMoveSpeed1, 
		HighWeightMoveSpeed2, 
		ReloadAndWeaponSwitchSpeed1, 
		ReloadAndWeaponSwitchSpeed2, 
		ItemCapacity1, 
		ItemCapacity2, 
		AmmoCapacity, 
		Dodge1,	
		Dodge2,	
		DarkSightRange,	
		FlameDamage, 
		FlameBoost, 
		Hearing1, 
		Hearing2, 
		AgileStunAttacker, 
		LoseLessMaxHP, 
		ArmourBonus1, 
		ArmourBonus2, 
		HigherWeightThreshold, 

		//Max 32 here without increasing the size of the bitfield

		NumPerks
	};

	const char * perkName( Perk );
	const char * perkDescription( Perk );
	Perk perkRequires( Perk );
	int perkXPCost( Perk );
	boost::uint32_t perkBitMask( Perk );
}


class Armour;
class Alien;


class PlayerCharacter : public AHGameObject, public AcidSplashable
{
public:

	explicit PlayerCharacter( AHGameModel& model );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual bool removeMe(AHGameModel&) const;

	virtual bool blocksDoor(void) const;

private:
	virtual int takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration );
public:
	virtual int takeDamage( AHGameModel& model, RL_shared::DBKeyValue attacker, damage::Type type, int amount, int penetration );

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	virtual boost::shared_ptr< PlayerStats > stats(void) const	{ return m_stats; }
	void setStats( boost::shared_ptr< PlayerStats > stats )		{ m_stats = stats; }

	virtual void acidSplash( AHGameModel&, int amount, int max );
	virtual void neutraliseAcid( AHGameModel& );
	bool isAcidSplashed( const AHGameModel& model ) const;

	//degrees, anticlockwise from east
	int getHeading(void) const	{ return m_heading; }
	void setHeading(int h)		{ m_heading = h; }

	int num( pickup::Type type ) const;
	int max( pickup::Type type ) const;
	void dec( pickup::Type type, bool used = true );
	bool canPickUp( pickup::Type type );
	bool canUseItem( pickup::Type type );

	enum WeaponSlot
	{
		WeaponSlot_None = -1,
		Sidearm = 0,
		Primary,
	};

	WeaponSlot currentWeapon(void) const	{ return m_held_weapon; }

	RL_shared::DBKeyValue armour(void) const	{ return m_armour; }
	RL_shared::DBKeyValue sidearm(void) const	{ return m_sidearm; }
	RL_shared::DBKeyValue primary(void) const	{ return m_primary; }
	RL_shared::DBKeyValue weapon( WeaponSlot slot ) const
	{
		switch(slot)
		{
			case Sidearm: return sidearm();
			case Primary: return primary();
			default:;
		}
		return RL_shared::INVALID_KEY;
	}

	int getXP(void) const		{ return m_XP; }
	int getSpentXP(void) const	{ return m_spent_XP; }
	int getHP(void) const		{ return m_HP; }
	int getMaxHP(void) const	{ return m_max_HP; }
	int getTopHP(void) const	{ return 20; }

	int getArmourHP( const AHGameModel& ) const;
	int getArmourMaxHP( const AHGameModel& ) const;

	weapon::Type getHeldWeaponType( const AHGameModel& ) const;
	int getHeldWeaponAmmoLoaded( const AHGameModel& ) const;
	int getHeldWeaponClipSize( const AHGameModel& ) const;
	int getHeldWeaponAmmoStock( const AHGameModel& ) const;
	int getHeldWeaponAmmoStockMax( const AHGameModel& ) const;

	int getWeight( const AHGameModel& ) const;
	int getWeightPenalty( const AHGameModel&, int weight ) const; //returns fraction * 1000000

	void addXP( int add )
	{
		m_XP += add;
	}

	bool hasMotionDetection( const AHGameModel& ) const;

	bool pickUp( Pickup& );
	bool addPickup( pickup::Type, int num );

	bool useItem( AHGameModel&, pickup::Type type );

	void wearArmour( AHGameModel&, Armour& );
	void dropArmour( AHGameModel& );

	void useWeapon( AHGameModel&, Weapon&, WeaponSlot );
	void dropWeapon( AHGameModel&, WeaponSlot );
	int takeAmmo( AHGameModel&, Weapon&, bool specific_action );
	void setCurrentWeapon(AHGameModel&, WeaponSlot slot);
	void reloadWeapon(AHGameModel&);

	RL_shared::GameTimeCoordinate getActionTime( const AHGameModel&, player_actions::Action ) const;

	bool isValidTargetOffset( int x, int z );
	bool isSuicidalGrenadeTarget( int x, int z, pickup::Type );

	const std::string& name(void) const
	{
		return m_name;
	}
	void setName( const std::string& name )
	{
		m_name = name;
	}

	bool hasPerk( player_perks::Perk ) const;
	bool canBuyPerk( player_perks::Perk ) const;
	bool buyPerk( player_perks::Perk );

	int getOpenHearingRange(void) const;
	int getCeilingHearingRange(void) const;
	int getHearingChance(void) const;

	void huggerAttack( AHGameModel& model, Alien& attacker );

	void youCanSeeMe( AHGameModel& model, Alien&, bool );

	void writeOutcome( AHGameModel& model, const char * );

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<AHGameObject>(*this);
		ar & boost::serialization::base_object<AcidSplashable>(*this);
		ar & m_stats;
		ar & m_name;
		ar & m_armour;
		ar & m_sidearm;
		ar & m_primary;
		ar & m_held_weapon;
		ar & m_XP;
		ar & m_spent_XP;
		ar & m_perks;
		ar & m_heading;
		ar & m_HP;
		ar & m_max_HP;
		ar & m_pickups;
	}

	//Default constructor purely for serialization use.
	PlayerCharacter(void);

private:

	boost::shared_ptr< PlayerStats > m_stats;

	std::string m_name;

	RL_shared::DBKeyValue m_armour;
	RL_shared::DBKeyValue m_sidearm;
	RL_shared::DBKeyValue m_primary;

	WeaponSlot m_held_weapon;

	int m_XP;
	int m_spent_XP;

	boost::uint32_t m_perks;

	boost::int16_t m_heading;

	boost::int16_t m_HP;
	boost::int16_t m_max_HP;

	boost::uint16_t m_pickups[ pickup::NumTypes ];

	void setWeapon( WeaponSlot slot, RL_shared::DBKeyValue key )
	{
		switch(slot)
		{
			case Sidearm:	m_sidearm = key; break;
			case Primary:	m_primary = key; break;
			default:;
		}
	}

	void interruptActions( AHGameModel& );
	int getDodgeChance(const AHGameModel&) const;
};


}


#endif
