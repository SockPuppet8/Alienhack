#include "StoryText.hpp"
#include "draw.hpp"
#include "Console/Console.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <ctime>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



StoryText::StoryText( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	const std::string& text, 
	int left_margin
	)
: InterfaceState(ism), m_left_margin(left_margin), m_done(false)
{
	//TODO identical to block in ChoiceDialog
	std::string next_line;
	BOOST_FOREACH( char next_char, text )
	{
		if ('\n' == next_char)
		{
			m_text_lines.push_back( next_line );
			next_line.clear();
			continue;
		}
		next_line += next_char;
	}
	if (!next_line.empty())
		m_text_lines.push_back( next_line );
}

void StoryText::enterFromParent( AGameModel& )
{
}
void StoryText::exitToParent( AGameModel& )
{
}
void StoryText::enterFromChild( AGameModel& )
{
}
void StoryText::exitToChild( AGameModel& )
{
}

StoryText::CommandResult StoryText::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	m_done = true;

	return CommandResult( false, true );
}

bool StoryText::finished(void)
{
	return m_done;
}

void StoryText::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );

	int start_y = (40 - m_text_lines.size()) / 2;

	int num_frames = (m_text_lines.size()*2)+2;
	for (int frame=0; frame < num_frames; ++frame)
	{

		console.clearScreen();

		int line_y = start_y;
		int cap_y = start_y + ((frame+1)/2);
		int len = 0;

		if (frame > 0)
		{
			BOOST_FOREACH( const std::string& msg_line, m_text_lines )
			{
				console.drawText(m_left_margin, line_y, msg_line.c_str(), Console::Green);

				++line_y;

				if (line_y >= cap_y)
				{
					len = msg_line.length();
					break;
				}
			}
		}

		if (frame < (num_frames-1))
		{
			if (frame & 1)
			{
				console.draw(m_left_margin+len, cap_y-1, ' ', Console::Black, Console::BrightGreen);
			}
			else
			{
				console.draw(m_left_margin, cap_y, ' ', Console::Black, Console::BrightGreen);
			}
		}

		console.updateScreen();


		//TODO perhaps replace with sleep call
		std::clock_t time_now( std::clock() );
		std::clock_t time_wait( time_now + (CLOCKS_PER_SEC / 50) );
		while ((std::clock() >= time_now) && (std::clock() < time_wait));
	}
}

bool StoryText::drawsWholeWindow(void) const
{
	return true;
}



}
