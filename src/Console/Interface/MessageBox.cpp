#include "MessageBox.hpp"
#include "Input/IFunctionMap.hpp"
#include "Console/Console.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



MessageBox::MessageBox( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	const std::string& message, int x_low, int y_low, int x_high, int y_high, ColourScheme scheme, int margin 
	)
: InterfaceState( ism ), m_key_map(keys), m_x_low(x_low), m_y_low(y_low), m_x_high(x_high), m_y_high(y_high), m_margin(margin), m_scheme(scheme), m_done(false)
{
	ASSERT( m_key_map );

	// TODO identical to block in ChoiceDialog.cpp
	std::string next_line;
	BOOST_FOREACH( char next_char, message )
	{
		if ('\n' == next_char)
		{
			m_message_lines.push_back( next_line );
			next_line.clear();
			continue;
		}
		next_line += next_char;
	}
	if (!next_line.empty())
		m_message_lines.push_back( next_line );
}

void MessageBox::enterFromParent( AGameModel& )
{
}
void MessageBox::exitToParent( AGameModel& )
{
}
void MessageBox::enterFromChild( AGameModel& )
{
}
void MessageBox::exitToChild( AGameModel& )
{
}

MessageBox::CommandResult MessageBox::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	if (m_key_map->isFunction(input, "Back") || m_key_map->isFunction(input, "OK"))
	{
		m_done = true;
	}

	return CommandResult( false, false );
}

bool MessageBox::finished(void)
{
	return m_done;
}

void MessageBox::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );

	Console::Colour frame_col = Green == m_scheme ? Console::Cyan : Console::Red;

	console.draw(m_x_low, m_y_low, '/', frame_col);
	console.draw(m_x_high, m_y_low, '\\', frame_col);
	console.draw(m_x_low, m_y_high, '\\', frame_col);
	console.draw(m_x_high, m_y_high, '/', frame_col);	

	for (int dx = m_x_low+1; dx < m_x_high; ++dx)
	{
		console.draw(dx, m_y_low, '-', frame_col);
		console.draw(dx, m_y_high, '-', frame_col);
	}
	for (int dy = m_y_low+1; dy < m_y_high; ++dy)
	{
		console.draw(m_x_low, dy, '|', frame_col);
		console.draw(m_x_high, dy, '|', frame_col);
	}
	for (int dy = m_y_low+1; dy < m_y_high; ++dy)
		for (int dx = m_x_low+1; dx < m_x_high; ++dx)
			console.draw(dx, dy, ' ', Console::Black);

	int line_y = m_y_low + 1;
	Console::Colour text_col = Green == m_scheme ? Console::Green : Console::Red;

	BOOST_FOREACH( const std::string& msg_line, m_message_lines )
	{
		if (line_y > (m_y_high-2))
			break;

		console.drawText(m_x_low+1, line_y, msg_line.c_str(), text_col);

		++line_y;
	}

	Console::Colour prompt_col = Green == m_scheme ? Console::BrightGreen : Console::BrightRed;
	console.drawText(m_x_high-11-m_margin, m_y_high-1, "Press Enter", prompt_col);
}

bool MessageBox::drawsWholeWindow(void) const
{
	return false;
}



}
