#ifndef ALIENHACK_EGG_HPP
#define	ALIENHACK_EGG_HPP


#include "AHGameObject.hpp"
#include <boost/cstdint.hpp>


namespace AlienHack
{


class Egg : public AHGameObject
{
public:

	Egg(void);

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool blocksDoor(void) const;

	virtual int takeDamage( AHGameModel&, damage::Type type, int amount, int penetration );
	//virtual void acidSplash( AHGameModel&, int amount, int );

	virtual bool isInteresting(bool) const
	{
		return true;
	}

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< AHGameObject >(*this);
		ar & m_timer;
		ar & m_HP;
	}

private:

	RL_shared::GameTimeCoordinate m_timer;
	boost::int8_t m_HP;

	void hatch( AHGameModel& );
};


}



#endif
