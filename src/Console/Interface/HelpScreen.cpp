#include "HelpScreen.hpp"
#include "ViewFile.hpp"
#include "draw.hpp"
#include "Console/Console.hpp"
#include "Input/IFunctionMap.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <fstream>
#include <sstream>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



HelpScreen::HelpScreen( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	ColourScheme colour_scheme
)
: InterfaceState( ism ), m_key_map(keys), m_colour_scheme(colour_scheme), m_done(false), m_readme_error(false)
{
}

void HelpScreen::enterFromParent( AGameModel& )
{
}
void HelpScreen::exitToParent( AGameModel& )
{
}
void HelpScreen::enterFromChild( AGameModel& )
{
}
void HelpScreen::exitToChild( AGameModel& )
{
}

HelpScreen::CommandResult HelpScreen::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	if (m_key_map->isFunction(input, "Back") || m_key_map->isFunction(input, "OK"))
	{
		m_done = true;
		return CommandResult( false, true );
	}
	else if (m_key_map->isFunction(input, "Help"))
	{
		try
		{
			std::ifstream fin( "ah_readme.txt" );
			if (fin.good())
			{
				//ugh. Need a better way to do this. 
				std::string temp((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
				std::istringstream issin(temp);

				shared_ptr< ViewFile > newstate( new ViewFile( 
						interfaceStateMachine(), m_key_map, issin, (Red == m_colour_scheme) ? ViewFile::Red : ViewFile::Normal 
						) 
					);
				setNextState( newstate );
				return CommandResult( false, true );
			}
		}
		catch(...)
		{
		}

		m_readme_error = true;
		return CommandResult( false, true );
	}

	return CommandResult( false, false );
}

bool HelpScreen::finished(void)
{
	return m_done;
}

namespace
{
	typedef RL_shared::IFunctionMap::FunctionToControlsMap Mapping;

	int drawControls(
		Console& console, const Mapping& map, 
		const std::string& func, 
		int x, int y, Console::Colour col
		)
	{
		Mapping::const_iterator look( map.find(func) );
		if ((map.end() == look) || (look->second.empty()))
		{
			console.drawText(x, y, "None", col);
			return x+4;
		}

		bool first = true;
		BOOST_FOREACH( const std::string& control, look->second )
		{
			if (!first)
			{
				console.drawText(x, y, ", ", col);
				x += 2;
			}
			console.drawText(x, y, control.c_str(), col);
			x += control.length();
			first = false;
		}

		return x;
	}
}

void HelpScreen::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	//const AHGameModel& model( dynamic_cast< const AHGameModel& >( in_model ) );

	Console& console( dynamic_cast<Console&>(window) );
	Console::ConsoleDims dims( console.getConsoleDimensions() );

	console.clearScreen();
	drawOuterFrame(console, (Red == m_colour_scheme));

	Console::Colour frame_col( Console::Cyan ), text_col( Console::Green ), hl_col( Console::BrightGreen );
	if (Red == m_colour_scheme)
	{
		frame_col = Console::Red;
		text_col = Console::Red;
		hl_col = Console::BrightRed;
	}

	const Mapping& controls( *(m_key_map->getFunctions()) );

	{
		std::string back_str("<Unknown>");

		Mapping::const_iterator look( controls.find("Back") );
		if ((controls.end() != look) && (!look->second.empty()))
		{
			back_str = look->second.front();
		}

		console.drawText( 2, 37, "Press ", text_col);
		console.drawText( 8, 37, back_str.c_str(), hl_col);
		console.drawText( 8+back_str.length(), 37, " to return", text_col);
	}

	console.drawText( 2, 2, "OK / Select", text_col );
	drawControls( console, controls, "OK", 26, 2, hl_col );

	console.drawText( 2, 3, "Cancel / Back", text_col );
	drawControls( console, controls, "Back", 26, 3, hl_col );

	console.drawText( 2, 4, "Up / North", text_col );
	drawControls( console, controls, "Up", 26, 4, hl_col );

	console.drawText( 2, 5, "Up + left / NW", text_col );
	drawControls( console, controls, "UpAndLeft", 26, 5, hl_col );

	console.drawText( 2, 6, "Left / West", text_col );
	drawControls( console, controls, "Left", 26, 6, hl_col );

	console.drawText( 2, 7, "Down + left / SW", text_col );
	drawControls( console, controls, "DownAndLeft", 26, 7, hl_col );

	console.drawText( 2, 8, "Down / South", text_col );
	drawControls( console, controls, "Down", 26, 8, hl_col );

	console.drawText( 2, 9, "Down + right / SE", text_col );
	drawControls( console, controls, "DownAndRight", 26, 9, hl_col );

	console.drawText( 2, 10, "Right / East", text_col );
	drawControls( console, controls, "Right", 26, 10, hl_col );

	console.drawText( 2, 11, "Up + right / NE", text_col );
	drawControls( console, controls, "UpAndRight", 26, 11, hl_col );

	console.drawText( 2, 13, "Wait", text_col );
	drawControls( console, controls, "Wait", 26, 13, hl_col );

	console.drawText( 2, 14, "Up one floor", text_col );
	drawControls( console, controls, "FloorUp", 26, 14, hl_col );

	console.drawText( 2, 15, "Down one floor", text_col );
	drawControls( console, controls, "FloorDown", 26, 15, hl_col );

	console.drawText( 2, 16, "Strafe", text_col );
	drawControls( console, controls, "Strafe", 26, 16, hl_col );

	console.drawText( 2, 17, "Strafe lock", text_col );
	drawControls( console, controls, "StrafeLock", 26, 17, hl_col );

	console.drawText( 2, 18, "Turn", text_col );
	drawControls( console, controls, "Turn", 26, 18, hl_col );

	console.drawText( 2, 19, "Turn lock", text_col );
	drawControls( console, controls, "TurnLock", 26, 19, hl_col );

	console.drawText( 2, 20, "Get items", text_col );
	drawControls( console, controls, "Get", 26, 20, hl_col );

	//console.drawText( 2, 21, "Take ammo", text_col );
	//drawControls( console, controls, "TakeAmmo", 26, 21, hl_col );

	console.drawText( 2, 22, "Operate (door/terminal)", text_col );
	drawControls( console, controls, "Operate", 26, 22, hl_col );

	//console.drawText( 2, 23, "Drop armour", text_col );
	//drawControls( console, controls, "Armour", 26, 23, hl_col );

	console.drawText( 2, 24, "Select sidearm", text_col );
	drawControls( console, controls, "Sidearm", 26, 24, hl_col );

	console.drawText( 2, 25, "Select primary", text_col );
	drawControls( console, controls, "Primary", 26, 25, hl_col );

	console.drawText( 2, 26, "Reload weapon", text_col );
	drawControls( console, controls, "Reload", 26, 26, hl_col );

	console.drawText( 2, 27, "Drop something", text_col );
	drawControls( console, controls, "Drop", 26, 27, hl_col );

	console.drawText( 2, 28, "Fire", text_col );
	drawControls( console, controls, "Fire", 26, 28, hl_col );

	console.drawText( 2, 29, "Frag grenade", text_col );
	drawControls( console, controls, "Frag", 26, 29, hl_col );

	console.drawText( 2, 30, "Stun grenade", text_col );
	drawControls( console, controls, "Stun", 26, 30, hl_col );

	console.drawText( 2, 31, "Inc grenade", text_col );
	drawControls( console, controls, "Inc", 26, 31, hl_col );

	console.drawText( 2, 32, "Krak grenade", text_col );
	drawControls( console, controls, "Krak", 26, 32, hl_col );

	console.drawText( 2, 33, "Use medkit", text_col );
	drawControls( console, controls, "Medkit", 26, 33, hl_col );

	console.drawText( 2, 34, "Use neutraliser", text_col );
	drawControls( console, controls, "Neutraliser", 26, 34, hl_col );

	console.drawText( 2, 35, "Use demo charge", text_col );
	drawControls( console, controls, "Demolition", 26, 35, hl_col );

	console.drawText( 40, 2, "Scroll messages up", text_col );
	drawControls( console, controls, "ScrollUp", 66, 2, hl_col );

	console.drawText( 40, 3, "Scroll messages down", text_col );
	drawControls( console, controls, "ScrollDown", 66, 3, hl_col );

	console.drawText( 40, 4, "Help (this screen)", text_col );
	drawControls( console, controls, "Help", 66, 4, hl_col );

	console.drawText( 40, 5, "Look", text_col );
	drawControls( console, controls, "Look", 66, 5, hl_col );

	console.drawText( 40, 6, "Save and quit", text_col );
	drawControls( console, controls, "Save", 66, 6, hl_col );

	console.drawText( 40, 7, "Character screen", text_col );
	drawControls( console, controls, "Char", 66, 7, hl_col );

	console.drawText( 40, 8, "Buy Perks", text_col );
	drawControls( console, controls, "Buy", 66, 8, hl_col );

	if (m_readme_error)
	{
		std::string msg("Could not open \"ah_readme.txt\" :(");
		console.drawText( (dims.x-msg.length())/2, dims.y-1, msg.c_str(), hl_col );
	}
	else
	{
		int x = (dims.x/2)-15;
		console.drawText( x, dims.y-1, "Press ", hl_col );
		x = drawControls( console, controls, "Help", x+6, dims.y-1, hl_col );
		console.drawText( x, dims.y-1, " to view game help file", hl_col );
	}
}

bool HelpScreen::drawsWholeWindow(void) const
{
	return true;
}



}
