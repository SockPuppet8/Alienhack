#include "WeaponData.hpp"
#include "assert.hpp"


namespace AlienHack
{


namespace weapon_data
{


const WeaponData no_weapon_data;


const WeaponData pistol_data
(
	weapon::Pistol,		//weapon::Type in_weapon_type,
	weapon::Sidearm,	//weapon::Class in_weapon_class,
	1250,				//RL_shared::GameTimeCoordinate in_reload_time,
	11,					//int in_clip_size,
	pickup::Ammo_9mm,	//pickup::Type in_ammo_type,
	950,				//int in_weight,
	300,				//int in_fire_time, (should be 583 for 35 RPM)
	1,					//int in_num_shots,
	1,					//int in_shots_per_round
	300,				//RL_shared::GameTimeCoordinate in_shot_interval,
	10,					//RL_shared::GameTimeCoordinate in_shot_speed,
	9,					//int in_shot_accuracy,
	projectile::Bullet,	//projectile::Type in_shot_type,
	impact::Puff,		//impact::Type in_impact_type,
	128,				//int in_shot_range,
	damage::Bullet,		//damage::Type in_shot_damage_type,
	4,		//int in_shot_damage_amount,
	0,		//int in_shot_penetration,
	8,		//int in_shot_noise
	4,		//int in_shot_damage_falloff,
	4,		//int in_shot_accuracy_falloff
	weapon::Special_All
);

const WeaponData smg_data
(
	weapon::SMG,		//weapon::Type in_weapon_type,
	weapon::Primary,	//weapon::Class in_weapon_class,
	1250,				//RL_shared::GameTimeCoordinate in_reload_time,
	30,					//int in_clip_size,
	pickup::Ammo_9mm,	//pickup::Type in_ammo_type,
	2000,				//int in_weight,
	300,	//int in_fire_time,
	5,		//int in_num_shots,
	1,		//int in_shots_per_round
	50,		//RL_shared::GameTimeCoordinate in_shot_interval, //should be 100 for 600rpm
	10,		//RL_shared::GameTimeCoordinate in_shot_speed,
	6,		//int in_shot_accuracy,
	projectile::Bullet,		//projectile::Type in_shot_type,
	impact::Puff,			//impact::Type in_impact_type,
	64,				//int in_shot_range,
	damage::Bullet,		//damage::Type in_shot_damage_type,
	4,		//int in_shot_damage_amount,
	0,		//int in_shot_penetration,
	9,		//int in_shot_noise
	3,		//int in_shot_damage_falloff,
	3,		//int in_shot_accuracy_falloff
	weapon::Special_All
);

const WeaponData mp_data
(
	weapon::MachinePistol,		//weapon::Type in_weapon_type,
	weapon::Sidearm,	//weapon::Class in_weapon_class,
	1250,				//RL_shared::GameTimeCoordinate in_reload_time,
	18,					//int in_clip_size,
	pickup::Ammo_9mm,	//pickup::Type in_ammo_type,
	1300,				//int in_weight,
	300,	//int in_fire_time,
	3,		//int in_num_shots,
	1,		//int in_shots_per_round
	50,		//RL_shared::GameTimeCoordinate in_shot_interval, //should be 100 for 600rpm
	10,		//RL_shared::GameTimeCoordinate in_shot_speed,
	5,		//int in_shot_accuracy,
	projectile::Bullet,		//projectile::Type in_shot_type,
	impact::Puff,			//impact::Type in_impact_type,
	32,				//int in_shot_range,
	damage::Bullet,		//damage::Type in_shot_damage_type,
	4,		//int in_shot_damage_amount,
	0,		//int in_shot_penetration,
	9,		//int in_shot_noise
	2,		//int in_shot_damage_falloff,
	2,		//int in_shot_accuracy_falloff
	weapon::Special_All
);

const WeaponData pulse_data
(
	weapon::PulseRifle,		//weapon::Type in_weapon_type,
	weapon::Primary,		//weapon::Class in_weapon_class,
	1250,		//RL_shared::GameTimeCoordinate in_reload_time,
	95,			//int in_clip_size,
	pickup::Ammo_pulse,		//pickup::Type in_ammo_type,
	3600,		//int in_weight,
	200,		//int in_fire_time,
	3,			//int in_num_shots,
	1,			//int in_shots_per_round
	40,			//RL_shared::GameTimeCoordinate in_shot_interval, //should be 80 for 750rpm
	10,			//RL_shared::GameTimeCoordinate in_shot_speed,
	8,			//int in_shot_accuracy,
	projectile::Bullet,		//projectile::Type in_shot_type,
	impact::Puff,			//impact::Type in_impact_type,
	128,					//int in_shot_range,
	damage::Bullet,			//damage::Type in_shot_damage_type,
	5,		//int in_shot_damage_amount,
	2,		//int in_shot_penetration,
	9,		//int in_shot_noise
	4,		//int in_shot_damage_falloff,
	4,		//int in_shot_accuracy_falloff
	weapon::Special_All
);

const WeaponData sawnoff_data
(
	weapon::SawnoffShotgun,		//weapon::Type in_weapon_type,
	weapon::Sidearm,			//weapon::Class in_weapon_class,
	2500,						//RL_shared::GameTimeCoordinate in_reload_time,
	1,							//int in_clip_size,
	pickup::Ammo_shotgun,		//pickup::Type in_ammo_type,
	1500,				//int in_weight,
	500,	//int in_fire_time,
	1,		//int in_num_shots,
	15,		//int in_shots_per_round
	0,		//RL_shared::GameTimeCoordinate in_shot_interval,
	10,		//RL_shared::GameTimeCoordinate in_shot_speed,
	20,		//int in_shot_accuracy,
	projectile::Shot,		//projectile::Type in_shot_type,
	impact::Puff,			//impact::Type in_impact_type,
	10,					//int in_shot_range,
	damage::Bullet,		//damage::Type in_shot_damage_type,
	3,		//int in_shot_damage_amount,
	0,		//int in_shot_penetration,
	13,		//int in_shot_noise
	2,		//int in_shot_damage_falloff,
	2,		//int in_shot_accuracy_falloff
	(weapon::Special)(weapon::Clip|weapon::Weight|weapon::Damage|weapon::Reload)
);

const WeaponData shotgun_data
(
	weapon::CombatShotgun,		//weapon::Type in_weapon_type,
	weapon::Primary,			//weapon::Class in_weapon_class,
	1750,						//RL_shared::GameTimeCoordinate in_reload_time,
	4,							//int in_clip_size,
	pickup::Ammo_shotgun,		//pickup::Type in_ammo_type,
	2260,				//int in_weight,
	600,	//int in_fire_time,
	1,		//int in_num_shots,
	15,		//int in_shots_per_round
	0,		//RL_shared::GameTimeCoordinate in_shot_interval,
	10,		//RL_shared::GameTimeCoordinate in_shot_speed,
	20,		//int in_shot_accuracy,
	projectile::Shot,		//projectile::Type in_shot_type,
	impact::Puff,			//impact::Type in_impact_type,
	10,					//int in_shot_range,
	damage::Bullet,		//damage::Type in_shot_damage_type,
	3,		//int in_shot_damage_amount,
	0,		//int in_shot_penetration,
	11,		//int in_shot_noise
	3,		//int in_shot_damage_falloff,
	2,		//int in_shot_accuracy_falloff
	(weapon::Special)(weapon::Clip|weapon::Weight|weapon::Damage|weapon::Reload)
);

const WeaponData colt_data
(
	weapon::Colt,		//weapon::Type in_weapon_type,
	weapon::Sidearm,	//weapon::Class in_weapon_class,
	1750,				//RL_shared::GameTimeCoordinate in_reload_time,
	6,					//int in_clip_size,
	pickup::Ammo_colt,	//pickup::Type in_ammo_type,
	1500,				//int in_weight,
	450,				//int in_fire_time,
	1,					//int in_num_shots,
	1,			//int in_shots_per_round
	450,				//RL_shared::GameTimeCoordinate in_shot_interval,
	10,					//RL_shared::GameTimeCoordinate in_shot_speed,
	15,					//int in_shot_accuracy,
	projectile::Bullet,	//projectile::Type in_shot_type,
	impact::Puff,		//impact::Type in_impact_type,
	128,				//int in_shot_range,
	damage::Bullet,		//damage::Type in_shot_damage_type,
	9,		//int in_shot_damage_amount,
	3,		//int in_shot_penetration,
	12,		//int in_shot_noise
	4,		//int in_shot_damage_falloff,
	4,		//int in_shot_accuracy_falloff
	weapon::Special_All
);

const WeaponData flamer_data
(
	weapon::Flamer,		//weapon::Type in_weapon_type,
	weapon::Primary,	//weapon::Class in_weapon_class,
	2000,				//RL_shared::GameTimeCoordinate in_reload_time,
	100,				//int in_clip_size,
	pickup::Ammo_flame,	//pickup::Type in_ammo_type,
	2700,				//int in_weight,
	300,				//int in_fire_time,
	20,					//int in_num_shots,
	1,					//int in_shots_per_round
	12,					//RL_shared::GameTimeCoordinate in_shot_interval,
	30,					//RL_shared::GameTimeCoordinate in_shot_speed,
	6,					//int in_shot_accuracy,
	projectile::Flame,	//projectile::Type in_shot_type,
	impact::Fire,		//impact::Type in_impact_type,
	6,					//int in_shot_range,
	damage::Fire,		//damage::Type in_shot_damage_type,
	1,		//int in_shot_damage_amount,
	0,		//int in_shot_penetration,
	7,		//int in_shot_noise
	10,		//int in_shot_damage_falloff,
	1,		//int in_shot_accuracy_falloff
	(weapon::Special)(weapon::Clip|weapon::Weight|weapon::Reload)
);

const WeaponData smartgun_data
(
	weapon::SmartGun,		//weapon::Type in_weapon_type,
	weapon::Primary,		//weapon::Class in_weapon_class,
	2500,		//RL_shared::GameTimeCoordinate in_reload_time,
	250,		//int in_clip_size,
	pickup::Ammo_smartgun,		//pickup::Type in_ammo_type,
	17820,		//int in_weight,
	500,		//int in_fire_time,
	15,			//int in_num_shots,
	1,			//int in_shots_per_round
	25,			//RL_shared::GameTimeCoordinate in_shot_interval, //should be 50 for 1200rpm
	10,			//RL_shared::GameTimeCoordinate in_shot_speed,
	15,			//int in_shot_accuracy,
	projectile::Bullet,		//projectile::Type in_shot_type,
	impact::Puff,			//impact::Type in_impact_type,
	128,					//int in_shot_range,
	damage::Bullet,			//damage::Type in_shot_damage_type,
	6,		//int in_shot_damage_amount,
	2,		//int in_shot_penetration,
	12,		//int in_shot_noise
	3,		//int in_shot_damage_falloff,
	4,		//int in_shot_accuracy_falloff
	(weapon::Special)(weapon::Clip|weapon::Weight|weapon::Damage)
);

const WeaponData laser_data
(
	weapon::Laser,		//weapon::Type in_weapon_type,
	weapon::Primary,	//weapon::Class in_weapon_class,
	1750,				//RL_shared::GameTimeCoordinate in_reload_time,
	20,					//int in_clip_size,
	pickup::Ammo_laser,	//pickup::Type in_ammo_type,
	6850,				//int in_weight,
	500,				//int in_fire_time,
	1,					//int in_num_shots,
	1,			//int in_shots_per_round
	500,					//RL_shared::GameTimeCoordinate in_shot_interval,
	1,					//RL_shared::GameTimeCoordinate in_shot_speed,
	12,					//int in_shot_accuracy,
	projectile::Laser, 	//projectile::Type in_shot_type,
	impact::Laser,		//impact::Type in_impact_type,
	128,				//int in_shot_range,
	damage::Laser,		//damage::Type in_shot_damage_type,
	16,		//int in_shot_damage_amount,
	2,		//int in_shot_penetration,
	8,		//int in_shot_noise
	5,		//int in_shot_damage_falloff,
	5,		//int in_shot_accuracy_falloff
	weapon::Special_All
);

const WeaponData canister_data
(
	weapon::CanisterRifle,		//weapon::Type in_weapon_type,
	weapon::Primary,			//weapon::Class in_weapon_class,
	2000,						//RL_shared::GameTimeCoordinate in_reload_time,
	32,							//int in_clip_size,
	pickup::Ammo_HEDP30mm,		//pickup::Type in_ammo_type,
	6200,				//int in_weight,
	400,				//int in_fire_time,
	1,					//int in_num_shots,
	1,			//int in_shots_per_round
	333,				//RL_shared::GameTimeCoordinate in_shot_interval, //should be 666 for 90rpm
	20,					//RL_shared::GameTimeCoordinate in_shot_speed,
	8,					//int in_shot_accuracy,
	projectile::Grenade,	//projectile::Type in_shot_type,
	impact::Fire,		//impact::Type in_impact_type,
	128,				//int in_shot_range,
	damage::Lance,		//damage::Type in_shot_damage_type,
	20,		//int in_shot_damage_amount,
	10,		//int in_shot_penetration,
	6,		//int in_shot_noise
	2,		//int in_shot_damage_falloff,
	3,		//int in_shot_accuracy_falloff
	weapon::Special_All
);


//This holds redundant copies of the base data. But it helps to prevent indexing errors. 
WeaponData mod_weapons[weapon::NumTypes][weapon::Special_All];

void initModWeapons(void)
{
	using namespace weapon;
	assert( (int)Special_All >= (int)(Clip|Weight|Damage|Accuracy|Rate|Reload) );

	const WeaponData* base_data_all[weapon::NumTypes] = 
	{
		&pistol_data,  
		&smg_data, 
		&pulse_data, 
		&sawnoff_data, 
		&shotgun_data, 
		&colt_data, 
		&flamer_data, 
		&smartgun_data, 
		&laser_data, 
		&canister_data, 
		&mp_data
	};

	for (int weapon=0; weapon < weapon::NumTypes; ++weapon)
	{
		for (int spec=0; spec < weapon::Special_All; ++spec)
		{
			WeaponData& mod_data( mod_weapons[weapon][spec] ); 
			const WeaponData& base_data( *base_data_all[weapon] );

			mod_data = base_data;

			if (spec != (spec & base_data.allowed_mods))
				continue;

			if (spec & Clip)
			{
				switch(weapon)
				{
					case Pistol:			mod_data.clip_size += 8; break;
					case SMG:				mod_data.clip_size += 15; break;
					case PulseRifle:		mod_data.clip_size += 30;break;
					case SawnoffShotgun:	mod_data.clip_size *= 2; break;
					case CombatShotgun:		mod_data.clip_size *= 2; break;
					case Colt:				mod_data.clip_size += 3; break;
					case Flamer:			mod_data.clip_size += 60; break;
					case SmartGun:			mod_data.clip_size += 250; break;
					case Laser:				mod_data.clip_size += 10; break;
					case CanisterRifle:		mod_data.clip_size += 8; break;
					case MachinePistol:		mod_data.clip_size += 6; break;
					default:;
				}				
			}
			if (spec & Weight)
			{
				mod_data.weight = (75 * base_data.weight)/100;
			}
			if (spec & Damage)
			{
				if (base_data.shot_damage_amount >= 8)
				{
					mod_data.shot_damage_amount += (base_data.shot_damage_amount / 2);
				}
				else
				{
					mod_data.shot_damage_amount += 1;
				}
			}
			if (spec & Accuracy)
			{
				mod_data.shot_accuracy += 2;
				mod_data.shot_accuracy_falloff += 1;
				mod_data.shot_damage_falloff += 1;
			}
			if (spec & Rate)
			{
				mod_data.shot_interval = (mod_data.shot_interval*75)/100;
				mod_data.fire_time = (mod_data.fire_time*75)/100;
			}
			if (spec & Reload)
			{
				mod_data.reload_time = (mod_data.reload_time*75)/100;
			}
		}
	}
}


const WeaponData& getWeaponData( weapon::Type type, weapon::Special spec )
{
	//TODO find some way to drive this initialisation from the engine init, rather than relying on dodgy statics.
	static bool first_time = true;
	if (first_time)
	{
		initModWeapons();
		first_time = false;
	}

	using namespace weapon;

	if (Special_None != spec)
	{
		if ((type >= 0) && (type < NumTypes))
			return mod_weapons[type][spec & Special_All];
	}

	switch(type)
	{
		case Pistol:			return pistol_data; 
		case SMG:				return smg_data;
		case PulseRifle:		return pulse_data;
		case SawnoffShotgun:	return sawnoff_data;
		case CombatShotgun:		return shotgun_data;
		case Colt:				return colt_data;
		case Flamer:			return flamer_data;
		case SmartGun:			return smartgun_data;
		case Laser:				return laser_data;
		case CanisterRifle:		return canister_data;
		case MachinePistol:		return mp_data;
		default:;
	}
	ASSERT(false);
	return no_weapon_data;
}


const SplashDamage no_splash_damage;

const SplashDamage canister_hedp_splash
(
	damage::Explosion, 
	15, 
	0
);

const SplashDamage& getWeaponSplashDamage( weapon::Type type, weapon::Special spec )
{
	if (weapon::CanisterRifle == type)
	{
		return canister_hedp_splash;
	}
	return no_splash_damage;
}


}


}
