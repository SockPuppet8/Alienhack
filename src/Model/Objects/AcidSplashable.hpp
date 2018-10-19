#ifndef ALIENHACK_ACID_SPLASHABLE_HPP
#define	ALIENHACK_ACID_SPLASHABLE_HPP


#include <boost/cstdint.hpp>


namespace RL_shared
{
	typedef boost::int32_t GameTimeCoordinate;
}


namespace AlienHack
{


class AHGameModel;
class AHGameObject;


class AcidSplashable
{
public: 

	AcidSplashable(void)
		: m_acid(0), m_timer(0)
	{
	}

	void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt, AHGameObject& );

	void addAcid(int amount)	{ m_acid += amount; }

	void setAcid(int amount)	{ m_acid = amount; }
	int getAcid(void) const		{ return m_acid; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & m_acid;
		ar & m_timer;
	}

private: 

	int m_acid;
	int m_timer;

};


}


#endif
