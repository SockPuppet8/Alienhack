#ifndef	ALIENHACK_PLAYER_STATS_HPP
#define	ALIENHACK_PLAYER_STATS_HPP


#include "Alien.hpp"
#include "PlayerCharacter.hpp"
#include <string>


namespace AlienHack
{



class PlayerStats
{
public:

	PlayerStats(void)
		: m_XP(0), m_spent_XP(0), m_perks(0), m_HP(0), m_max_HP(0), m_acid(0), m_hugger_attacks(0)
	{
		for (int i=0; i<pickup::NumTypes; ++i)
		{
			m_pickups[i] = 0;
			m_pickups_used[i] = 0;
		}
		for (int i=0; i<aliens::NumTypes; ++i)
		{
			m_kills[i] = 0;
		}
	}

	RL_shared::WorldObject::WorldLocation& location(void)	{ return m_location; }

	std::string& outcome(void)	{ return m_outcome; }
	std::string& name(void)		{ return m_name; }
	std::string& armour(void)	{ return m_armour; }
	std::string& primary(void)	{ return m_primary; }
	std::string& sidearm(void)	{ return m_sidearm; }

	int& XP(void)		{ return m_XP; }
	int& spentXP(void)	{ return m_spent_XP; }

	boost::uint32_t& perks(void)	{ return m_perks; }

	boost::int16_t& HP(void)	{ return m_HP; }
	boost::int16_t& maxHP(void)	{ return m_max_HP; }

	boost::int16_t& acid(void)	{ return m_acid; }

	boost::int16_t& huggerAttacksSurvived(void)	{ return m_hugger_attacks; }

	boost::uint16_t& pickups(pickup::Type type)
	{
		if ((type <= pickup::None) || (type >= pickup::NumTypes))
			throw std::runtime_error("Pickup index out of range in PlayerStats object");
		return m_pickups[ type ]; 
	}
	boost::uint16_t& pickupsUsed(pickup::Type type)
	{ 
		if ((type <= pickup::None) || (type >= pickup::NumTypes))
			throw std::runtime_error("Pickup index out of range in PlayerStats object");
		return m_pickups_used[ type ];
	}

	unsigned int& kills(aliens::Type type)
	{
		if ((type <= aliens::None) || (type >= aliens::NumTypes))
			throw std::runtime_error("Alien index out of range in PlayerStats object");
		return m_kills[ type ];
	}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & m_location;
		ar & m_outcome;
		ar & m_name;
		ar & m_armour;
		ar & m_primary;
		ar & m_sidearm;
		ar & m_XP;
		ar & m_spent_XP;
		ar & m_perks;
		ar & m_HP;
		ar & m_max_HP;
		ar & m_acid;
		ar & m_hugger_attacks;
		ar & m_pickups;
		ar & m_pickups_used;
		ar & m_kills;
    }

private:

	RL_shared::WorldObject::WorldLocation m_location;

	std::string m_outcome;
	std::string m_name;
	std::string m_armour;
	std::string m_primary;
	std::string m_sidearm;

	int m_XP;
	int m_spent_XP;

	boost::uint32_t m_perks;

	boost::int16_t m_HP;
	boost::int16_t m_max_HP;

	boost::int16_t m_acid;
	boost::int16_t m_hugger_attacks;

	boost::uint16_t m_pickups[ pickup::NumTypes ];
	boost::uint16_t m_pickups_used[ pickup::NumTypes ];

	unsigned int m_kills[ aliens::NumTypes ];
};



}


#endif
