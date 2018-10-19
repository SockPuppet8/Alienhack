#ifndef ALIENHACK_DROP_ITEM_ACTION_HPP
#define	ALIENHACK_DROP_ITEM_ACTION_HPP


#include "ActionEngine/ActionEngine.hpp"
#include "../Objects/PlayerCharacter.hpp"
#include "../Objects/Pickup.hpp"


namespace AlienHack
{


class DropItemAction : public RL_shared::BaseAction
{
public:

	DropItemAction( 
		boost::shared_ptr< PlayerCharacter > player, 
		pickup::Type item_type, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_player(player), m_item_type(item_type), m_time_full(time), m_time_remaining(time)
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
		ar & m_item_type;
		ar & m_time_full;
		ar & m_time_remaining;
	}

	//Default constructor intended only for serialization use.
	DropItemAction(void)
		: m_time_full(0), m_time_remaining(0)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_player;
	pickup::Type m_item_type;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;

};


}


#endif
