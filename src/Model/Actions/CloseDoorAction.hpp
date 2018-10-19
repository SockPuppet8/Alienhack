#ifndef ALIENHACK_CLOSE_DOOR_ACTION_HPP
#define	ALIENHACK_CLOSE_DOOR_ACTION_HPP


#include "ActionEngine/ActionEngine.hpp"
#include "../../Model/Objects/AHGameObject.hpp"



namespace AlienHack
{


class CloseDoorAction : public RL_shared::BaseAction
{
public:

	CloseDoorAction( 
		boost::shared_ptr< AHGameObject > object, 
		const RL_shared::WorldObject::WorldLocation& at, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_object(object), m_target(at), m_time_full(time), m_time_remaining(time)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const
	{
		return m_object.lock();
	}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::BaseAction >(*this);
		ar & m_object;
		ar & m_target;
		ar & m_time_full;
		ar & m_time_remaining;
	}

	//Default constructor intended only for serialization use.
	CloseDoorAction(void)
		: m_time_full(0), m_time_remaining(0)
	{
	}

private:

	boost::weak_ptr< AHGameObject > m_object;
	RL_shared::WorldObject::WorldLocation m_target;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;

};


}


#endif
