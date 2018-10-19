#include "GameTypeMenu.hpp"
#include "../../Model/AHGameModel.hpp"
#include "Input/IFunctionMap.hpp"
#include "Console/Console.hpp"
#include "assert.hpp"


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



GameTypeMenu::GameTypeMenu( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, boost::shared_ptr< RL_shared::IFunctionMap > keys )
: InterfaceState( ism )
, m_key_map(keys)
, m_selected(1)
, m_done(false)
{
	ASSERT( m_key_map );
}

void GameTypeMenu::enterFromParent( AGameModel& )
{
}
void GameTypeMenu::exitToParent( AGameModel& )
{
}
void GameTypeMenu::enterFromChild( AGameModel& )
{
}
void GameTypeMenu::exitToChild( AGameModel& )
{
}

GameTypeMenu::CommandResult GameTypeMenu::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	bool advance( false );
	bool redraw( false );
	AHGameModel& model( dynamic_cast< AHGameModel& >( in_model ) );

	if (!m_done)
	{
		if (m_key_map->isFunction(input, "OK"))
		{
			model.clear();
			if (0 == m_selected)
			{
				model.setGameType( AHGameModel::Quick );
			}
			else
			{
				model.setGameType( AHGameModel::Normal );
			}
			redraw = true;
			m_done = true;
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

bool GameTypeMenu::finished(void)
{
	return m_done;
}

void GameTypeMenu::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );

	int loc_x = 45-9;
	int loc_y = 28;

	console.drawText(loc_x+1, loc_y-1, "..........................", Console::Blue);
	console.drawText(loc_x+2,   loc_y  ,  " Quick Game   Long Game " , Console::Green);
	console.drawText(loc_x+1, loc_y+1, "..........................", Console::Blue);
	console.draw(loc_x+1, loc_y, '.', Console::Blue);
	console.draw(loc_x+26, loc_y, '.', Console::Blue);

	if (0 == m_selected)
	{
		console.drawText(loc_x+2, loc_y, "<Quick Game>", Console::BrightYellow);
	}
	else
	{
		console.drawText(loc_x+15, loc_y, "<Long Game>", Console::BrightYellow);
	}
}

bool GameTypeMenu::drawsWholeWindow(void) const
{
	return false;
}



}
