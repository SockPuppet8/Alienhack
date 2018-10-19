#ifndef ALIENHACK_DISCRETE_MOVE_ACTION_HPP
#define	ALIENHACK_DISCRETE_MOVE_ACTION_HPP


#include "../../Model/AHGameModel.hpp"
#include "../../Model/Objects/AHGameObject.hpp"
#include "../IGameEvents.hpp"
#include "HasCompletionCallback.hpp"


namespace AlienHack
{


class DiscreteMoveAction : public HasCompletionCallback
{
public:

	DiscreteMoveAction( 
		boost::shared_ptr< AHGameObject > object, 
		const RL_shared::WorldObject::WorldLocation& to, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_object(object), m_target(to), m_time_full(time), m_time_remaining(time)
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
		ar & boost::serialization::base_object< HasCompletionCallback >(*this);
		ar & m_object;
		ar & m_target;
		ar & m_time_full;
		ar & m_time_remaining;
	}

	//Default constructor intended only for serialization use.
	DiscreteMoveAction(void)
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
