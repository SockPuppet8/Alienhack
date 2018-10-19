#ifndef ALIENHACK_LASER_TRAIL_HPP
#define	ALIENHACK_LASER_TRAIL_HPP


#include "AHGameObject.hpp"
#include <boost/shared_ptr.hpp>


namespace AlienHack
{


class AHGameModel;




class LaserTrail : public AHGameObject
{
public:

	LaserTrail( 
		AHGameModel& model, 
		int target_offs_x, int target_offs_z, 
		RL_shared::GameTimeCoordinate time 
		);

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool isFastMovingObject(void) const	{ return true; }

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	int targetX(void) const	{ return m_target_offs_x; }
	int targetZ(void) const	{ return m_target_offs_z; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<AHGameObject>(*this);
		ar & m_target_offs_x;
		ar & m_target_offs_z;
		ar & m_time_full;
		ar & m_time_remaining;
	}

	//Default constructor intended only for serialization use.
	LaserTrail(void);

private:

	int m_target_offs_x;
	int m_target_offs_z;

	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;

};


}


#endif
