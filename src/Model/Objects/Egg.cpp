#include "Egg.hpp"
#include "TerrainType.hpp"
#include "ObjectType.hpp"
#include "Alien.hpp"
#include "../IGameEvents.hpp"
#include "../AHGameModel.hpp"
#include <stdlib.h>



namespace AlienHack
{


namespace
{
	const RL_shared::GameTimeCoordinate PROX_CHECK_INTERVAL = 400;
}


Egg::Egg(void)
: AHGameObject(objects::Egg)
, m_timer(PROX_CHECK_INTERVAL + (rand() & 0xff))
, m_HP(10)
{
}

bool Egg::canOverlap(const RL_shared::WorldObject& other) const
{
	if (m_HP <= 0)
		return true;

	const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >(other) );

	if (objects::Alien == game_obj.type())
	{
		const Alien& alien(dynamic_cast< const Alien& >(game_obj));
		return (aliens::isHugger(alien.alienType())) || alien.isInSubstructure();
	}

	return game_obj.type() >= objects::First_Transient;
}
bool Egg::canOverlap(TerrainType type) const
{
	return terrain::isPassable(type);
}

void Egg::notifyTimeStep( AHGameModel& model, RL_shared::GameTimeCoordinate t )
{
	if (m_HP >= 0)
	{
		m_timer -= t;
		if (m_timer <= 0)
		{
			m_timer = PROX_CHECK_INTERVAL + (rand() & 0xff);

			using namespace RL_shared;
			World& world( model.world() );
			DBKeyValue avatar_key( model.avatar() );
			if (world.objectExists( avatar_key ))
			{
				WorldObject::WorldLocation player_loc( world.object(avatar_key).location() );
				WorldObject::WorldLocation my_loc( location() );
				if (player_loc.zone == my_loc.zone)
				{
					int dx = my_loc.x - player_loc.x;
					int dz = my_loc.z - player_loc.z;
					int dsq = (dx*dx)+(dz*dz);
					if ((rand() & 15) >= dsq)
					{
						//TODO: Initiate hatch action

						hatch(model);
					}
				}
			}
		}
	}
}
bool Egg::removeMe(AHGameModel&) const
{
	return (m_HP <= 0);
}
bool Egg::shouldDraw(void) const
{
	return true;
}
bool Egg::blocksDoor(void) const
{
	return true;
}

int Egg::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	if (m_HP > 0)
	{
		if (type != damage::Acid)
		{
			amount -= (std::max)(0, 2-penetration);
			amount = (std::min)((int)m_HP, amount);
			m_HP -= amount;

			//TODO: message about damage

			if (m_HP <= 0)
			{
				//TODO: message about egg destruction

				if (0 == (rand() & 3))
					hatch(model);
			}
			else
			if ((amount > 0) && (rand() & 1))
			{
				//TODO: initiate hatch action

				hatch(model);
			}

			return amount;
		}
	}
	return 0;
}

void Egg::hatch( AHGameModel& model )
{
	m_HP = 0;

	aliens::Type type = (rand() & 3) ? aliens::Facehugger : aliens::Fasthugger;

	using namespace RL_shared;

	World& world( model.world() );
	boost::shared_ptr< Alien > alien( new Alien(type, false) );
	DBKeyValue alien_key( world.addWorldObject( alien ) );
	if (!alien->moveTo(model, location(), true))
	{
		world.removeWorldObject(alien_key);
	}
}

std::string Egg::getThereIsString(bool) const
{
	return "";
}

std::string Egg::getSelectName(bool) const
{
	return "Egg";
}


}
