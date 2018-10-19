#include "ChoiceDialog.hpp"
#include "Input/IFunctionMap.hpp"
#include "Console/Console.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



ChoiceDialog::ChoiceDialog( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	const std::string& message, int x_low, int y_low, int x_high, int y_high, ColourScheme scheme, 
	const std::string& button1, const std::string& button2, 
	boost::shared_ptr< IChoiceDialogSelect > action, 
	int default_button
	)
: InterfaceState( ism )
, m_key_map(keys)
, m_x_low(x_low), m_y_low(y_low), m_x_high(x_high), m_y_high(y_high)
, m_action_call(action)
, m_selected(default_button)
, m_scheme(scheme)
, m_done(false)
{
	ASSERT( m_key_map );
	ASSERT( m_action_call );

	m_button_text[0] = button1;
	m_button_text[1] = button2;

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

void ChoiceDialog::enterFromParent( AGameModel& )
{
}
void ChoiceDialog::exitToParent( AGameModel& )
{
}
void ChoiceDialog::enterFromChild( AGameModel& )
{
}
void ChoiceDialog::exitToChild( AGameModel& )
{
}

ChoiceDialog::CommandResult ChoiceDialog::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	bool advance( false );
	bool redraw( false );

	if (!m_done)
	{
		if (m_key_map->isFunction(input, "OK"))
		{
			if (m_action_call)
			{
				advance = m_action_call->select( m_selected );
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

bool ChoiceDialog::finished(void)
{
	return m_done;
}

void ChoiceDialog::draw( AOutputWindow& window, AGameModel& in_model ) const
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
	Console::Colour text_col    = Green == m_scheme ? Console::Green : Console::Red;
	Console::Colour hl_text_col = Green == m_scheme ? Console::BrightGreen : Console::BrightRed;

	BOOST_FOREACH( const std::string& msg_line, m_message_lines )
	{
		if (line_y > (m_y_high-2))
			break;

		console.drawText(m_x_low+1, line_y, msg_line.c_str(), text_col);

		++line_y;
	}

	int xmid = (m_x_high + m_x_low) / 2;
	int button1x = ((m_x_low  + xmid) / 2) - (m_button_text[0].length() / 2);
	int button2x = ((m_x_high + xmid) / 2) - (m_button_text[1].length() / 2);

	console.drawText(button1x, m_y_high-1, m_button_text[0].c_str(), (0 == m_selected) ? hl_text_col : text_col);
	console.drawText(button2x, m_y_high-1, m_button_text[1].c_str(), (1 == m_selected) ? hl_text_col : text_col);

	if (0 == m_selected)
	{
		console.draw(button1x-1, m_y_high-1, '[', hl_text_col);
		console.draw(button1x+m_button_text[0].length(), m_y_high-1, ']', hl_text_col);
	}
	else if (1 == m_selected)
	{
		console.draw(button2x-1, m_y_high-1, '[', hl_text_col);
		console.draw(button2x+m_button_text[1].length(), m_y_high-1, ']', hl_text_col);
	}
}

bool ChoiceDialog::drawsWholeWindow(void) const
{
	return false;
}



}
