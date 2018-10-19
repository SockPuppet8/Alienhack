#ifndef ALIENHACK_FIRE_HPP
#define	ALIENHACK_FIRE_HPP


#include "AHGameObject.hpp"
#include <boost/shared_ptr.hpp>


namespace AlienHack
{


class AHGameModel;



class Fire : public AHGameObject
{
public:

	Fire( 
		RL_shared::GameTimeCoordinate time, 
		RL_shared::GameTimeCoordinate smoke_time, 
		int damage );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool blocksVision(void) const;

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	bool isSmoke(void) const	{ return m_time_remaining <= m_smoke_time; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< AHGameObject >(*this);
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_smoke_time;
		ar & m_damage;
	}

	//Default constructor intended only for serialization use.
	Fire(void);

private:

	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	RL_shared::GameTimeCoordinate m_smoke_time;
	int m_damage;

};


}


#endif
