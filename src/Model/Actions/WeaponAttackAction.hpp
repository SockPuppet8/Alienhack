#ifndef ALIENHACK_WEAPON_ATTACK_ACTION_HPP
#define	ALIENHACK_WEAPON_ATTACK_ACTION_HPP


#include "../Objects/PlayerCharacter.hpp"
#include "../Objects/Projectile.hpp"
#include "Game/FastAction.hpp"


namespace AlienHack
{


class Weapon;


class WeaponAttackAction : public RL_shared::FastAction
{
public:

	WeaponAttackAction( 
		AHGameModel& model, 
		RL_shared::World& world, 
		boost::shared_ptr< PlayerCharacter > player, 
		boost::shared_ptr< Weapon > weapon, 
		int target_offs_x, int target_offs_z
		);

	virtual void advance( RL_shared::GameTimeCoordinate t, RL_shared::AGameModel& in_model );

	virtual RL_shared::GameTimeCoordinate timeRemaining(void) const	
	{ 
		return m_time_remaining; 
	}

	virtual boost::shared_ptr< RL_shared::Actor > actor(void) const
	{
		return m_player.lock();
	}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::FastAction >(*this);
		ar & m_player;
		ar & m_weapon;
		ar & m_time_remaining;
		ar & m_num_shots;
		ar & m_shot_interval;
		ar & m_shot_timer;
		ar & m_target_x;
		ar & m_target_z;
		ar & m_target_obj;
		ar & m_first_shot;
	}

	//Default constructor intended only for serialization use.
	WeaponAttackAction(void)
		: m_time_remaining(0), m_num_shots(0), m_shot_interval(0), m_shot_timer(0), 
		  m_target_x(0), m_target_z(0), m_target_obj(RL_shared::INVALID_KEY), m_first_shot(false)
	{
	}

private:

	boost::weak_ptr< PlayerCharacter > m_player;
	boost::weak_ptr< Weapon > m_weapon;
	RL_shared::GameTimeCoordinate m_time_remaining;
	int m_num_shots;
	RL_shared::GameTimeCoordinate m_shot_interval;
	RL_shared::GameTimeCoordinate m_shot_timer;
	int m_target_x;
	int m_target_z;
	RL_shared::DBKeyValue m_target_obj;
	bool m_first_shot;

};



class DoWeaponDamage : public IExecuteImpactAction
{
	RL_shared::WorldObject::WorldLocation	m_hit_loc;
	AHGameModel*					m_model;
	RL_shared::DBKeyValue		m_hit_obj;
	RL_shared::DBKeyValue		m_source_obj;
	damage::Type	m_damage_type;
	weapon::Type	m_weapon_type;
	int				m_amount;
	int				m_penetration;

public:

	DoWeaponDamage( 
		AHGameModel& model, RL_shared::DBKeyValue hit_obj, RL_shared::DBKeyValue source_obj, 
		RL_shared::WorldObject::WorldLocation hit_loc, 
		damage::Type type, weapon::Type weapon, int amount, int penetration 
		)
		: m_hit_loc(hit_loc), m_model(&model), m_hit_obj(hit_obj), m_source_obj(source_obj), 
		m_damage_type(type), m_weapon_type(weapon), m_amount(amount), m_penetration(penetration)
	{
	}

	virtual void execute(void);


    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & m_hit_loc;
		ar & m_model;
		ar & m_hit_obj;
		ar & m_source_obj;
		ar & m_damage_type;
		ar & m_weapon_type;
		ar & m_amount;
		ar & m_penetration;
    }

	//Default constructor intended only for serialization use.
	DoWeaponDamage(void)
		: m_model(0), m_hit_obj(RL_shared::INVALID_KEY), m_source_obj(RL_shared::INVALID_KEY), 
		m_damage_type(damage::None), m_weapon_type(weapon::Type_None), m_amount(0), m_penetration(0)
	{
	}

};



}


#endif
