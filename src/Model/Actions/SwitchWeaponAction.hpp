#ifndef ALIENHACK_SWITCH_WEAPON_ACTION_HPP
#define	ALIENHACK_SWITCH_WEAPON_ACTION_HPP


#include "ActionEngine/ActionEngine.hpp"
#include "../Objects/PlayerCharacter.hpp"


namespace AlienHack
{


class SwitchWeaponAction : public RL_shared::BaseAction
{
public:

	SwitchWeaponAction( 
		boost::shared_ptr< PlayerCharacter > player, 
		PlayerCharacter::WeaponSlot slot, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_player(player), m_slot(slot), m_time_full(time), m_time_remaining(time)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	{ return m_time_remaining; }

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::BaseAction >(*this);
		ar & m_player;
		ar & m_slot;
		ar & m_time_full;
		ar & m_time_remaining;
	}

	//Default constructor intended only for serialization use.
	SwitchWeaponAction(void)
		: m_time_full(0), m_time_remaining(0)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_player;
	PlayerCharacter::WeaponSlot m_slot;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;

};


}


#endif
