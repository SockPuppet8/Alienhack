#ifndef ALIENHACK_ENTER_EXIT_VENT_ACTION_HPP
#define	ALIENHACK_ENTER_EXIT_VENT_ACTION_HPP


#include "HasCompletionCallback.hpp"


namespace AlienHack
{


class Alien;


class EnterExitVentAction : public HasCompletionCallback
{
public:

	EnterExitVentAction( 
		boost::shared_ptr< Alien > alien, 
		bool entering, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_alien(alien), m_time_full(time), m_time_remaining(time), m_entering(entering)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< HasCompletionCallback >(*this);
		ar & m_alien;
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_entering;
	}

	//Default constructor intended only for serialization use.
	EnterExitVentAction(void)
		: m_time_full(0), m_time_remaining(0), m_entering(false)
	{
	}

private:

	boost::weak_ptr< Alien > m_alien;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	bool m_entering;

};


}


#endif
