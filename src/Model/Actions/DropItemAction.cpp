#include "DropItemAction.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


void DropItemAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	RL_shared::GameTimeCoordinate half_time( m_time_full / 2 );
	if ((old_time >= half_time) && (m_time_remaining < half_time))
	{
		shared_ptr< PlayerCharacter > player( m_player.lock() );
		if (player && (player->num(m_item_type) > 0))
		{
		    shared_ptr< Pickup > item_obj( new Pickup(m_item_type) );
		    DBKeyValue item_key( model.world().addWorldObject( item_obj ) );
            if (item_obj->moveTo(model, player->location(), true))
            {
    			player->dec(m_item_type, false);
    			if (model.gameEventsObserver())
    			{
    			    model.gameEventsObserver()->playerDropsItem(model, *player, *item_obj);
    			}
			}
			else
			{
			    model.world().removeWorldObject(item_key);
			}
		}
	}
}


boost::shared_ptr< RL_shared::Actor > DropItemAction::actor(void) const
{
	return m_player.lock();
}


}
