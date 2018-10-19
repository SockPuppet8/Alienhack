#ifndef ALIENHACK_SET_CHARGE_ACTION_HPP
#define	ALIENHACK_SET_CHARGE_ACTION_HPP


#include "ActionEngine/ActionEngine.hpp"
#include "WorldObjects/WorldObject.hpp"


namespace AlienHack
{


class PlayerCharacter;


class SetChargeAction : public RL_shared::BaseAction
{
public:

	SetChargeAction( 
		boost::shared_ptr< PlayerCharacter > player, 
		RL_shared::WorldObject::WorldLocation loc, 
		int dx, int dz, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_player(player), m_loc(loc), m_dir_x(dx), m_dir_z(dz), 
		m_time_full(time), m_time_remaining(time), m_committed(false)
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
		ar & m_loc;
		ar & m_dir_x;
		ar & m_dir_z;
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_committed;
	}

	//Default constructor intended only for serialization use.
	SetChargeAction(void)
		: m_time_full(0), m_time_remaining(0), m_committed(false)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_player;
	RL_shared::WorldObject::WorldLocation m_loc;
	int m_dir_x;
	int m_dir_z;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	bool m_committed;

};


}


#endif
