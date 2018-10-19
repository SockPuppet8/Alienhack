#ifndef ALIENHACK_WEAPON_DATA_HPP
#define	ALIENHACK_WEAPON_DATA_HPP


#include "Weapon.hpp"
#include "Projectile.hpp"


namespace AlienHack
{

	struct WeaponData
	{
		weapon::Type weapon_type;
		weapon::Class weapon_class;
		RL_shared::GameTimeCoordinate reload_time;
		int clip_size;
		pickup::Type ammo_type;
		int weight; //grams

		RL_shared::GameTimeCoordinate fire_time;
		int num_shots;
		int shots_per_round;
		RL_shared::GameTimeCoordinate shot_interval;
		RL_shared::GameTimeCoordinate shot_speed; //time/unit
		int shot_accuracy;
		projectile::Type shot_type;
		impact::Type impact_type;
		int shot_range;
		damage::Type shot_damage_type;
		int shot_damage_amount;
		int shot_penetration;
		int shot_noise;
		int shot_damage_falloff; //unit/point
		int shot_accuracy_falloff; //unit/point
		weapon::Special allowed_mods;

		WeaponData(void)
			: weapon_type( weapon::Type_None )
			, weapon_class( weapon::Class_None )
			, reload_time( 0 )
			, clip_size( 0 )
			, ammo_type( pickup::None )
			, weight( 0 )
			, fire_time( 0 )
			, num_shots( 0 )
			, shots_per_round( 0 )
			, shot_interval( 0 )
			, shot_speed( 0 )
			, shot_accuracy( 0 )
			, shot_type( projectile::None )
			, impact_type( impact::None )
			, shot_range( 0 )
			, shot_damage_type( damage::None )
			, shot_damage_amount( 0 )
			, shot_penetration( 0 )
			, shot_noise( 0 )
			, shot_damage_falloff( 0 )
			, shot_accuracy_falloff( 0 )
			, allowed_mods( weapon::Special_None )
		{
		}

		WeaponData(
			weapon::Type in_weapon_type,
			weapon::Class in_weapon_class,
			RL_shared::GameTimeCoordinate in_reload_time,
			int in_clip_size,
			pickup::Type in_ammo_type,
			int in_weight,
			RL_shared::GameTimeCoordinate in_fire_time,
			int in_num_shots,
			int in_shots_per_round, 
			RL_shared::GameTimeCoordinate in_shot_interval,
			RL_shared::GameTimeCoordinate in_shot_speed,
			int in_shot_accuracy,
			projectile::Type in_shot_type,
			impact::Type in_impact_type,
			int in_shot_range,
			damage::Type in_shot_damage_type,
			int in_shot_damage_amount,
			int in_shot_penetration,
			int in_shot_noise,
			int in_shot_damage_falloff,
			int in_shot_accuracy_falloff, 
			weapon::Special in_allowed_mods = weapon::Special_None
			)
			: weapon_type( in_weapon_type )
			, weapon_class( in_weapon_class )
			, reload_time( in_reload_time )
			, clip_size( in_clip_size )
			, ammo_type( in_ammo_type )
			, weight( in_weight )
			, fire_time( in_fire_time )
			, num_shots( in_num_shots )
			, shots_per_round( in_shots_per_round )
			, shot_interval( in_shot_interval )
			, shot_speed( in_shot_speed )
			, shot_accuracy( in_shot_accuracy )
			, shot_type( in_shot_type )
			, impact_type( in_impact_type )
			, shot_range( in_shot_range )
			, shot_damage_type( in_shot_damage_type )
			, shot_damage_amount( in_shot_damage_amount )
			, shot_penetration( in_shot_penetration )
			, shot_noise( in_shot_noise )
			, shot_damage_falloff( in_shot_damage_falloff )
			, shot_accuracy_falloff( in_shot_accuracy_falloff )
			, allowed_mods( in_allowed_mods )
		{
		}
	};


	struct SplashDamage
	{
		damage::Type type;
		int	amount;
		int penetration;

		SplashDamage(void) : type(damage::None), amount(0), penetration(0)
		{
		}
		SplashDamage( damage::Type in_type, int in_amount, int in_penetration )
			: type( in_type ), amount( in_amount ), penetration( in_penetration )
		{
		}
	};

	namespace weapon_data
	{

		const WeaponData& getWeaponData( weapon::Type, weapon::Special );

		const SplashDamage& getWeaponSplashDamage( weapon::Type, weapon::Special );

	}

}


#endif
