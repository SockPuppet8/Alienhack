#ifndef ALIENHACK_USE_ITEM_ACTION_HPP
#define	ALIENHACK_USE_ITEM_ACTION_HPP


#include "ActionEngine/ActionEngine.hpp"
#include "../Objects/Pickup.hpp"


namespace AlienHack
{


class PlayerCharacter;


class UseItemAction : public RL_shared::BaseAction
{
public:

	UseItemAction( 
		boost::shared_ptr< PlayerCharacter > player, 
		pickup::Type object, 
		RL_shared::GameTimeCoordinate time 
		)
		: m_player(player), m_object(object), m_time_full(time), m_time_remaining(time), 
		m_committed(false)
	{
	}

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual bool interrupt( RL_shared::AGameModel& );
	virtual bool isLongAction( RL_shared::AGameModel& ) const
	{
		return (pickup::Neutraliser != m_object);
	}

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
	UseItemAction(void)
		: m_object(pickup::None), m_time_full(0), m_time_remaining(0), m_committed(false)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_player;
	pickup::Type m_object;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	bool m_committed;

};


}


#endif
