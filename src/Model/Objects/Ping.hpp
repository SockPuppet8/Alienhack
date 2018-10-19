#ifndef ALIENHACK_PING_HPP
#define	ALIENHACK_PING_HPP


#include "AHGameObject.hpp"
#include <boost/shared_ptr.hpp>


namespace AlienHack
{


namespace ping
{
	enum Type
	{
		None, 
		Motion, 
		Noise,
		Danger
	};
}


class AHGameModel;



class Ping : public AHGameObject
{
public:

	Ping( ping::Type type, RL_shared::GameTimeCoordinate time );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }
	RL_shared::GameTimeCoordinate timeFull(void) const		{ return m_time_full; }

	ping::Type pingType(void) const	{ return m_ping_type; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<AHGameObject>(*this);
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_ping_type;
	}

	//Default constructor intended only for serialization use.
	Ping(void);

private:

	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	ping::Type m_ping_type;
};


}


#endif
