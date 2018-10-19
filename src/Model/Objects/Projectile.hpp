#ifndef ALIENHACK_PROJECTILE_HPP
#define	ALIENHACK_PROJECTILE_HPP


#include "AHGameObject.hpp"
//#include "assert.hpp"
#include <boost/shared_ptr.hpp>


namespace AlienHack
{


class AHGameModel;


namespace projectile
{
	enum Type
	{
		None = 0,
		Bullet,
		Shot,
		Flame,
		Laser,
		Grenade,
		Impact
	};
}

namespace impact
{
	enum Type
	{
		None = 0, 
		Puff, 
		Fire,
		Laser, 
		AlienBlood
	};
}

class IExecuteImpactAction
{
public:

	virtual ~IExecuteImpactAction()
	{
	}

	virtual void execute(void) = 0;
};


//Animation for a projectile. Where it goes and what it 'hits' is predetermined.
class Projectile : public AHGameObject
{
public:

	Projectile(
		AHGameModel& model, 
		projectile::Type, RL_shared::DBKeyValue zone, 
		int start_x, int start_z, 
		int finish_x, int finish_z, 
		RL_shared::GameTimeCoordinate time, 
		RL_shared::GameTimeCoordinate delay, 
		impact::Type impact, 
		boost::shared_ptr< IExecuteImpactAction >
		);

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool isFastMovingObject(void) const	{ return true; }

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	projectile::Type projectileType(void) const	{ return m_projectile_type; }
	impact::Type impactType(void) const			{ return m_impact_type; }

	RL_shared::GameTimeCoordinate delay(void) const	{ return m_delay; }

	int startX(void) const	{ return m_start_x; }
	int startZ(void) const	{ return m_start_z; }
	int finishX(void) const	{ return m_finish_x; }
	int finishZ(void) const	{ return m_finish_z; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<AHGameObject>(*this);
		ar & m_action;
		ar & m_projectile_type;
		ar & m_impact_type;
		ar & m_zone;
		ar & m_start_x;
		ar & m_start_z;
		ar & m_finish_x;
		ar & m_finish_z;
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_delay;
	}

	//Default constructor intended only for serialization use.
	Projectile(void);

private:

	boost::shared_ptr< IExecuteImpactAction > m_action;

	projectile::Type m_projectile_type;
	impact::Type m_impact_type;

	RL_shared::DBKeyValue m_zone;
	int m_start_x;
	int m_start_z;
	int m_finish_x;
	int m_finish_z;

	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	RL_shared::GameTimeCoordinate m_delay;
};


}


#endif
