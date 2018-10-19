#include "PickupWeaponAction.hpp"
#include "../Objects/Weapon.hpp"
#include "../Objects/WeaponData.hpp"
#include "../AHGameModel.hpp"
#include "../IGameEvents.hpp"
#include "assert.hpp"



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


PlayerCharacter::WeaponSlot getSlot( shared_ptr<Weapon> weapon_obj )
{
	switch( weapon_obj->weaponData().weapon_class )
	{
		case weapon::Sidearm:	return PlayerCharacter::Sidearm;
		case weapon::Primary:	return PlayerCharacter::Primary;
		default:;
	}
	assert(false);
	return PlayerCharacter::WeaponSlot_None;
}


void PickupWeaponAction::advance( GameTimeCoordinate t, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	World& world( model.world() );

	shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );

	GameTimeCoordinate old_time( m_time_remaining );
	m_time_remaining = (std::max)((GameTimeCoordinate)0, m_time_remaining-t);

	if (!m_committed)
	{
		if ((old_time > m_drop_time) && (m_drop_time >= m_time_remaining))
		{
			shared_ptr< PlayerCharacter > player( m_player.lock() );
			shared_ptr< Weapon > object( m_object.lock() );
			if (object)
			{
				PlayerCharacter::WeaponSlot slot( getSlot(object) );
				if (player)
					if ( world.objectExists( player->weapon( slot ) ) )
						player->dropWeapon(model, slot);
			}
		}

		if ((old_time > 0) && (0 >= m_time_remaining))
		{
			shared_ptr< PlayerCharacter > player( m_player.lock() );
			shared_ptr< Weapon > object( m_object.lock() );
			if (player && object)
			{
				m_committed = true;
				PlayerCharacter::WeaponSlot slot( getSlot(object) );
				player->useWeapon(model, *object, slot);
			}
		}
	}
}

bool PickupWeaponAction::interrupt( AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );

	if (!m_committed)
	{
		m_committed = true;
		m_time_remaining = 0;

		shared_ptr<IGameEvents> game_events( model.gameEventsObserver() );
		if (game_events)
			game_events->playerActionInterrupted(model);

		return true;
	}

	return false;
}

boost::shared_ptr< RL_shared::Actor > PickupWeaponAction::actor(void) const
{
	return m_player.lock();
}


}
