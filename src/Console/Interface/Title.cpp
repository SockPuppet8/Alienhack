#include "Title.hpp"
#include "PlayingGame.hpp"
#include "LoadOrNew.hpp"
#include "GameTypeMenu.hpp"
#include "draw.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/save.hpp"
#include "ConsoleView/KeyMap.hpp"
#include "assert.hpp"
#include "../../Model/version.hpp"
#include <string>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


namespace
{
	const int BACKGROUND_W = 78;
	const int BACKGROUND_H = 38;

	const int title_base = 15;
	const int aty = title_base+4;
	const int atx = 40;
	const int title_x = atx - 23;
}


Title::Title( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism )
: InterfaceState(ism), m_done(false)
{
	m_key_map.reset( new KeyMapFile("keys.txt") );
	makeBackground();
}

void Title::makeBackground(void)
{
	m_background.reset( new std::vector<char> );

	m_background->resize(BACKGROUND_W*BACKGROUND_H);

	int ar = 32;
	int arsq = ar*ar;
	int zi = 24;
	int arsqi = (ar-zi)*(ar-zi);
	int arsqd = arsq - arsqi;
	int lr = 36;
	int lsq = lr*lr;
	int range = (std::max)(ar, lr);
	//int xmin = (std::max)(1, atx-range);
	//int xmax = (std::min)(78, atx+range);
	std::vector<char>::iterator iter( m_background->begin() );
	std::vector<char>::iterator end( m_background->end() );
	for (int ty=1; ty<(BACKGROUND_H+1); ++ty)
		for (int tx=1; (tx<(BACKGROUND_W+1)) & (iter != end); ++tx, ++iter)
		{
			*iter = ' ';

			int dx = atx-tx;
			int dy = aty-ty;
			int ds = (dx*dx)+(dy*dy);
			if (ds < arsq)
			{
				if (ds > arsqi)
				{
					if ((arsqi + (rand() % arsqd)) < ds)
					{
						if (0 == (rand() & 7))
							*iter = '%';
						else if (ds <= lsq)
							*iter = '.';
						continue;
					}
				}

				int rt = rand() & 3;
				if (rt < 1)
					*iter = 'a';
				else if (rt < 2)
					*iter = 'A';
				else if (0 == (rand() & 15))
					*iter = '%';
				else if (ds <= lsq)
					*iter = '.';
			}
			else if (ds <= lsq)
				*iter = '.';
		}
}

void Title::enterFromParent( AGameModel& )
{
}
void Title::exitToParent( AGameModel& )
{
}
void Title::enterFromChild( AGameModel& in_model )
{
	if (!m_background)
		makeBackground();

	AHGameModel& model( dynamic_cast< AHGameModel& >( in_model ) );

	if (m_game_menu)
	{
		m_game_menu.reset();

		if (isSaveFile( getSaveFileName(model) ))
		{
			m_load_menu.reset( new LoadOrNew( interfaceStateMachine(), m_key_map ) );
			setNextState( m_load_menu );
		}
		else
		{
			startGame(false);
		}
	}
	else
	if (m_load_menu)
	{
		if (!m_load_menu->cancelled())
		{
			startGame( m_load_menu->loadedGame() );
		}
		else
		{
			m_game_menu.reset( new GameTypeMenu( interfaceStateMachine(), m_key_map ) );
			setNextState( m_game_menu );
		}
		m_load_menu.reset();
	}
}
void Title::exitToChild( AGameModel& )
{
}

void Title::startGame( bool loaded )
{
	m_background.reset();

	shared_ptr< PlayingGame > newstate( new PlayingGame( interfaceStateMachine(), m_key_map, loaded ) );
	setNextState( newstate );
}

Title::CommandResult Title::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	ASSERT(!m_load_menu);

	if (m_key_map->isFunction(input, "Back"))
	{
		m_done = true;
	}
	else if (m_key_map->isFunction(input, "OK"))
	{
		m_game_menu.reset( new GameTypeMenu( interfaceStateMachine(), m_key_map ) );
		setNextState( m_game_menu );
		return CommandResult( false, true );
	}

	return CommandResult( false, false );
}

bool Title::finished(void)
{
	return m_done;
}

namespace
{
	const Console::Colour floor_col = Console::Blue;

	void drawTitleLine(Console& console, int x, int y, const char * text, Console::Colour col)
	{
		for (; *text;)
		{
			if ('*' != *text)
				if ('.' == *text)
					console.draw(x, y, '.', floor_col);
				else
					console.draw(x, y, *text, col);

			++text;
			++x;
		}
	}
}

void Title::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );

	console.clearScreen();
	drawOuterFrame(console, false);

	std::vector<char>::const_iterator iter( m_background->begin() );
	std::vector<char>::const_iterator end( m_background->end() );
	for (int ty=1; ty < (BACKGROUND_H+1); ++ty)
		for (int tx=1; (tx < (BACKGROUND_W+1)) && (iter != end); ++tx, ++iter)
		{
			switch (*iter)
			{
				case 'A':
				case 'a':
				case '%':
					console.draw(tx, ty, *iter, Console::Green);
					break;
				case '.': 
					console.draw(tx, ty, *iter, floor_col);
					break;
				default:;
			};
		}


	static const char * title_0 = "***     ..             .      ..             ";
	static const char * title_1 = "*** A A .. A AAAA A  A . A  A .. A AAAA A  A ";
	static const char * title_2 = "**  A A .. A A    A  A . A  A .  A A    A A  ";
	static const char * title_3 = "** AA A .. A A  . AA A . A  A . AA A .. AA  *";
	static const char * title_4 = "*  AA A .. A AA . AA A @ AAAA   AA A .. A  **";
	static const char * title_5 = "* A A A .. A A  . A AA . A  A  A A A .. A  **";
	static const char * title_6 = "  A A A .. A A .. A AA . A  A  A A A .. AA  *";
	static const char * title_7 = " AAAA A    A A    A  A . A  A AAAA A    A A  ";
	static const char * title_8 = " A  A AAAA A AAAA A  A . A  A A  A AAAA A  A ";
	static const char * title_9 = "                       .                     ";

	drawTitleLine(console, title_x, title_base  , title_0, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+1, title_1, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+2, title_2, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+3, title_3, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+4, title_4, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+5, title_5, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+6, title_6, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+7, title_7, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+8, title_8, Console::BrightGreen);
	drawTitleLine(console, title_x, title_base+9, title_9, Console::BrightGreen);

	console.draw(atx, aty, '@', Console::Green);

	static const char * border0 = "....................";
	std::string version = " Version " + std::string(GAMEMODEL_VERSION_STR) + std::string(" ");
	console.drawText(atx+2, title_base+10, border0, floor_col);
	console.drawText(atx+2, title_base+11, version.c_str(), Console::Green);
	console.drawText(atx+2, title_base+12, border0, floor_col);

	static const char * press_enter = " Press Enter ";
	static const char * border1     = ".............";
	console.drawText(atx+9, title_base+13, press_enter, Console::BrightYellow);
	console.drawText(atx+9, title_base+14, border1    , floor_col);
}

bool Title::drawsWholeWindow(void) const
{
	return true;
}



}
