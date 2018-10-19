#ifndef ALIENHACK_EXPLOSION_HPP
#define	ALIENHACK_EXPLOSION_HPP


//#include "AHGameObject.hpp"
#include "Pickup.hpp"
#include <boost/shared_ptr.hpp>


namespace AlienHack
{


class AHGameModel;


namespace explosion
{
	enum Type
	{
		None = -1,
		FragBlast = 0, 
		KrakBlast,
		IncBlast, 
		StunFlash,
		BarrelBlast, 
		DemoBlast, 

		End,
		NumTypes = End
	};

	void makeExplosion(
		AHGameModel& model, 
		RL_shared::DBKeyValue caused_by, 
		RL_shared::WorldObject::WorldLocation hit_loc, 
		Type type, 
		bool flame_special_1=false, 
		bool flame_special_2=false 
		);

	void makeDemoExplosion(
		AHGameModel& model, 
		RL_shared::DBKeyValue caused_by, 
		RL_shared::WorldObject::WorldLocation loc, 
		int dx, int dz
		);

	int getExplosionRange(Type type);

	explosion::Type getExplosionType( pickup::Type type );
}



class Explosion : public AHGameObject
{
public:

	Explosion(
		AHGameModel& model, 
		explosion::Type
		);

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool isFastMovingObject(void) const	{ return true; }

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	explosion::Type explosionType(void) const	{ return m_explosion_type; }

	RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< AHGameObject >(*this);
		ar & m_explosion_type;
		ar & m_time_full;
		ar & m_time_remaining;
	}

	//Default constructor intended only for serialization use.
	Explosion(void);

private:

	explosion::Type m_explosion_type;

	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;

};


}


#endif
