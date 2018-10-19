#include "AcidSplashable.hpp"
#include "AHGameObject.hpp"



namespace AlienHack
{


const int ACID_INTERVAL = 1000;


void AcidSplashable::notifyTimeStep( AHGameModel& model, RL_shared::GameTimeCoordinate dt, AHGameObject& owner )
{
	m_acid = (std::max)(0, m_acid);

	if (0 == m_acid)
	{
		m_timer = ACID_INTERVAL;
		return;
	}

	int acid_damage(0);

	m_timer -= dt;
	while (m_timer <= 0)
	{
		m_timer += ACID_INTERVAL;

		if (m_acid > 0)
		{
			acid_damage += m_acid;
			--m_acid;
		}
	}

	if (acid_damage > 0)
		owner.takeDamage(model, RL_shared::INVALID_KEY, damage::Acid, acid_damage, 0);
}


}
