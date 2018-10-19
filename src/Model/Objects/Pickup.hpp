#ifndef ALIENHACK_PICKUP_HPP
#define	ALIENHACK_PICKUP_HPP


#include "AHGameObject.hpp"


namespace AlienHack
{

namespace pickup
{
	enum Type
	{
		None = -1,
		Medkit = 0,
		Neutraliser, 
		Ammo_9mm,
		Ammo_shotgun, 
		Ammo_colt, 
		Ammo_pulse,
		Ammo_flame,
		Ammo_smartgun,
		Ammo_laser,
		Ammo_HEDP30mm,
		StunGrenade,
		FragGrenade,
		IncGrenade,
		KrakGrenade,
		MotionTracker,
		CreditChip,
		DemoCharge, 
		MegaCredChip, 

		End,
		NumTypes = End
	};
}


class Pickup : public AHGameObject
{
public:

	explicit Pickup( pickup::Type );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool blocksDoor(void) const;

	virtual int takeDamage( AHGameModel&, damage::Type type, int amount, int penetration );
	virtual void acidSplash( AHGameModel&, int amount, int );

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	pickup::Type pickupType(void) const	{ return m_pickup_type; }

	void disappear(void);

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<AHGameObject>(*this);
		ar & m_pickup_type;
		ar & m_disappear;
	}

	//Default constructor intended only for serialization use.
	Pickup(void);

private:

	pickup::Type m_pickup_type;
	bool m_disappear;

};


}



#endif
