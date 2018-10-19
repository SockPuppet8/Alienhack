#ifndef ALIENHACK_THROW_GRENADE_ACTION_HPP
#define	ALIENHACK_THROW_GRENADE_ACTION_HPP


#include "ActionEngine/ActionEngine.hpp"
#include "../Objects/Pickup.hpp"
#include "../Objects/Projectile.hpp"
#include "Game/FastAction.hpp"


namespace AlienHack
{


class PlayerCharacter;


class ThrowGrenadeAction : public RL_shared::FastAction
{
public:

	ThrowGrenadeAction( 
		AHGameModel& model, 
		boost::shared_ptr< PlayerCharacter > player, 
		pickup::Type type, 
		int target_offs_x, int target_offs_z, 
		RL_shared::GameTimeCoordinate time
		);

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	
	{ 
		return m_time_remaining;
	}

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::FastAction >(*this);
		ar & m_player;
		ar & m_type;
		ar & m_target_x;
		ar & m_target_z;
		ar & m_time_full;
		ar & m_time_remaining;
		ar & m_commit_time;
	}

	//Default constructor intended only for serialization use.
	ThrowGrenadeAction(void)
		: m_type(pickup::None), m_target_x(0), m_target_z(0), 
		  m_time_full(0), m_time_remaining(0), m_commit_time(0)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_player;
	pickup::Type m_type;
	int m_target_x;
	int m_target_z;
	RL_shared::GameTimeCoordinate m_time_full;
	RL_shared::GameTimeCoordinate m_time_remaining;
	RL_shared::GameTimeCoordinate m_commit_time;

};



class GrenadeExplode : public IExecuteImpactAction
{
	RL_shared::WorldObject::WorldLocation	m_hit_loc;
	AHGameModel*		m_model;
	pickup::Type	m_grenade_type;
	RL_shared::DBKeyValue m_thrower;
	bool			m_flame_special_1;
	bool			m_flame_special_2;

public:

	GrenadeExplode( 
		AHGameModel& model, RL_shared::WorldObject::WorldLocation hit_loc, 
		pickup::Type type, RL_shared::DBKeyValue thrower, 
		bool flame_special_1, bool flame_special_2
		)
		: m_hit_loc(hit_loc), m_model(&model), m_grenade_type(type), 
		m_thrower(thrower), m_flame_special_1( flame_special_1 ), 
		m_flame_special_2( flame_special_2 )
	{
	}
	virtual void execute(void);

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & m_hit_loc;
		ar & m_model;
		ar & m_grenade_type;
		ar & m_thrower;
		ar & m_flame_special_1;
		ar & m_flame_special_2;
	}

	//Default constructor intended only for serialization use.
	GrenadeExplode(void)
		: m_model(0), m_grenade_type(pickup::None), 
		m_flame_special_1(false), m_flame_special_2(false)
	{
	}
};



}


#endif
