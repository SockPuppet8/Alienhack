#ifndef ALIENHACK_PICKUP_ARMOUR_ACTION_HPP
#define	ALIENHACK_PICKUP_ARMOUR_ACTION_HPP


#include "ActionEngine/ActionEngine.hpp"


namespace AlienHack
{


class PlayerCharacter;
class Armour;


class PickupArmourAction : public RL_shared::BaseAction
{
public:

	PickupArmourAction( 
		boost::shared_ptr< PlayerCharacter > player, 
		boost::shared_ptr< Armour > object, 
		RL_shared::GameTimeCoordinate takeoff_time, 
		RL_shared::GameTimeCoordinate total_time 
		)
		: m_player(player), m_object(object), m_takeoff_time(total_time-takeoff_time), 
		m_time_full(total_time), m_time_remaining(total_time), m_committed(false)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual bool interrupt( RL_shared::AGameModel& );
	virtual bool isLongAction( RL_shared::AGameModel& ) const
	{
		return true;
	}

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::BaseAction >(*this);
		ar & m_player;
		ar & m_object;
		ar & m_takeoff_time;
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_committed;
	}

	//Default constructor intended only for serialization use.
	PickupArmourAction(void)
		: m_takeoff_time(0), m_time_full(0), m_time_remaining(0), m_committed(false)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_player;
	boost::weak_ptr< Armour > m_object;
	RL_shared::GameTimeCoordinate m_takeoff_time;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	bool m_committed;

};


}


#endif
