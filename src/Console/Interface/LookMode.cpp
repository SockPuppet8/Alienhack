#include "LookMode.hpp"
#include "draw.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "../../Model/Objects/TerrainType.hpp"
#include "../../Model/OverWorld.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include "Input/IFunctionMap.hpp"
#include "Console/Console.hpp"
#include "Messages/Console/MessageDisplay.hpp"
#include "assert.hpp"


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



LookMode::LookMode( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	MessageDisplay& msgs, 
	int x, int z, int block, int floor
	)
: InterfaceState(ism), m_key_map(keys), m_msgs(msgs), 
m_lx(x), m_lz(z), m_lb(block), m_lf(floor), m_done(false)
{
	ASSERT( m_key_map );
}

void LookMode::enterFromParent( AGameModel& )
{
}
void LookMode::exitToParent( AGameModel& )
{
}
void LookMode::enterFromChild( AGameModel& )
{
}
void LookMode::exitToChild( AGameModel& )
{
}

LookMode::CommandResult LookMode::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	World& world( model.world() );

	//if (!world.objectExists(model.avatar()))
	//{
	//	m_done = true;
	//	return CommandResult(false, true);
	//}

	OverWorld& overworld( model.overworld() );

	int mx(0), mz(0);
	bool redraw( false );

	if (m_key_map->isFunction(input, "Left"))
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
	else if (m_key_map->isFunction(input, "Back"))
	{
		m_done = true;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "FloorDown"))
	{
		if (m_lf > OverWorld::FLOOR_MIN)
		{
			if (overworld.isVisited(m_lb, m_lf-1))
			{
				--m_lf;
				redraw = true;
			}
		}
	}
	else if (m_key_map->isFunction(input, "FloorUp"))
	{
		if (m_lf < OverWorld::MAX_FLOORS)
		{
			if (overworld.isVisited(m_lb, m_lf+1))
			{
				++m_lf;
				redraw = true;
			}
		}
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

	int old_lx = m_lx;
	int old_lz = m_lz;

	m_lx += mx;
	m_lz += mz;

	DBKeyValue look_zone( overworld.getFloorKey( m_lb, m_lf ) );
	if (world.zoneExists(look_zone))
	{
		const Zone& zone( world.zone( look_zone ) );

		int offx = (m_lx < 0) ? -1 : (m_lx >= zone.sizeX()) ? 1 : 0;
		int offz = (m_lz < 0) ? -1 : (m_lz >= zone.sizeZ()) ? 1 : 0;
		if ((offx != 0) || (offz != 0))
		{
			int nblock( overworld.getNeighbouringBlock(m_lb, offx, offz) );
			DBKeyValue nzone( INVALID_KEY );

			bool can_scroll(true);

			if ((nblock < 0) || (nblock >= OverWorld::NUM_BLOCKS) || (!overworld.isVisited(nblock, m_lf)))
				can_scroll = false;
			else
			{
				nzone = overworld.getFloorKey(nblock, m_lf);
				if (!world.zoneExists(nzone))
					can_scroll = false;
			}

			if (can_scroll)
			{
				m_lb = nblock;
				const Zone& zone( world.zone( nzone ) );
				if (offx < 0)
					m_lx = zone.sizeX()-1;
				else if (offx > 0)
					m_lx = 0;
				if (offz < 0)
					m_lz = zone.sizeZ()-1;
				else if (offz > 0)
					m_lz = 0;

				m_lx = (std::max)(0, (std::min)(zone.sizeX()-1, m_lx));
				m_lz = (std::max)(0, (std::min)(zone.sizeZ()-1, m_lz));
			}
			else
			{
				m_lx = (0 == offx) ? m_lx : old_lx;
				m_lz = (0 == offz) ? m_lz : old_lz;
			}
		}
	}

	return CommandResult( false, redraw );
}

bool LookMode::finished(void)
{
	return m_done;
}

void LookMode::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );
	const AHGameModel& model( dynamic_cast<AHGameModel&>(in_model) );
	const World& world( model.world() );
	const OverWorld& overworld( model.overworld() );

	console.clearScreen();
	drawFrame(console, model.isCountdownActive());

	{
		WorldObject::WorldLocation loc;
		bool has_motion_tracking(false);
		bool is_current_zone(false);

		if (world.objectExists(model.avatar()))
		{
			const PlayerCharacter& player_obj( dynamic_cast<const PlayerCharacter&>( world.object( model.avatar() ) ) );
			loc = player_obj.location();
			has_motion_tracking = player_obj.hasMotionDetection(model);
			is_current_zone = (overworld.getFloorKey(m_lb, m_lf) == loc.zone);
		}


		DBKeyValue look_zone( overworld.getFloorKey( m_lb, m_lf ) );

		if (world.zoneExists(look_zone))
		{
			const Zone& look_zone_obj( world.zone(look_zone) );

			drawWorld(
				console, model, look_zone, 
				m_lx, m_lz, 
				loc.x, loc.z, 
				WORLD_VIEW_X_LOW, WORLD_VIEW_X_HI, WORLD_VIEW_Y_LOW, WORLD_VIEW_Y_HI, 
				cursor_type::Look, 0, 0,
				!is_current_zone, 
				has_motion_tracking
				);

			drawZoneName(console, model, look_zone, model.isCountdownActive());

			writeSelectedItems(
				console, model, 
				look_zone, m_lx, m_lz, 
				loc.x, loc.z, 
				model.isCountdownActive(), 
				has_motion_tracking,
				( 0 != look_zone_obj.recordedObjectAt(m_lx, m_lz) )
				);
		}
	}

	if (model.isCountdownActive())
		m_msgs.draw(console, Console::BrightRed, Console::Red);
	else
		m_msgs.draw(console, Console::BrightGreen, Console::Green);

	drawHUD(console, model, HUD_X_LOW, HUD_Y_LOW, HUD_X_HI, HUD_Y_HI, 0);
}
bool LookMode::drawsWholeWindow(void) const
{
	return true;
}



}
