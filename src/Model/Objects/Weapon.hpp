#ifndef ALIENHACK_WEAPON_HPP
#define	ALIENHACK_WEAPON_HPP


#include "AHGameObject.hpp"
#include "Pickup.hpp"


namespace AlienHack
{


class AHGameModel;
struct WeaponData;
struct SplashDamage;


namespace weapon
{
	enum Type
	{
		Type_None = -1,
		Pistol = 0, 
		SMG,
		PulseRifle,
		SawnoffShotgun,
		CombatShotgun,
		Colt,
		Flamer,
		SmartGun,
		Laser,
		CanisterRifle,
		MachinePistol,

		End,
		NumTypes = End
	};

	enum Class
	{
		Class_None = 0,
		Sidearm,
		Primary
	};

	enum Special
	{
		Special_None = 0,
		Clip = 1, 
		Weight = 1<<1, 
		Damage = 1<<2, 
		Accuracy = 1<<3, 
		Rate = 1<<4, 
		Reload = 1<<5, 
		Special_All = 0x3F
	};

	int getClipSize( Type, Special );
	Class getClass( Type, Special );
	pickup::Type getAmmoType( Type );
	int getWeight( Type type, Special );
}


class Weapon : public AHGameObject
{
public:

	Weapon( weapon::Type, weapon::Special, int num_bullets );

	const WeaponData& weaponData(void) const;
	const SplashDamage& splashDamage(void) const;

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool blocksDoor(void) const;

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;
	virtual std::string getStatusString(void) const;

	virtual void acidSplash( AHGameModel& model, int amount, int max );
	virtual int takeDamage( AHGameModel&, damage::Type type, int amount, int penetration );

	weapon::Type weaponType(void) const	{ return m_weapon_type; }

	int getAmmo(void) const		{ return m_ammo; }
	void setAmmo(int ammo);
	int decrementAmmo(int amount);

	void setWielder(RL_shared::DBKeyValue wielder)
	{
		m_wielder = wielder;
	}
	RL_shared::DBKeyValue wielder(void) const	{ return m_wielder; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<AHGameObject>(*this);
		ar & m_weapon_type;
		ar & m_special;
		ar & m_wielder;
		ar & m_ammo;
		ar & m_disappear;
	}

	//Default constructor intended only for serialization use.
	Weapon(void);

private:

	weapon::Type m_weapon_type;
	weapon::Special m_special;

	RL_shared::DBKeyValue m_wielder;

	int m_ammo;
	bool m_disappear;
};


}



#endif
