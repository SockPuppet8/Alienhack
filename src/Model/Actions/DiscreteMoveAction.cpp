#include "DiscreteMoveAction.hpp"
#include "../../Model/Objects/ObjectType.hpp"
#include "../../Model/Objects/Ping.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "../../Model/Objects/Alien.hpp"
#include "World-2DTiles/Zone.hpp"



namespace AlienHack
{


using namespace boost;
using namespace RL_shared;


void DiscreteMoveAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	if (old_time >= m_time_full)
	{
		shared_ptr< AHGameObject > object( m_object.lock() );
		if (object)
		{
			if (object->moveTo(model, m_target, true))
			{
				if (objects::Player == object->type())
				{
					model.dirtyHearingField();

					shared_ptr< PlayerCharacter > player( dynamic_pointer_cast< PlayerCharacter >( object ) );
					model.alertAliensInRadius(m_target.zone, m_target.x, m_target.z, 2);

					shared_ptr<IGameEvents> events( model.gameEventsObserver() );
					if (events)
						events->playerMoved(model, m_target);
				}
				else
				{
					if (rand() & 1)
					{
						WorldObject::WorldLocation loc( object->location() );
						World& world( model.world() );
						Zone& zone( world.zone( loc.zone ) );
						shared_ptr< Ping > ping( new Ping( ping::Motion, 800 ) );
						DBKeyValue ping_key( world.addWorldObject( ping ) );
						if (1 == (rand() & 15))
						{
							loc.x += ((rand()%3)-1);
							loc.z += ((rand()%3)-1);
						}
						ping->moveTo( model, loc, true );
					}
					if (objects::Alien == object->type())
					{
						alienMakesSound( model, *(dynamic_pointer_cast< Alien >(object)), 1 );
					}
				}
			}
			else
				m_time_remaining = 0; //blocked.
		}
	}

	if (0 == m_time_remaining)
		onActionComplete(model);
}


}

