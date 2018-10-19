#ifndef ALIENHACK_DIRECT_ATTACK_ACTION_HPP
#define	ALIENHACK_DIRECT_ATTACK_ACTION_HPP


#include "HasCompletionCallback.hpp"
#include "../Objects/AHGameObject.hpp"


namespace AlienHack
{


class DirectAttackAction : public HasCompletionCallback
{
public:

	DirectAttackAction( 
		boost::shared_ptr< AHGameObject > attacker, 
		boost::shared_ptr< AHGameObject > attackee, 
		damage::Type dmg_type, int dmg_amount, int dmg_penetration, 
		RL_shared::GameTimeCoordinate time, 
		RL_shared::GameTimeCoordinate commit_time
		)
		: m_attacker(attacker), m_attackee(attackee), 
		m_dmg_type(dmg_type), m_dmg_amount(dmg_amount), m_dmg_penetration(dmg_penetration), 
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
		ar & m_dmg_type;
		ar & m_dmg_amount;
		ar & m_dmg_penetration;
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_commit_time;
	}

	//Default constructor intended only for serialization use.
	DirectAttackAction(void)
		: m_dmg_type(damage::None), m_dmg_penetration(0), m_time_full(0), 
		  m_time_remaining(0), m_commit_time(0)
	{
	}

private:

	boost::weak_ptr< AHGameObject > m_attacker;
	boost::weak_ptr< AHGameObject > m_attackee; 
	damage::Type m_dmg_type;
	int m_dmg_amount;
	int m_dmg_penetration;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	RL_shared::GameTimeCoordinate m_commit_time;

};


}


#endif
