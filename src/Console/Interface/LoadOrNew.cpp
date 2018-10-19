#include "LoadOrNew.hpp"
#include "../../Model/save.hpp"
#include "../../Model/AHGameModel.hpp"
#include "Input/IFunctionMap.hpp"
#include "Console/Console.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



LoadOrNew::LoadOrNew( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, boost::shared_ptr< RL_shared::IFunctionMap > keys )
: InterfaceState( ism )
, m_key_map(keys)
, m_selected(0)
, m_done(false)
, m_loaded(false)
, m_cancelled(false)
{
	ASSERT( m_key_map );
}

void LoadOrNew::enterFromParent( AGameModel& )
{
}
void LoadOrNew::exitToParent( AGameModel& )
{
}
void LoadOrNew::enterFromChild( AGameModel& )
{
}
void LoadOrNew::exitToChild( AGameModel& )
{
}

LoadOrNew::CommandResult LoadOrNew::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	bool advance( false );
	bool redraw( false );

	if (!m_done)
	{
		if (m_key_map->isFunction(input, "Back"))
		{
			m_cancelled = true;
			redraw = true;
			m_done = true;
		}
		else if (m_key_map->isFunction(input, "OK"))
		{
			AHGameModel& model( dynamic_cast< AHGameModel& >( in_model ) );
			if (0 == m_selected)
			{
				if (loadGame( model, getSaveFileName(model) ))
				{
					m_loaded = true;
					redraw = true;
					m_done = true;
				}
				//TODO add error message in case of failure!
			}
			else
			{
				m_loaded = false;
				redraw = true;
				m_done = true;
			}
		}
		else if (m_key_map->isFunction(input, "Left"))
		{
			m_selected = 0;
			redraw = true;
		}
		else if (m_key_map->isFunction(input, "Right"))
		{
			m_selected = 1;
			redraw = true;
		}
	}

	return CommandResult( advance, redraw );
}

bool LoadOrNew::finished(void)
{
	return m_done;
}

void LoadOrNew::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );

	int loc_x = 45;
	int loc_y = 28;

	console.drawText(loc_x-1, loc_y-1, "...................", Console::Blue);
	console.drawText(loc_x,   loc_y  ,  " Load   New Game " , Console::Green);
	console.drawText(loc_x-1, loc_y+1, "...................", Console::Blue);
	console.draw(loc_x-1, loc_y, '.', Console::Blue);
	console.draw(loc_x+17, loc_y, '.', Console::Blue);

	if (0 == m_selected)
	{
		console.drawText(loc_x, loc_y, "<Load>", Console::BrightYellow);
	}
	else
	{
		console.drawText(loc_x+7, loc_y, "<New Game>", Console::BrightYellow);
	}
}

bool LoadOrNew::drawsWholeWindow(void) const
{
	return false;
}



}
