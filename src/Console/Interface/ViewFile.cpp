#include "ViewFile.hpp"
#include "draw.hpp"
#include "Console/Console.hpp"
#include "Input/IFunctionMap.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



ViewFile::ViewFile( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	std::istringstream& contents, 
	ColourScheme colour_scheme
)
: InterfaceState( ism )
, m_key_map(keys)
, m_last_window_w(0)
, m_last_window_h(0)
, m_offset(0)
, m_colour_scheme(colour_scheme)
, m_done(false)
{
	readContents(contents);
}

void ViewFile::readContents(std::istringstream& contents)
{
	std::string word;

	for(;;)
	{
		if (!contents.good())
			break;

		char chr(0);
		contents.get(chr);

		if ((' ' == chr) || ('\n' == chr) || ('\r' == chr) || ('\t' == chr))
		{
			m_contents.push_back(word);
			word.clear();

			std::string wsstr;
			wsstr.push_back(chr);
			m_contents.push_back( wsstr );
		}
		else
		{
			word.push_back(chr);
		}
	}
}

void ViewFile::makeLines(int ww, int wh) const
{
	m_last_window_h = wh;

	if (m_last_window_w == ww)
		return;

	m_last_window_w = ww;

	m_lines.clear();

	std::string newline;
	BOOST_FOREACH(const std::string& word, m_contents)
	{
		std::string rword(("\t" == word) ? "    " : word);

		if ((newline.length() + rword.length() > ((unsigned int)ww))
			|| (("\n" == rword) || ("\r" == rword)))
		{
			m_lines.push_back(newline);

			if ((" " != rword) && ("\n" != rword) && ("\r" != rword))
				newline = rword;
			else
				newline.clear();

			continue;
		}

		newline += rword;
	}
}

void ViewFile::enterFromParent( AGameModel& )
{
}
void ViewFile::exitToParent( AGameModel& )
{
}
void ViewFile::enterFromChild( AGameModel& )
{
}
void ViewFile::exitToChild( AGameModel& )
{
}

ViewFile::CommandResult ViewFile::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	bool redraw( false );

	if (m_key_map->isFunction(input, "Up") || m_key_map->isFunction(input, "ScrollUp"))
	{
		--m_offset;
		if (m_offset < 0)
			m_offset = 0;
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "Down") || m_key_map->isFunction(input, "ScrollDown"))
	{
		++m_offset;
		m_offset = (std::min)(m_offset, ((int)m_lines.size())-m_last_window_h);
		redraw = true;
	}
	else if (m_key_map->isFunction(input, "Back") || m_key_map->isFunction(input, "OK"))
	{
		m_done = true;
		redraw = true;
	}

	return CommandResult( false, redraw );
}

bool ViewFile::finished(void)
{
	return m_done;
}


void ViewFile::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );
	console.clearScreen();

	Console::ConsoleDims dims( console.getConsoleDimensions() );
	makeLines(dims.x, dims.y);

	if (m_lines.empty())
		return;

	int offset = (std::max)(0, (std::min)(m_offset, ((int)m_lines.size())-dims.y));

	Console::Colour frame_col( Console::Cyan ), text_col( Console::Green ), hl_col( Console::BrightGreen );
	if (Red == m_colour_scheme)
	{
		frame_col = Console::Red;
		text_col = Console::Red;
		hl_col = Console::BrightRed;
	}

	std::vector< std::string >::const_iterator nextline( m_lines.begin() );
	std::advance( nextline, offset );
	for(int dy(0); dy < dims.y; ++nextline, ++dy)
	{
		console.drawText(0, dy, nextline->c_str(), text_col);
	}

	console.drawText(dims.x - 36, dims.y - 1, " Arrows to scroll, ENTER/ESC to exit", hl_col);
}

bool ViewFile::drawsWholeWindow(void) const
{
	return true;
}



}
