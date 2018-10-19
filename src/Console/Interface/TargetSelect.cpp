#include "TargetSelect.hpp"
#include "draw.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "../../Model/Objects/TerrainType.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include "ConsoleView/KeyMap.hpp"
#include "Messages/Console/MessageDisplay.hpp"
#include "assert.hpp"


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



TargetSelect::TargetSelect( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	boost::shared_ptr< ITargetSelectCallback > callback, 
	MessageDisplay& msgs, const KeyCode& initiating_key
	)
: InterfaceState(ism), m_key_map(keys), m_init_key(initiating_key), m_msgs(msgs), m_callback(callback), m_tx(0), m_tz(0), m_done(false)
{
	ASSERT( m_callback );
	ASSERT( m_key_map );
}

void TargetSelect::enterFromParent( AGameModel& )
{
}
void TargetSelect::exitToParent( AGameModel& )
{
}
void TargetSelect::enterFromChild( AGameModel& )
{
}
void TargetSelect::exitToChild( AGameModel& )
{
}

void TargetSelect::jumpToObject( const AHGameModel& model, RL_shared::DBKeyValue object )
{
	const World& world( model.world() );
	if (world.objectExists(object) && world.objectExists(model.avatar()))
	{
		WorldObject::WorldLocation loc_targ( world.object(object).location() );
		WorldObject::WorldLocation loc_base( world.object(model.avatar()).location() );

		int tx = loc_targ.x - loc_base.x;
		int tz = loc_targ.z - loc_base.z;

		if ((tx >= -16) && (tx <= 16) && (tz >= -16) && (tz <= 16))
		{
			m_tx = tx;
			m_tz = tz;
		}
	}
}

TargetSelect::CommandResult TargetSelect::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	const KeyPress& keypress( dynamic_cast<const KeyPress&>(input) );
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	World& world( model.world() );

	if (!world.objectExists(model.avatar()))
	{
		m_done = true;
		return CommandResult(false, true);
	}

	shared_ptr< PlayerCharacter > player_obj( dynamic_pointer_cast<PlayerCharacter>( world.objectPtr( model.avatar() ) ) );
	ASSERT( player_obj );
	WorldObject::WorldLocation loc( player_obj->location() );

	int mx(0), mz(0);
	bool advance( false );
	bool redraw( false );

	if (m_key_map->isFunction(input, "Back"))
	{
		m_done = true;
	}
	else if ((m_key_map->isFunction(input, "OK")) || (keypress.value == m_init_key))
	{
		std::pair< bool, bool > result = m_callback->select(model, m_tx, m_tz);
		m_done = result.first;
		advance = result.second;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "Left"))
	{
		mx = -1;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "Right"))
	{
		mx = 1;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "Up"))
	{
		mz = 1;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "Down"))
	{
		mz = -1;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "UpAndLeft"))
	{
		mx = -1;
		mz = 1;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "UpAndRight"))
	{
		mx = 1;
		mz = 1;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "DownAndLeft"))
	{
		mx = -1;
		mz = -1;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "DownAndRight"))
	{
		mx = 1;
		mz = -1;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "ScrollDown"))
	{
		m_msgs.scrollDown();
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "ScrollUp"))
	{
		m_msgs.scrollUp();
		redraw = true;
	}

	m_tx += mx;
	m_tz += mz;

	return CommandResult( advance, redraw );
}

bool TargetSelect::finished(void)
{
	return m_done;
}

void TargetSelect::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );
	const AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	const World& world( model.world() );

	DBKeyValue player_key( model.avatar() );
	ASSERT( INVALID_KEY != player_key );
	const PlayerCharacter& player_obj( dynamic_cast<const PlayerCharacter&>( world.object( player_key ) ) );
	WorldObject::WorldLocation loc( player_obj.location() );

	console.clearScreen();
	drawFrame(console, model.isCountdownActive());
	drawWorld(
		console, model, loc.zone, 
		loc.x, loc.z, 
		loc.x, loc.z, 
		WORLD_VIEW_X_LOW, WORLD_VIEW_X_HI, WORLD_VIEW_Y_LOW, WORLD_VIEW_Y_HI, 
		cursor_type::Target, m_tx, m_tz, false, 
		player_obj.hasMotionDetection(model) 
		);

	drawZoneName(console, model, loc.zone, model.isCountdownActive());

	writeSelectedItems(console, model, loc.zone, 
		loc.x + m_tx, loc.z + m_tz, 
		loc.x, loc.z,
		model.isCountdownActive(), 
		player_obj.hasMotionDetection(model), 
		false
		);

	if (model.isCountdownActive())
		m_msgs.draw(console, Console::BrightRed, Console::Red);
	else
		m_msgs.draw(console, Console::BrightGreen, Console::Green);

	drawHUD(console, model, HUD_X_LOW, HUD_Y_LOW, HUD_X_HI, HUD_Y_HI, 0);
}
bool TargetSelect::drawsWholeWindow(void) const
{
	return true;
}



}
