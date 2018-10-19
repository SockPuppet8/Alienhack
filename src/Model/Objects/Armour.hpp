#ifndef ALIENHACK_ARMOUR_HPP
#define	ALIENHACK_ARMOUR_HPP


#include "AHGameObject.hpp"
#include "AcidSplashable.hpp"


namespace AlienHack
{


class AHGameModel;


namespace armour
{
	enum Special
	{
		Weight = 1 << 15, 
		Value = 1 << 14, 
		HP = 1 << 13, 
		BaseField = 0xFF, 
		SpecialFlags = 0xFF00
	};
	enum Type
	{
		None = -1,
		FlakJacket = 0, 
		RiotArmour, 
		CombatArmour, 
		PlasteelArmour, 
		PowerArmour,
		ChemhazardSuit,
		ChemhazardArmour,
		AdvCombatArmour,

		End,
		NumTypes = End
	};

	int getArmourValue( Type type );
	int getArmourHP( Type type ); //Max HP
	bool hasAcidImmunity( Type type );
	bool hasWeightImmunity( Type type );
	int getWeight( Type type );
}


class Armour : public AHGameObject, public AcidSplashable
{
public:

	Armour( armour::Type, int condition_percent );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual int takeDamage( AHGameModel&, damage::Type type, int amount, int penetration );

	virtual bool blocksDoor(void) const;

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	virtual void acidSplash( AHGameModel&, int amount, int );

	armour::Type armourType(void) const	{ return m_armour_type; }

	int getHP(void) const	{ return m_HP; }

	void setWearer(RL_shared::DBKeyValue wearer)
	{
		m_wearer = wearer;
	}
	RL_shared::DBKeyValue wearer(void) const	{ return m_wearer; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< AHGameObject >(*this);
		ar & boost::serialization::base_object< AcidSplashable >(*this);
		ar & m_armour_type;
		ar & m_wearer;
		ar & m_HP;
	}

	//Default constructor intended only for serialization use.
	Armour(void);

private:

	armour::Type m_armour_type;

	RL_shared::DBKeyValue m_wearer;

	int m_HP;

};


}



#endif
