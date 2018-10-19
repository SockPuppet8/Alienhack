#include "PlayingGame.hpp"
#include "story.hpp"
#include "draw.hpp"
#include "TargetSelect.hpp"
#include "LookMode.hpp"
#include "MessageBox.hpp"
#include "ChoiceDialog.hpp"
#include "GetDialog.hpp"
#include "DropDialog.hpp"
#include "GetWeaponDialog.hpp"
#include "CharScreen.hpp"
#include "StoryText.hpp"
#include "HelpScreen.hpp"
#include "Kaboom.hpp"
#include "NameEntry.hpp"
#include "BuyPerks.hpp"
#include "ViewFile.hpp"
#include "GameEvents.hpp"
#include "../../Model/save.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/FindNearest.hpp"
#include "../../Model/OverWorld.hpp"
#include "../../Model/Objects/ObjectType.hpp"
#include "../../Model/Objects/TerrainType.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "../../Model/Objects/Alien.hpp"
#include "../../Model/Objects/Pickup.hpp"
#include "../../Model/Objects/Armour.hpp"
#include "../../Model/Objects/Weapon.hpp"
#include "../../Model/Objects/WeaponData.hpp"
#include "../../Model/Objects/Terminal.hpp"
#include "../../Model/Objects/Explosion.hpp"
#include "../../Model/Objects/Fire.hpp"
#include "../../Model/Actions/WaitAction.hpp"
#include "../../Model/Actions/DiscreteMoveAction.hpp"
#include "../../Model/Actions/DiscreteTurnAction.hpp"
#include "../../Model/Actions/OpenDoorAction.hpp"
#include "../../Model/Actions/CloseDoorAction.hpp"
#include "../../Model/Actions/WeaponAttackAction.hpp"
#include "../../Model/Actions/PickupAction.hpp"
#include "../../Model/Actions/PickupArmourAction.hpp"
#include "../../Model/Actions/DropArmourAction.hpp"
#include "../../Model/Actions/PickupWeaponAction.hpp"
#include "../../Model/Actions/DropWeaponAction.hpp"
#include "../../Model/Actions/DropItemAction.hpp"
#include "../../Model/Actions/SwitchWeaponAction.hpp"
#include "../../Model/Actions/ReloadWeaponAction.hpp"
#include "../../Model/Actions/ThrowGrenadeAction.hpp"
#include "../../Model/Actions/UseItemAction.hpp"
#include "../../Model/Actions/UseTerminalAction.hpp"
#include "../../Model/Actions/TakeAmmoAction.hpp"
#include "../../Model/Actions/SetChargeAction.hpp"
#include "../../Model/MapCreator.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include "ActionEngine/ActionEngine.hpp"
#include "Input/IFunctionMap.hpp"
#include "ConsoleView/KeyPress.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>
#include <fstream>
#include <sstream>


namespace AlienHack
{

using namespace RL_shared;
using namespace boost;


namespace
{

	using namespace story_text;

	const std::string drop_armour_AYS_msg = 
		" Are you sure that you \n"
		" want to drop your armour? \n";
	const int drop_armour_AYS_x_low = 25;
	const int drop_armour_AYS_y_low = 17;
	const int drop_armour_AYS_x_high = 54;
	const int drop_armour_AYS_y_high = 22;

	const std::string drop_weapon_AYS_msg = 
		" Are you sure that you \n"
		" want to drop your weapon? \n";
	const int drop_weapon_AYS_x_low = 25;
	const int drop_weapon_AYS_y_low = 17;
	const int drop_weapon_AYS_x_high = 54;
	const int drop_weapon_AYS_y_high = 22;



class GetWeaponChoice : public IChoiceDialogSelect
{
	shared_ptr< PlayerCharacter > m_player; 
	shared_ptr< Weapon > m_weapon;
	AHGameModel& m_model;
	bool m_committed;

public: 

	GetWeaponChoice( AHGameModel& model, shared_ptr< PlayerCharacter > player, shared_ptr< Weapon > weapon )
		: m_model(model), m_player(player), m_weapon(weapon), m_committed(false)
	{
	}

	virtual bool select( int option )
	{
		if (GetWeaponDialog::TakeAmmo == option)
		{
			shared_ptr< TakeAmmoAction > action( new TakeAmmoAction(m_player, m_weapon, 
				m_player->getActionTime(m_model, player_actions::TakeAmmo)) );
			m_model.actionEngine().addAction( action );
			m_committed = true;
		}
		else if (GetWeaponDialog::TakeWeapon == option)
		{
			shared_ptr< PickupWeaponAction > action( new PickupWeaponAction(m_player, m_weapon, 
				m_player->getActionTime(m_model, player_actions::DropWeapon), 
				m_player->getActionTime(m_model, player_actions::PickupWeapon)) );
			m_model.actionEngine().addAction( action );
			m_committed = true;
		}
		return m_committed;
	}

	bool committed() const	{ return m_committed; }
};




class GetItemChoice : public IGetDialogSelect
{
	shared_ptr< IFunctionMap > m_keys;
	shared_ptr< PlayerCharacter > m_player; 
	AHGameModel& m_model;
	InterfaceState& m_is;

public:

	GetItemChoice( 
		InterfaceState& is, 
		shared_ptr< IFunctionMap > keys, 
		AHGameModel& model, 
		shared_ptr< PlayerCharacter > player 
		)
		: m_keys(keys), m_player(player), m_model(model), m_is(is)
	{
	}

	virtual bool select( boost::shared_ptr< AHGameObject > game_obj )
	{
		WorldObjectType obj_type( game_obj->type() );
		if (objects::Pickup == obj_type)
		{
			shared_ptr< Pickup > pickup( dynamic_pointer_cast< Pickup >( game_obj ) );
			shared_ptr< PickupAction > action( new PickupAction(m_player, pickup, m_player->getActionTime(m_model, player_actions::PickupObject)) );
			m_model.actionEngine().addAction( action );
			return true;
		}
		else if (objects::Armour == obj_type)
		{
			shared_ptr< Armour > armour( dynamic_pointer_cast< Armour >( game_obj ) );
			shared_ptr< PickupArmourAction > action( new PickupArmourAction(m_player, armour, 
				m_player->getActionTime(m_model, player_actions::DropArmour), 
				m_player->getActionTime(m_model, player_actions::PickupArmour)) );
			m_model.actionEngine().addAction( action );
			return true;
		}
		else if (objects::Weapon == obj_type)
		{
			shared_ptr< Weapon > weapon( dynamic_pointer_cast< Weapon >( game_obj ) );
			shared_ptr< GetWeaponChoice > weapon_choice( new GetWeaponChoice(m_model, m_player, weapon) );
			shared_ptr< GetWeaponDialog > weapon_dialog( 
				new GetWeaponDialog(m_is.interfaceStateMachine(), weapon, m_keys, weapon_choice, m_model.isCountdownActive() ? ChoiceDialog::Red : ChoiceDialog::Green ) );
			m_is.setNextState( weapon_dialog );
		}
		return false;
	}
};




bool doGetPickupsMenu(InterfaceState& is, shared_ptr< IFunctionMap > keys, AHGameModel& model, MessageDisplay& msgs)
{
	World& world( model.world() );
	if (!world.objectExists(model.avatar()))
		return false;
	shared_ptr<PlayerCharacter> player( dynamic_pointer_cast<PlayerCharacter>(world.objectPtr(model.avatar())) );
	WorldObject::WorldLocation player_loc( player->location() );
	if (!world.zoneExists(player_loc.zone))
		return false;
	Zone& zone( world.zone(player_loc.zone) );
	if (!zone.isWithin(player_loc.x, player_loc.z))
		return false;

	const ObjectList& objects( zone.objectsAt( player_loc.x, player_loc.z ) );

	shared_ptr< std::vector< shared_ptr< AHGameObject > > > pickups( new std::vector< shared_ptr< AHGameObject > >() );

	BOOST_FOREACH( RL_shared::DBKeyValue obj_key, objects )
	{
		if (world.objectExists( obj_key ))
		{
			shared_ptr<AHGameObject> game_obj( dynamic_pointer_cast<AHGameObject>( world.objectPtr( obj_key ) ) );
			WorldObjectType obj_type( game_obj->type() );
			if ((objects::Pickup == obj_type) || (objects::Armour == obj_type) || (objects::Weapon == obj_type))
				pickups->push_back( game_obj );
		}
	}

	if (pickups->empty())
	{
		msgs.addString("There's nothing here to get.");
		return false;
	}

	shared_ptr< GetItemChoice > item_choice( new GetItemChoice(is, keys, model, player) );

	if (1 == pickups->size())
		return item_choice->select( (*pickups)[0] );

	shared_ptr< GetDialog > get_dialog( new GetDialog(pickups, is.interfaceStateMachine(), keys, item_choice, model.isCountdownActive() ? GetDialog::Red : GetDialog::Green) );
	is.setNextState( get_dialog );

	return false;
}

}


class SelectFireTarget : public ITargetSelectCallback
{
	RL_shared::MessageDisplay& m_msgs;

public:

	explicit SelectFireTarget( RL_shared::MessageDisplay& msgs )
		: m_msgs(msgs)
	{
	}

	virtual std::pair< bool, bool > select( AHGameModel& model, int x, int z )
	{
		if ((0==x) && (0==z))
		{
			m_msgs.addString( "Invalid target\nSelect target:" );
			return std::pair< bool, bool >( false, false );
		}

		World& world( model.world() );
		if (world.objectExists( model.avatar() ))
		{
			shared_ptr< PlayerCharacter > player_obj( dynamic_pointer_cast< PlayerCharacter >( world.objectPtr(model.avatar()) ) );

			if (!player_obj->isValidTargetOffset( x, z ))
			{
				m_msgs.addString( "Invalid target\nSelect target:" );
				return std::pair< bool, bool >( false, false );
			}

			DBKeyValue weapon_key( player_obj->weapon( player_obj->currentWeapon() ) );
			if (world.objectExists(weapon_key))
			{
				shared_ptr< Weapon > weapon_obj( dynamic_pointer_cast<Weapon>( world.objectPtr( weapon_key ) ) );
				if (0 < weapon_obj->getAmmo())
				{
					shared_ptr< WeaponAttackAction > action( 
						new WeaponAttackAction( model, model.world(), player_obj, weapon_obj, x, z ) );
					model.actionEngine().addAction( action );

					m_msgs.addString("You open fire!");

					return std::pair< bool, bool >( true, true );
				}
			}
		}
		return std::pair< bool, bool >( false, false );
	}
};

class SelectGrenadeTarget : public ITargetSelectCallback
{
	RL_shared::MessageDisplay& m_msgs;
	pickup::Type m_grenade_type;

public:

	SelectGrenadeTarget( RL_shared::MessageDisplay& msgs, pickup::Type type )
		: m_msgs(msgs), m_grenade_type(type)
	{
	}

	virtual std::pair< bool, bool > select( AHGameModel& model, int x, int z )
	{
		World& world( model.world() );
		if (world.objectExists( model.avatar() ))
		{
			shared_ptr< PlayerCharacter > player_obj( dynamic_pointer_cast< PlayerCharacter >( world.objectPtr(model.avatar()) ) );

			shared_ptr< ThrowGrenadeAction > action( 
				new ThrowGrenadeAction( model, player_obj, m_grenade_type, x, z, 
					player_obj->getActionTime(model, player_actions::ThrowGrenade) ) );
			model.actionEngine().addAction( action );

			return std::pair< bool, bool >( true, true );
		}
		return std::pair< bool, bool >( false, false );
	}
};


class SelectDropItem : public IDropDialogSelect
{
	RL_shared::MessageDisplay& m_msgs;
	AHGameModel& m_model;
	World& m_world;
	OverWorld& m_overworld;
	shared_ptr< PlayerCharacter > m_player_obj;

public:

	SelectDropItem( 
	    RL_shared::MessageDisplay& msgs, 
	    AHGameModel& model, 
	    World& world, 
	    OverWorld& overworld, 
	    shared_ptr< PlayerCharacter > player_obj
	    ) : m_msgs(msgs), m_model(model), m_world(world), m_overworld(overworld), m_player_obj(player_obj)
	{
	}

	virtual bool select( const std::string& action )
	{
	    if ("Sidearm" == action)
	    {
	        return dropWeapon( PlayerCharacter::Sidearm, "sidearm" );
	    }
	    else if ("Primary" == action)
	    {
	        return dropWeapon( PlayerCharacter::Primary, "primary weapon" );
	    }
	    else if ("Armour" == action)
	    {
		    if (m_world.objectExists(m_player_obj->armour()))
		    {
			    shared_ptr< DropArmourAction > drop_action( 
				    new DropArmourAction(m_player_obj, 
					    m_player_obj->getActionTime(m_model, player_actions::DropArmour)) );
			    m_model.actionEngine().addAction( drop_action );
			    return true;
		    }
		    else
		    {
			    m_msgs.addString("You're not wearing any armour.");
		    }
	    }
	    else if ("Stun" == action)
	    {
	        return dropItem( pickup::StunGrenade, "stun grenades" );
	    }
	    else if ("Inc" == action)
	    {
	        return dropItem( pickup::IncGrenade, "inc grenades" );
	    }
	    else if ("Frag" == action)
	    {
	        return dropItem( pickup::FragGrenade, "frag grenades" );
	    }
	    else if ("Krak" == action)
	    {
	        return dropItem( pickup::KrakGrenade, "krak grenades" );
	    }
	    else if ("Medkit" == action)
	    {
	        return dropItem( pickup::Medkit, "medkits" );
	    }
	    else if ("Neutraliser" == action)
	    {
	        return dropItem( pickup::Neutraliser, "neutralisers" );
	    }
	    else if ("Demolition" == action)
	    {
	        return dropItem( pickup::DemoCharge, "demolition charges" );
	    }

        return false;
	}

private:

    bool dropWeapon( PlayerCharacter::WeaponSlot slot, const std::string& name )
    {
	    if (m_world.objectExists(m_player_obj->weapon( slot )))
	    {
		    shared_ptr< DropWeaponAction > drop_action( 
			    new DropWeaponAction(m_player_obj, slot, 
				    m_player_obj->getActionTime(m_model, player_actions::DropWeapon)) );
		    m_model.actionEngine().addAction( drop_action );
		    return true;
	    }
	    else
	    {
	        std::string msg;
            msg += "You don't have a " + name + ".";
		    m_msgs.addString(msg.c_str());
	    }
        return false;
    }

    bool dropItem( pickup::Type type, const std::string& name )
    {
        if (m_player_obj->num(type) > 0)
        {
		    shared_ptr< DropItemAction > drop_action( 
			    new DropItemAction(m_player_obj, type, 
				    m_player_obj->getActionTime(m_model, player_actions::DropItem)) );
		    m_model.actionEngine().addAction( drop_action );
		    return true;
        }
        else
        {
            std::string msg;
            msg += "You don't have any " + name + ".";
    	    m_msgs.addString(msg.c_str());
        }
        return false;
    }
};


PlayingGame::PlayingGame( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	bool loaded 
	)
: InterfaceState(ism)
, m_msgs( MessageDisplay::WindowParams(MSGS_X_LOW, MSGS_X_HI, MSGS_Y_LOW, MSGS_Y_HI) )
, m_key_map(keys)
, m_last_input(0)
, m_quit(false)
, m_loaded_game(loaded)
, m_move_only(false)
, m_turn_only(false)
, m_move_lock(false)
, m_turn_lock(false)
, m_activating_object(false)
, m_placing_charge(false)
, m_first_look_mode(true)
, m_shown_story_text_1(false)
, m_shown_story_text_2(false)
, m_shown_story_text_3(false)
, m_shown_intro_text(false)
, m_shown_master_text(false)
, m_shown_found_queen_text(false)
, m_shown_killed_queen_text(false)
, m_shown_countdown_start_text(false)
, m_shown_ending_text_1(false)
, m_good_ending(false)
, m_shown_bad_ending_text(false)
, m_shown_good_ending_text_1(false)
, m_shown_good_ending_text_2(false)
, m_shown_death_msg(false)
, m_shown_mortem(false)
{
	ASSERT( m_key_map );
}


void PlayingGame::enterFromParent( AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );

	if (!m_loaded_game)
	{
		AHGameModel::GameType game_type = model.gameType();
		model.clear(); //GameTypeMenu will have cleared it already. This is just safety.
		model.setGameType(game_type);

		initialiseWorld( model );

		//TODO make it unnecessary to do these here
		model.updateVision();
		model.updateHearing();

		shared_ptr< NameEntry > newstate( new NameEntry(interfaceStateMachine(), m_key_map) );
		setNextState( newstate );
	}
	else
	{
		m_first_look_mode = false;
		m_shown_story_text_1 = true;
		m_shown_story_text_2 = true;
		m_shown_story_text_3 = true;
		m_shown_intro_text = true;
	}

	m_game_events = makeGameEvents(m_msgs);
	model.setGameEventsObserver( m_game_events );

	m_msgs.addString( "Welcome to AlienHack!" );
	m_msgs.addString( "Press ? for help, l for look mode, c for the character screen." );
}
void PlayingGame::exitToParent( AGameModel& )
{
}
void PlayingGame::enterFromChild( AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast< AHGameModel& >( in_model ) );

	m_activating_object = false;
	m_placing_charge = false;
	m_move_only = false;
	m_turn_only = false;
	m_move_lock = false;
	m_turn_lock = false;

	if (m_quit)
	{
		showMortem();
		return;
	}

	if (!m_shown_story_text_1)
	{
		m_shown_story_text_1 = true;
		shared_ptr< StoryText > newstate( new StoryText(interfaceStateMachine(), background_text_1, 8) );
		setNextState( newstate );
		return;
	}
	if (!m_shown_story_text_2)
	{
		m_shown_story_text_2 = true;
		shared_ptr< StoryText > newstate( new StoryText(interfaceStateMachine(), background_text_2, 8) );
		setNextState( newstate );
		return;
	}
	else if (!m_shown_story_text_3)
	{
		m_shown_story_text_3 = true;
		shared_ptr< StoryText > newstate( new StoryText(interfaceStateMachine(), background_text_3, 7) );
		setNextState( newstate );
		return;
	}
	else if (!m_shown_intro_text)
	{
		m_shown_intro_text = true;
		shared_ptr< MessageBox > newstate;
		newstate.reset( new MessageBox(interfaceStateMachine(), m_key_map, intro_text, 17, 12, 62, 28, MessageBox::Green, 1) );
		setNextState( newstate );
		return;
	}

	if (m_shown_ending_text_1)
	{
		if (m_good_ending)
		{
			if (!m_shown_good_ending_text_1)
			{
				m_shown_good_ending_text_1 = true;
				shared_ptr< StoryText > newstate( new StoryText(interfaceStateMachine(), ending_text_good_1, 8) );
				setNextState( newstate );
				return;
			}
			else if (!m_shown_good_ending_text_2)
			{
				m_shown_good_ending_text_2 = true;
				shared_ptr< StoryText > newstate( new StoryText(interfaceStateMachine(), ending_text_good_2, 8) );
				setNextState( newstate );
				return;
			}
			else
			{
				m_quit = true;
				showMortem();
			}
		}
		else 
		{
			if (!m_shown_bad_ending_text)
			{
				m_shown_bad_ending_text = true;
				shared_ptr< StoryText > newstate( new StoryText(interfaceStateMachine(), ending_text_bad, 8) );
				setNextState( newstate );
				return;
			}
			else
			{
				m_quit = true;
				showMortem();
			}
		}
	}
}
void PlayingGame::exitToChild( AGameModel& )
{
}


PlayingGame::CommandResult PlayingGame::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	World& world( model.world() );

	if (!world.objectExists( model.avatar() ))
	{
		m_quit = true;
		showMortem();
		return CommandResult( false, true );
	}

	m_msgs.beginNewMessage();

	OverWorld& overworld( model.overworld() );

	ASSERT( world.objectExists( model.avatar() ) );
	shared_ptr< PlayerCharacter > player_obj( dynamic_pointer_cast<PlayerCharacter>( world.objectPtr( model.avatar() ) ) );
	ASSERT( player_obj );
	WorldObject::WorldLocation loc( player_obj->location() );

	m_last_input = model.gameTime();

	bool advance(false);
	bool move(false);
	int mx(0), mz(0);


	//if (RL_shared::KeyCode('v',false) == (dynamic_cast<const RL_shared::KeyPress&>(input)).value)
	//{
	//}

	if (m_key_map->isFunction(input, "Help"))
	{
		shared_ptr< HelpScreen > newstate( new HelpScreen(interfaceStateMachine(), m_key_map, model.isCountdownActive() ? HelpScreen::Red : HelpScreen::Normal) );
		setNextState( newstate );
		return CommandResult( false, true );
	}
	else if (m_key_map->isFunction(input, "Left"))
	{
		move = true;
		mx = -1;
	}
	else if (m_key_map->isFunction(input, "Right"))
	{
		move = true;
		mx = 1;
	}
	else if (m_key_map->isFunction(input, "Up"))
	{
		move = true;
		mz = 1;
	}
	else if (m_key_map->isFunction(input, "Down"))
	{
		move = true;
		mz = -1;
	}
	else if (m_key_map->isFunction(input, "UpAndLeft"))
	{
		move = true;
		mx = -1;
		mz = 1;
	}
	else if (m_key_map->isFunction(input, "UpAndRight"))
	{
		move = true;
		mx = 1;
		mz = 1;
	}
	else if (m_key_map->isFunction(input, "DownAndLeft"))
	{
		move = true;
		mx = -1;
		mz = -1;
	}
	else if (m_key_map->isFunction(input, "DownAndRight"))
	{
		move = true;
		mx = 1;
		mz = -1;
	}
	else if (m_key_map->isFunction(input, "Wait"))
	{
		advance = true;
		shared_ptr< WaitAction > wait_action( new WaitAction(player_obj, player_obj->getActionTime(model, player_actions::Wait)) );
		model.actionEngine().addAction( wait_action );
	}
	else if (m_key_map->isFunction(input, "Strafe"))
	{
		m_move_only = true;
		m_turn_only = false;
		m_move_lock = false;
		m_turn_lock = false;
		m_activating_object = false;
		m_placing_charge = false;
	}
	else if (m_key_map->isFunction(input, "StrafeLock"))
	{
		m_move_only = true;
		m_turn_only = false;
		m_move_lock = true;
		m_turn_lock = false;
		m_activating_object = false;
		m_placing_charge = false;
	}
	else if (m_key_map->isFunction(input, "Turn"))
	{
		m_move_only = false;
		m_turn_only = true;
		m_move_lock = false;
		m_turn_lock = false;
		m_activating_object = false;
		m_placing_charge = false;
	}
	else if (m_key_map->isFunction(input, "TurnLock"))
	{
		m_move_only = false;
		m_turn_only = true;
		m_move_lock = false;
		m_turn_lock = true;
		m_activating_object = false;
		m_placing_charge = false;
	}
	else if (m_key_map->isFunction(input, "Back"))
	{
		m_activating_object = false;
		m_placing_charge = false;
		m_move_only = false;
		m_turn_only = false;
		m_move_lock = false;
		m_turn_lock = false;
	}
	else if (m_key_map->isFunction(input, "Get"))
	{
		advance = doGetPickupsMenu(*this, m_key_map, model, m_msgs);
	}
	else if (m_key_map->isFunction(input, "Fire"))
	{
		DBKeyValue weapon_key( player_obj->weapon( player_obj->currentWeapon() ) );
		if (!world.objectExists(weapon_key))
		{
			m_msgs.addString("You aren't holding a weapon!");
		}
		else
		{
			shared_ptr< Weapon > weapon_obj( dynamic_pointer_cast<Weapon>( world.objectPtr( weapon_key ) ) );
			if (0 >= weapon_obj->getAmmo())
			{
				m_msgs.addString("Your weapon is empty!");
			}
			else
			{
				m_msgs.addString( "Select target:" );
				shared_ptr< SelectFireTarget > action( new SelectFireTarget(m_msgs) );
				shared_ptr< TargetSelect > newstate( new TargetSelect( interfaceStateMachine(), m_key_map, action, m_msgs, KeyCode('f', false) ) );

				DBKeyValue jump_to( findNearestInterestingThing(model, true) );
				newstate->jumpToObject(model, jump_to);

				setNextState( newstate );
				return CommandResult( false, true );
			}
		}
	}
	else if (m_key_map->isFunction(input, "Look"))
	{
		if (m_first_look_mode)
			m_msgs.addString("Use movement controls to move the cursor, and look at the bottom left of the screen to see what is under it. Press Esc to return to the game.");

		m_first_look_mode = false;

		WorldObject::WorldLocation look_loc( loc );
		DBKeyValue look_obj( findNearestInterestingThing(model, false) );
		if (world.objectExists(look_obj))
			look_loc = world.object(look_obj).location();

		OverWorld::BlockAndFloor bnf( overworld.getBlockAndFloor( look_loc.zone ) );

		shared_ptr< LookMode > newstate( new LookMode(interfaceStateMachine(), m_key_map, m_msgs, look_loc.x, look_loc.z, bnf.block, bnf.floor) );
		setNextState( newstate );
		return CommandResult( false, true );
	}
	else if (m_key_map->isFunction(input, "Sidearm"))
	{
		if (PlayerCharacter::Sidearm != player_obj->currentWeapon())
		{
			advance = true;
			shared_ptr< SwitchWeaponAction > action( new SwitchWeaponAction(player_obj, PlayerCharacter::Sidearm, 
				player_obj->getActionTime(model, player_actions::SwitchToSidearm)) );
			model.actionEngine().addAction( action );
		}
		else
			m_msgs.addString("You're already holding your sidearm!");
	}
	else if (m_key_map->isFunction(input, "Primary"))
	{
		if (world.objectExists(player_obj->weapon( PlayerCharacter::Primary )))
		{
			if (PlayerCharacter::Primary != player_obj->currentWeapon())
			{
				advance = true;
				shared_ptr< SwitchWeaponAction > action( new SwitchWeaponAction(player_obj, PlayerCharacter::Primary, 
					player_obj->getActionTime(model, player_actions::SwitchToPrimary)) );
				model.actionEngine().addAction( action );
			}
			else
				m_msgs.addString("You're already holding your primary weapon!");
		}
		else
			m_msgs.addString("You don't have a primary weapon!");
	}
	else if (m_key_map->isFunction(input, "Reload"))
	{
		DBKeyValue weapon_key( player_obj->weapon( player_obj->currentWeapon() ) );
		if (world.objectExists(weapon_key))
		{
			Weapon& weapon_obj( dynamic_cast< Weapon& >( world.object( weapon_key ) ) );
			if (weapon_obj.getAmmo() >= weapon_obj.weaponData().clip_size )
			{
				m_msgs.addString("Your weapon is already loaded!");
			}
			else
			{
				advance = true;
				shared_ptr< ReloadWeaponAction > action( new ReloadWeaponAction(player_obj, 
					player_obj->getActionTime(model, player_actions::ReloadWeapon)) );
				model.actionEngine().addAction( action );
			}
		}
		else
		{
			m_msgs.addString("You're not holding a weapon.");
		}
	}
	//TODO merge code for the below two commands
	else if (m_key_map->isFunction(input, "FloorUp"))
	{
		if (world.zoneExists(loc.zone))
		{
			Zone& zone( world.zone( loc.zone ) );
			if (terrain::StairsUp == zone.terrainAt(loc.x, loc.z))
			{
				OverWorld::BlockAndFloor bnf = overworld.getBlockAndFloor(loc.zone);
				if (bnf.floor < OverWorld::MAX_FLOORS)
				{
					DBKeyValue target_floor = overworld.getFloorKey(bnf.block, bnf.floor+1);
					if (world.zoneExists(target_floor))
					{
						//TODO add action for this
						if (player_obj->moveTo( model, WorldObject::WorldLocation( target_floor, loc.x, loc.z ), true ))
						{
							model.updateVision();
							model.updateHearing();
						}
					}
				}
			}
		}
	}
	else if (m_key_map->isFunction(input, "FloorDown"))
	{
		if (world.zoneExists(loc.zone))
		{
			Zone& zone( world.zone( loc.zone ) );
			if (terrain::StairsDown == zone.terrainAt(loc.x, loc.z))
			{
				OverWorld::BlockAndFloor bnf = overworld.getBlockAndFloor(loc.zone);
				if (bnf.floor > OverWorld::FLOOR_MIN)
				{
					DBKeyValue target_floor = overworld.getFloorKey(bnf.block, bnf.floor-1);
					if (world.zoneExists(target_floor))
					{
						//TODO add action for this
						if (player_obj->moveTo( model, WorldObject::WorldLocation( target_floor, loc.x, loc.z ), true ))
						{
							model.updateVision();
							model.updateHearing();
						}
					}
				}
			}
		}
	}
	//TODO merge code from the below 4 grenade-throwing commands
	else if (m_key_map->isFunction(input, "Frag"))
	{
		if (player_obj->num( pickup::FragGrenade ) < 1)
		{
			m_msgs.addString("You don't have any frag grenades!");
		}
		else
		{
			m_msgs.addString( "Select target:" );
			shared_ptr< SelectGrenadeTarget > action( new SelectGrenadeTarget(m_msgs, pickup::FragGrenade) );
			shared_ptr< TargetSelect > newstate( new TargetSelect( interfaceStateMachine(), m_key_map, action, m_msgs, KeyCode('r', false) ) );
			setNextState( newstate );
			return CommandResult( false, true );
		}
	}
	else if (m_key_map->isFunction(input, "Krak"))
	{
		if (player_obj->num( pickup::KrakGrenade ) < 1)
		{
			m_msgs.addString("You don't have any krak grenades!");
		}
		else
		{
			m_msgs.addString( "Select target:" );
			shared_ptr< SelectGrenadeTarget > action( new SelectGrenadeTarget(m_msgs, pickup::KrakGrenade) );
			shared_ptr< TargetSelect > newstate( new TargetSelect( interfaceStateMachine(), m_key_map, action, m_msgs, KeyCode('k', false) ) );
			setNextState( newstate );
			return CommandResult( false, true );
		}
	}
	else if (m_key_map->isFunction(input, "Stun"))
	{
		if (player_obj->num( pickup::StunGrenade ) < 1)
		{
			m_msgs.addString("You don't have any stun grenades!");
		}
		else
		{
			m_msgs.addString( "Select target:" );
			shared_ptr< SelectGrenadeTarget > action( new SelectGrenadeTarget(m_msgs, pickup::StunGrenade) );
			shared_ptr< TargetSelect > newstate( new TargetSelect( interfaceStateMachine(), m_key_map, action, m_msgs, KeyCode('t', false) ) );
			setNextState( newstate );
			return CommandResult( false, true );
		}
	}
	else if (m_key_map->isFunction(input, "Inc"))
	{
		if (player_obj->num( pickup::IncGrenade ) < 1)
		{
			m_msgs.addString("You don't have any inc grenades!");
		}
		else
		{
			m_msgs.addString( "Select target:" );
			shared_ptr< SelectGrenadeTarget > action( new SelectGrenadeTarget(m_msgs, pickup::IncGrenade) );
			shared_ptr< TargetSelect > newstate( new TargetSelect( interfaceStateMachine(), m_key_map, action, m_msgs, KeyCode('i', false) ) );
			setNextState( newstate );
			return CommandResult( false, true );
		}
	}
	else if (m_key_map->isFunction(input, "Demolition"))
	{
		if (player_obj->num( pickup::DemoCharge ) < 1)
		{
			m_msgs.addString("You don't have any demo charges!");
		}
		else
		{
			m_msgs.addString("Press direction to blast:");
			m_placing_charge = true;
			m_activating_object = false;
		}
	}
	//else if (m_key_map->isFunction(input, "Armour"))
	//{
	//}
	else if (m_key_map->isFunction(input, "Drop"))
	{
        shared_ptr< SelectDropItem > selector( new SelectDropItem(m_msgs, model, world, overworld, player_obj) );
        DropDialog::ColourScheme scheme( model.isCountdownActive() ? DropDialog::Red : DropDialog::Green );
        shared_ptr< DropDialog > dialog( new DropDialog( interfaceStateMachine(), m_key_map, selector, scheme ) );
    	setNextState( dialog );
		return CommandResult( false, true );
	}
	else if (m_key_map->isFunction(input, "Neutraliser"))
	{
		if (player_obj->canUseItem(pickup::Neutraliser))
		{
			if (player_obj->isAcidSplashed(model))
			{
				advance = true;
				shared_ptr< UseItemAction > use_action( new UseItemAction(player_obj, pickup::Neutraliser, 
					player_obj->getActionTime(model, player_actions::UseNeutraliser)) );
				model.actionEngine().addAction( use_action );
			}
			else
				m_msgs.addString("There's nothing to neutralise.");
		}
		else
		{
			m_msgs.addString("You don't have any neutraliser.");
		}
	}
	else if (m_key_map->isFunction(input, "Medkit"))
	{
		if (player_obj->canUseItem(pickup::Medkit))
		{
			if (player_obj->getHP() < player_obj->getTopHP())
			{
				advance = true;
				bool plus = player_obj->getHP() == player_obj->getMaxHP();
				GameTimeCoordinate use_time = player_obj->getActionTime(model, plus ? player_actions::UseMedkit : player_actions::UseMedkitPlus);
				if (player_obj->hasPerk(player_perks::LoseLessMaxHP))
					use_time /= 2;
				shared_ptr< UseItemAction > use_action( new UseItemAction(player_obj, pickup::Medkit, use_time) );
				model.actionEngine().addAction( use_action );
			}
			else
				m_msgs.addString("You'd rather not use that stuff if you don't have to...");
		}
		else
		{
			m_msgs.addString("You don't have a medkit.");
		}
	}
	else if (m_key_map->isFunction(input, "ScrollDown"))
	{
		m_msgs.scrollDown();
	}
	else if (m_key_map->isFunction(input, "ScrollUp"))
	{
		m_msgs.scrollUp();
	}
	else if (m_key_map->isFunction(input, "Operate"))
	{
		//TODO move this block of code out.
		bool found_terminal( false );
		if (world.zoneExists(loc.zone))
		{
			const Zone& player_zone( world.zone( loc.zone ) );
			if (player_zone.isWithin( loc.x, loc.z ))
			{
				const ObjectList& objects( player_zone.objectsAt( loc.x, loc.z ) );
				BOOST_FOREACH( DBKeyValue obj_key, objects )
				{
					if (world.objectExists(obj_key))
					{
						const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( world.object(obj_key) ) );
						if (game_obj.type() == objects::Terminal)
						{
							found_terminal = true;
							const Terminal& terminal( dynamic_cast< const Terminal& >(game_obj) );
							if (!terminal.isBroken())
							{
								if (terminal::Info == terminal.terminalType())
								{
									OverWorld::BlockAndFloor bnf = overworld.getBlockAndFloor(loc.zone);
									if (!overworld.isMapped(bnf.block, bnf.floor))
									{
										shared_ptr< UseTerminalAction > action( new UseTerminalAction( player_obj, terminal::Info, 
											player_obj->getActionTime(model, player_actions::UseTerminal) ) );
										model.actionEngine().addAction( action );
										advance = true;
										m_msgs.addString("Downloading map...");
									}
									else
									{
										m_msgs.addString("You have already downloaded the map for this floor.");
									}
								}
								else if (terminal::Master == terminal.terminalType())
								{
									if (model.usedMasterTerminal())
									{
										m_msgs.addString("You have already accessed the master terminal... There's nothing more you can do with it.");
									}
									else
									{
										shared_ptr< UseTerminalAction > action( new UseTerminalAction( player_obj, terminal::Master, 
											player_obj->getActionTime(model, player_actions::UseTerminal) ) );
										model.actionEngine().addAction( action );
										advance = true;
										m_msgs.addString("Accessing master terminal...");
									}
								}
							}
							else
							{
								m_msgs.addString("You can't use this terminal; it's totalled.");
							}
						}
					}
				}
			}
		}

		if (!found_terminal)
		{
			if (!m_activating_object)
				m_msgs.addString("Press direction of object to operate:");
			m_activating_object = true;
		}
	}
	else if (m_key_map->isFunction(input, "Char"))
	{
		shared_ptr< CharScreen > newstate( new CharScreen( interfaceStateMachine(), m_key_map ) );
		setNextState( newstate );
		return CommandResult( false, true );
	}
	else
	if (m_key_map->isFunction(input, "Buy"))
	{
		shared_ptr< BuyPerks > newstate( new BuyPerks( interfaceStateMachine(), m_key_map ) );
		setNextState( newstate );
		return CommandResult( false, true );
	}
	else if (m_key_map->isFunction(input, "Save"))
	{
		if (saveGame(model, getSaveFileName(model)))
		{
			m_quit = true;
		}
	}

	if (move)
	{
		advance = true;
		if (m_activating_object || m_placing_charge)
		{
			WorldObject::WorldLocation tloc = loc;
			tloc.x += mx;
			tloc.z += mz;

			TerrainType there( world.zone( tloc.zone ).terrainAt( tloc.x, tloc.z ) );

			if (m_activating_object)
			{
				if ((terrain::Door == terrain::getType(there)) && !terrain::isBroken(there))
				{
					if (0 == (there & terrain::Open))
					{
						shared_ptr< OpenDoorAction > open_action( new OpenDoorAction(player_obj, tloc, 
							player_obj->getActionTime(model, player_actions::OpenDoor)) );
						model.actionEngine().addAction( open_action );
						m_msgs.addString("You activate the door.");
					}
					else
					{
						shared_ptr< CloseDoorAction > close_action( new CloseDoorAction(player_obj, tloc, 
							player_obj->getActionTime(model, player_actions::CloseDoor)) );
						model.actionEngine().addAction( close_action );
						m_msgs.addString("You activate the door.");
					}
				}
				else
				{
					m_msgs.addString("There's nothing to activate.");
				}
			}
			else if (m_placing_charge)
			{
				if ((mx != 0) && (mz != 0))
				{
					m_msgs.addString("You can't blast diagonally.");
				}
				else if ((terrain::Wall == terrain::getType(there)) && !terrain::isBroken(there))
				{
					advance = true;
					shared_ptr< SetChargeAction > use_action( new SetChargeAction(
						player_obj, player_obj->location(), mx, mz, 
						player_obj->getActionTime(model, player_actions::SetCharge)) );
					model.actionEngine().addAction( use_action );
				}
				else
				{
					m_msgs.addString("There's nothing there to blast.");
				}
			}
		}
		else
		{
			if (!m_turn_only)
			{
				if (INVALID_KEY != loc.zone)
				{
					WorldObject::WorldLocation tloc = loc;
					tloc.x += mx;
					tloc.z += mz;

					const Zone& target_zone( world.zone( tloc.zone ) );
					bool cancel( false );
					//TODO this block of code should REALLY not be in here. Move out.
					if (!target_zone.isWithin( tloc.x, tloc.z ))
					{
						cancel = true;
						OverWorld::BlockAndFloor bnf = overworld.getBlockAndFloor(tloc.zone);
						int search_x(0), search_z(0);
						if ((-1 != bnf.block) && (-1 != bnf.floor))
						{
							if (tloc.z < 0)
							{	//south
								int tblock = overworld.exitsToBlock(bnf.block, bnf.floor, OverWorld::South);
								tloc.zone = overworld.getFloorKey(tblock, bnf.floor);
								if (world.zoneExists(tloc.zone))
								{
									tloc.z = world.zone(tloc.zone).sizeZ()-1;
									search_x = 1;
									cancel = false;
								}
							}
							else if (tloc.z >= target_zone.sizeZ())
							{	//north
								int tblock = overworld.exitsToBlock(bnf.block, bnf.floor, OverWorld::North);
								tloc.zone = overworld.getFloorKey(tblock, bnf.floor);
								if (world.zoneExists(tloc.zone))
								{
									tloc.z = 0;
									search_x = 1;
									cancel = false;
								}
							}
							else if (tloc.x < 0)
							{	//west
								int tblock = overworld.exitsToBlock(bnf.block, bnf.floor, OverWorld::West);
								tloc.zone = overworld.getFloorKey(tblock, bnf.floor);
								if (world.zoneExists(tloc.zone))
								{
									tloc.x = world.zone(tloc.zone).sizeX()-1;
									search_z = 1;
									cancel = false;
								}
							}
							else if (tloc.x >= target_zone.sizeX())
							{	//east
								int tblock = overworld.exitsToBlock(bnf.block, bnf.floor, OverWorld::East);
								tloc.zone = overworld.getFloorKey(tblock, bnf.floor);
								if (world.zoneExists(tloc.zone))
								{
									tloc.x = 0;
									search_z = 1;
									cancel = false;
								}
							}
						}

						if (!cancel)
						{	//the bridge openings might not line up, so find the nearest space the player can move to.
							const Zone& new_target_zone( world.zone( tloc.zone ) );
							for (int i=0; ; ++i)
							{
								WorldObject::WorldLocation try_loc1( tloc.zone, tloc.x + (i*search_x), tloc.z + (i*search_z) );
								WorldObject::WorldLocation try_loc2( tloc.zone, tloc.x - (i*search_x), tloc.z - (i*search_z) );
								if (!new_target_zone.isWithin(try_loc1.x, try_loc1.z))
									if (!new_target_zone.isWithin(try_loc2.x, try_loc2.z))
									{
										cancel = true;
										break;
									}
								TerrainType there1( new_target_zone.terrainAt( try_loc1.x, try_loc1.z ) );
								if (player_obj->canOverlap( there1 ))
								{
									tloc = try_loc1;
									break;
								}
								TerrainType there2( new_target_zone.terrainAt( try_loc2.x, try_loc2.z ) );
								if (player_obj->canOverlap( there2 ))
								{
									tloc = try_loc2;
									break;
								}
							}
						}
					}

					if (cancel)
					{
						advance = !m_move_only;
					}
					else
					{
						TerrainType there( world.zone( tloc.zone ).terrainAt( tloc.x, tloc.z ) );
						if (player_obj->canOverlap( there ))
						{
							GameTimeCoordinate move_time( (mx != 0) && (mz != 0) ? 
								player_obj->getActionTime(model, player_actions::MoveDiagonal) : 
								player_obj->getActionTime(model, player_actions::Move) );
							shared_ptr< DiscreteMoveAction > move_action( new DiscreteMoveAction(player_obj, tloc, move_time) );
							model.actionEngine().addAction( move_action );
						}
						else
						{
							if ((terrain::Door == terrain::getType(there)) && !terrain::isBroken(there))
							{
								shared_ptr< OpenDoorAction > open_action( new OpenDoorAction(player_obj, tloc, 
									player_obj->getActionTime(model, player_actions::OpenDoor)) );
								model.actionEngine().addAction( open_action );
								m_msgs.addString("You activate the door.");
							}
						}
					}
				}
			}
			if (!m_move_only)
			{
				const int16_t angles[3][3] = { {-135, -90, -45}, {180, 0, 0}, {135, 90, 45} };
				int16_t heading = angles[mz+1][mx+1];
				shared_ptr< DiscreteTurnAction > turn_action( 
					new DiscreteTurnAction(player_obj, heading, 
						player_obj->getActionTime(model, player_actions::Turn)) );
				model.actionEngine().addAction( turn_action );
			}
		}
		if (!m_turn_lock)
			m_turn_only = false;
		if (!m_move_lock)
			m_move_only = false;
		m_activating_object = false;
		m_placing_charge = false;
	}

	return CommandResult( advance, true );
}

void PlayingGame::notifyAHGameModelAdvance( RL_shared::AGameModel& in_model, RL_shared::GameTimeCoordinate dt, bool is_current_state )
{
	if (is_current_state)
	{
		AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
		World& world( model.world() );

		if (!world.objectExists(model.avatar()))
		{
			if (!m_shown_death_msg)
			{
				m_shown_death_msg = true;
				m_msgs.beginNewMessage();
				m_msgs.addString("You are dead. Press any key to continue.");

				writeMortem(in_model);
			}
			return;
		}

		if (model.worldExplodes())
		{
			shared_ptr< Kaboom > newstate( new Kaboom( interfaceStateMachine() ) );
			setNextState( newstate );
			m_quit = true;

			if (world.objectExists(model.avatar()))
			{
				PlayerCharacter& player( dynamic_cast< PlayerCharacter& >( world.object(model.avatar()) ) );
				player.writeOutcome(model, "Obliterated in a massive nuclear explosion");
			}

			writeMortem(in_model);

			return;
		}
		if (model.usedMasterTerminal() && 
			(!m_shown_master_text))
		{
			m_shown_master_text = true;
			shared_ptr< MessageBox > newstate( new MessageBox(interfaceStateMachine(), m_key_map, used_master_terminal_text, 13, 10, 66, 30, model.isCountdownActive() ? MessageBox::Red : MessageBox::Green, 1) );
			setNextState( newstate );
			return;
		}
		if (model.isQueenFound() && 
			(!m_shown_found_queen_text))
		{
			m_shown_found_queen_text = true;
			shared_ptr< MessageBox > newstate( new MessageBox(interfaceStateMachine(), m_key_map, found_queen_text, 17, 14, 63, 25, model.isCountdownActive() ? MessageBox::Red : MessageBox::Green, 1) );
			setNextState( newstate );
			return;
		}
		if (model.isQueenDead() && 
			(!m_shown_killed_queen_text))
		{
			m_shown_killed_queen_text = true;
			shared_ptr< MessageBox > newstate( new MessageBox(interfaceStateMachine(), m_key_map, killed_queen_text, 11, 14, 69, 25, model.isCountdownActive() ? MessageBox::Red : MessageBox::Green, 1) );
			setNextState( newstate );

			m_msgs.addString("Return to the colony entrance to escape.");

			return;
		}
		if (model.isCountdownActive() && 
			(!m_shown_countdown_start_text))
		{
			m_shown_countdown_start_text = true;
			shared_ptr< MessageBox > newstate( new MessageBox(interfaceStateMachine(), m_key_map, started_countdown_text, 14, 12, 64, 27, MessageBox::Red, 1) );
			setNextState( newstate );

			m_msgs.addString("Return to the colony entrance to escape.");

			return;
		}
		if (model.hasPlayerEscaped())
		{
			if (!m_shown_ending_text_1)
			{
				m_good_ending = model.doesPlayerGetTheGoodEnding();

				if (world.objectExists(model.avatar()))
				{
					PlayerCharacter& player( dynamic_cast< PlayerCharacter& >( world.object(model.avatar()) ) );
					if (m_good_ending)
						player.writeOutcome(model, "Escaped the infested colony and then... who knows?");
					else
						player.writeOutcome(model, "Escaped, but obliterated in the reactor explosion");
				}

				writeMortem(in_model);

				m_shown_ending_text_1 = true;

				shared_ptr< StoryText > newstate( new StoryText(interfaceStateMachine(), ending_text_1, 8) );
				setNextState( newstate );

				return;
			}
		}
	}
}

bool PlayingGame::finished(void)
{
	return m_quit;
}

void PlayingGame::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );
	const AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	const World& world( model.world() );

	console.clearScreen();

	drawFrame(console, model.isCountdownActive());

	if (model.isCountdownActive())
		m_msgs.draw(console, Console::BrightRed, Console::Red);
	else
		m_msgs.draw(console, Console::BrightGreen, Console::Green);

	DBKeyValue player_key( model.avatar() );
	if (!world.objectExists(player_key))
		return;

	const PlayerCharacter& player_obj( dynamic_cast<const PlayerCharacter&>( world.object( player_key ) ) );
	WorldObject::WorldLocation loc( player_obj.location() );

	drawWorld(console, model, loc.zone, loc.x, loc.z, loc.x, loc.z, 
		WORLD_VIEW_X_LOW, WORLD_VIEW_X_HI, WORLD_VIEW_Y_LOW, WORLD_VIEW_Y_HI, 
		cursor_type::None, 0, 0, false, 
		player_obj.hasMotionDetection(model) );

	drawZoneName(console, model, loc.zone, model.isCountdownActive());

	GameTimeCoordinate time_step = 0;//model.gameTime() - m_last_input;
	drawHUD(console, model, HUD_X_LOW, HUD_Y_LOW, HUD_X_HI, HUD_Y_HI, time_step);
}

bool PlayingGame::drawsWholeWindow(void) const
{
	return true;
}

void PlayingGame::writeMortem( AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast< AHGameModel& >( in_model ) );

	try
	{
		std::ofstream fout( "mortem.txt" );
		if (fout.good())
		{
			fout << model.mortem();
		}
	}
	catch(...) //TODO signal in some fashion if there was a file exception
	{
	}
}

void PlayingGame::showMortem()
{
	if (m_shown_mortem)
		return;

	try
	{
		std::ifstream fin( "mortem.txt" );
		if (fin.good())
		{
			//ugh.
			std::string temp((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
			std::istringstream issin(temp);

			shared_ptr< ViewFile > newstate( new ViewFile( 
					interfaceStateMachine(), m_key_map, issin, ViewFile::Normal 
					) 
				);
			setNextState( newstate );
		}
	}
	catch(...)
	{
	}

	m_shown_mortem = true;
}

}
