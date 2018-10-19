#ifndef ALIENHACK_WAIT_ACTION_HPP
#define	ALIENHACK_WAIT_ACTION_HPP


#include "HasCompletionCallback.hpp"
#include "../AHGameModel.hpp"


namespace AlienHack
{


class WaitAction : public HasCompletionCallback
{
public:

	WaitAction( 
		boost::shared_ptr< RL_shared::Actor > object, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_object(object), m_time_remaining(time)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model )
	{
		m_time_remaining = (std::max)((RL_shared::GameTimeCoordinate)0, m_time_remaining-t);
		if (0 == m_time_remaining)
		{
			AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
			onActionComplete(model);
		}
	}

	virtual bool interrupt( RL_shared::AGameModel& in_model )
	{
		m_time_remaining = 0;
		AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
		onActionComplete(model);
		return true;
	}

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
		ar & m_time_remaining;
	}

	//Default constructor intended only for serialization use.
	WaitAction(void)
		: m_time_remaining(0)
	{
	}

private:

	boost::weak_ptr< RL_shared::Actor > m_object;
	RL_shared::GameTimeCoordinate m_time_remaining;

};


}


#endif
