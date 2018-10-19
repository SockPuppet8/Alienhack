#ifndef ALIENHACK_SPITTER_ATTACK_ACTION_HPP
#define	ALIENHACK_SPITTER_ATTACK_ACTION_HPP


#include "HasCompletionCallback.hpp"
#include "../Objects/AHGameObject.hpp"


namespace AlienHack
{


class Alien;
class PlayerCharacter;


class SpitterAttackAction : public HasCompletionCallback
{
public:

	SpitterAttackAction( 
		boost::shared_ptr< Alien > attacker, 
		boost::shared_ptr< PlayerCharacter > attackee, 
		int amount, int range_squared, 
		RL_shared::GameTimeCoordinate time
		);

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< HasCompletionCallback >(*this);
		ar & m_source;
		ar & m_attacker;
		ar & m_attackee; 
		ar & m_amount;
		ar & m_range_squared;
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_commit_time;
	}

	//Default constructor intended only for serialization use.
	SpitterAttackAction(void)
		: m_amount(0), m_time_full(0), m_time_remaining(0), m_commit_time(0)
	{
	}

private:

	RL_shared::WorldObject::WorldLocation m_source;
	boost::weak_ptr< Alien > m_attacker;
	boost::weak_ptr< PlayerCharacter > m_attackee; 
	int m_amount;
	int m_range_squared;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	RL_shared::GameTimeCoordinate m_commit_time; //TODO unused - remove me

};


}


#endif
