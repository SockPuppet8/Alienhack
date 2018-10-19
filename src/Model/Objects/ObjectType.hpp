#ifndef ALIENHACK_OBJECT_TYPE_HPP
#define	ALIENHACK_OBJECT_TYPE_HPP



namespace AlienHack
{
	namespace objects
	{
		enum type
		{
			None = 0, 

			Player, 
			Alien, 
			Door, 
			Pickup, 
			Armour, 
			Weapon,
			Decoration,
			Terminal,
			Barrel,
			Furniture,
			Explosive,
			Egg,

			First_Transient, 

			Projectile = First_Transient, 
			Explosion,
			Fire,
			LaserTrail,
			Ping
		};

		inline bool isPassiveTarget( type object )
		{
			return (Barrel == object) || (Egg == object);
		}
		inline bool isPreferredTarget( type object )
		{
			return (Egg == object) || (Alien == object);
		}
	}
}



#endif
