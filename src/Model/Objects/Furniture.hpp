#ifndef ALIENHACK_FURNITURE_HPP
#define	ALIENHACK_FURNITURE_HPP


#include "AHGameObject.hpp"


namespace AlienHack
{


namespace furniture
{
	enum Type
	{
		None = -1,
		Crate = 0,	//Crates should always have the honour of first place.
		CrateStack, 
		Locker,
		Table,
		Chair,
		Desk,
		Sofa,
		Junk,
		Machinery,
		Piping,
		Bed,
		Workbench,
		Hopper,

		End,
		NumTypes = End
	};
}


class Furniture : public AHGameObject
{
public:

	Furniture( furniture::Type, bool broken );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool blocksDoor(void) const;
	virtual bool blocksVision(void) const;
	virtual bool blocksProjectiles(void) const;

	virtual int takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration );
	virtual void acidSplash( AHGameModel&, int amount, int max );

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	furniture::Type furnitureType(void) const	{ return m_furn_type; }

	bool isBroken(void) const	{ return m_broken; }

	virtual bool isInteresting(bool) const
	{
		return false;
	}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<AHGameObject>(*this);
		ar & m_furn_type;
		ar & m_broken;
	}

	//Default constructor intended only for serialization use.
	Furniture(void);

private:

	furniture::Type m_furn_type;
	bool m_broken;
};


}



#endif
