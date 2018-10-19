#ifndef ALIENHACK_DISCRETE_TURN_ACTION_HPP
#define	ALIENHACK_DISCRETE_TURN_ACTION_HPP


#include "../../Model/AHGameModel.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "HasCompletionCallback.hpp"


namespace AlienHack
{


class DiscreteTurnAction : public HasCompletionCallback
{
public:

	DiscreteTurnAction( 
		boost::shared_ptr< PlayerCharacter > object, 
		boost::int16_t to, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_object(object), m_target(to), m_time_full(time), m_time_remaining(time)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model )
	{
		AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
		RL_shared::GameTimeCoordinate old_time( m_time_remaining );
		m_time_remaining = (std::max)((RL_shared::GameTimeCoordinate)0, m_time_remaining-t);
		RL_shared::GameTimeCoordinate half_time( m_time_full / 2 );
		if ((old_time >= half_time) && (m_time_remaining < half_time))
		{
			boost::shared_ptr< PlayerCharacter > object( m_object.lock() );
			if (object)
			{
				object->setHeading(m_target);
			}
		}

		if (0 == m_time_remaining)
			onActionComplete(model);
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
		ar & m_target;
		ar & m_time_full;
		ar & m_time_remaining;
	}

	//Default constructor intended only for serialization use.
	DiscreteTurnAction(void)
		: m_target(0), m_time_full(0), m_time_remaining(0)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_object;
	boost::int16_t m_target;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;

};


}


#endif
