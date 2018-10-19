#ifndef ALIENHACK_HUGGER_ATTACK_ACTION_HPP
#define	ALIENHACK_HUGGER_ATTACK_ACTION_HPP


#include "HasCompletionCallback.hpp"
#include "../Objects/AHGameObject.hpp"


namespace AlienHack
{


class Alien;
class PlayerCharacter;


class HuggerAttackAction : public HasCompletionCallback
{
public:

	HuggerAttackAction( 
		boost::shared_ptr< Alien > attacker, 
		boost::shared_ptr< PlayerCharacter > attackee, 
		RL_shared::GameTimeCoordinate time, 
		RL_shared::GameTimeCoordinate commit_time
		)
		: m_attacker(attacker), m_attackee(attackee), 
		m_time_full(time), m_time_remaining(time), m_commit_time(commit_time)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< HasCompletionCallback >(*this);
		ar & m_attacker;
		ar & m_attackee; 
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_commit_time;
	}

	//Default constructor intended only for serialization use.
	HuggerAttackAction(void)
		: m_time_full(0), m_time_remaining(0), m_commit_time(0)
	{
	}

private:

	boost::weak_ptr< Alien > m_attacker;
	boost::weak_ptr< PlayerCharacter > m_attackee; 
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	RL_shared::GameTimeCoordinate m_commit_time;

};


}


#endif
