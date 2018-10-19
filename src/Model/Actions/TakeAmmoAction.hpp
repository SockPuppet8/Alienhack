#ifndef ALIENHACK_TAKE_AMMO_ACTION_HPP
#define	ALIENHACK_TAKE_AMMO_ACTION_HPP


#include "ActionEngine/ActionEngine.hpp"
#include "../Objects/PlayerCharacter.hpp"


namespace AlienHack
{


class Weapon;


class TakeAmmoAction : public RL_shared::BaseAction
{
public:

	TakeAmmoAction( 
		boost::shared_ptr< PlayerCharacter > player, 
		boost::shared_ptr< Weapon > object, 
		RL_shared::GameTimeCoordinate total_time 
		)
		: m_player(player), m_object(object), 
		m_time_full(total_time), m_time_remaining(total_time), m_committed(false)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual bool interrupt( RL_shared::AGameModel& );

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::BaseAction >(*this);
		ar & m_player;
		ar & m_object;
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_committed;
	}

	//Default constructor intended only for serialization use.
	TakeAmmoAction(void)
		: m_time_full(0), m_time_remaining(0), m_committed(false)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_player;
	boost::weak_ptr< Weapon > m_object;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	bool m_committed;

};


}


#endif
