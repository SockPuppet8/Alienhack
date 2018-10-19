#ifndef ALIENHACK_EXPLOSIVE_HPP
#define	ALIENHACK_EXPLOSIVE_HPP


#include "AHGameObject.hpp"
#include <boost/shared_ptr.hpp>


namespace AlienHack
{


class AHGameModel;



class Explosive : public AHGameObject
{
public:

	Explosive( 
		RL_shared::GameTimeCoordinate fuse_time, 
		int direction_x, int direction_z
		);

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual int takeDamage( AHGameModel&, damage::Type type, int amount, int penetration );

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< AHGameObject >(*this);
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_dir_x;
		ar & m_dir_z;
	}

	//Default constructor intended only for serialization use.
	Explosive(void);

private:

	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	int m_dir_x;
	int m_dir_z;
};


}


#endif
