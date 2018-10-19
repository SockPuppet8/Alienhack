#include "DropDialog.hpp"
#include "Input/IFunctionMap.hpp"
#include "Console/Console.hpp"
#include "assert.hpp"
#include <xutility>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


namespace 
{
    const std::string ERROR_STR("ERROR");

	//TODO move to FunctionToControlsMap
    const std::string& getControl( shared_ptr< const IFunctionMap::FunctionToControlsMap > keys, const std::string& what )
    {
        IFunctionMap::FunctionToControlsMap::const_iterator iter(keys->find(what));
        if (keys->end() != iter)
        {
            const std::vector< std::string >& controls(iter->second);
            if (!controls.empty())
            {
                return controls[0];
            }
        }
        return ERROR_STR;
    }
}



DropDialog::DropDialog( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	boost::shared_ptr< IDropDialogSelect > selector, 
	ColourScheme scheme
	)
: InterfaceState( ism )
, m_key_map(keys)
, m_selector(selector)
, m_scheme(scheme)
, m_done(false)
{
	ASSERT( m_key_map );
	ASSERT( m_selector );
}

void DropDialog::enterFromParent( AGameModel& )
{
}
void DropDialog::exitToParent( AGameModel& )
{
}
void DropDialog::enterFromChild( AGameModel& )
{
}
void DropDialog::exitToChild( AGameModel& )
{
}

DropDialog::CommandResult DropDialog::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	bool advance( false );
	bool redraw( false );

	if (!m_done)
	{
        if (m_key_map->isFunction(input, "Back"))
        {
            m_done = true;
        	return CommandResult( false, true );
        }

        const int NUM_OPTIONS = 10;
        const std::string options[NUM_OPTIONS] = {
            std::string("Sidearm"), 
            std::string("Primary"), 
            std::string("Armour"), 
            std::string("Frag"), 
            std::string("Krak"), 
            std::string("Inc"), 
            std::string("Stun"), 
            std::string("Medkit"), 
            std::string("Neutraliser"), 
            std::string("Demolition")
        };

        for (int opt=0; opt < NUM_OPTIONS; ++opt)
        {
            if (m_key_map->isFunction(input, options[opt]))
            {
	            advance = m_selector->select(options[opt]);
	            redraw = true;
	            m_done = true;
	            break;
            }
        }
	}

	return CommandResult( advance, redraw );
}

bool DropDialog::finished(void)
{
	return m_done;
}

void DropDialog::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );

	Console::Colour frame_col   = Green == m_scheme ? Console::Cyan : Console::Red;
	Console::Colour text_col    = Green == m_scheme ? Console::Green : Console::Red;
	Console::Colour hl_text_col = Green == m_scheme ? Console::BrightGreen : Console::BrightRed;

    int x_low = 18;
    int x_high = 61;

    shared_ptr< const IFunctionMap::FunctionToControlsMap > keys( m_key_map->getFunctions() );

    const int NUM_OPTIONS = 10;
    const std::string vals[NUM_OPTIONS][2] = {
        { std::string("Armour"),  std::string("Armour") }, 
        { std::string("Primary"), std::string("Primary") }, 
        { std::string("Sidearm"), std::string("Sidearm") }, 
        { std::string("Stun"),    std::string("Stun") }, 
        { std::string("Inc"),     std::string("Inc") }, 
        { std::string("Frag"),    std::string("Frag") }, 
        { std::string("Krak"),    std::string("Krak") }, 
        { std::string("Med"),     std::string("Medkit") }, 
        { std::string("Neut"),    std::string("Neutraliser") }, 
        { std::string("Demo"),    std::string("Demolition") }
    };

    int maxlen = 0;
    for (int opt=0; opt < NUM_OPTIONS; ++opt) 
    {
        const std::string& control( getControl( keys, vals[opt][1] ) );
        maxlen = (std::max)((int)control.length(), maxlen);
    }
    maxlen += 10;

    int min_spacing = 2;
    int cols = (std::max)(1, 1 + ( ((x_high-x_low-3-maxlen) / (maxlen+min_spacing)) ));
    int colw = cols < 2 ? (x_high-x_low-3) : ((x_high-x_low-3-maxlen) / (cols-1));
    int rows = (std::max)(1, (NUM_OPTIONS/cols));

    int y_size = 8 + rows;
    int y_low = 20 - (y_size/2) - (y_size&1);
    int y_high = y_low + y_size;

	console.draw(x_low, y_low, '/', frame_col);
	console.draw(x_high, y_low, '\\', frame_col);
	console.draw(x_low, y_high, '\\', frame_col);
	console.draw(x_high, y_high, '/', frame_col);	

	for (int dx = x_low+1; dx < x_high; ++dx)
	{
		console.draw(dx, y_low, '-', frame_col);
		console.draw(dx, y_high, '-', frame_col);
	}
	for (int dy = y_low+1; dy < y_high; ++dy)
	{
		console.draw(x_low, dy, '|', frame_col);
		console.draw(x_high, dy, '|', frame_col);
	}
	for (int dy = y_low+1; dy < y_high; ++dy)
		for (int dx = x_low+1; dx < x_high; ++dx)
			console.draw(dx, dy, ' ', Console::Black);


    console.drawText(x_low+2, y_low+2, "Drop what?", text_col);


    int xt(x_low+2), yt(y_low+4);
    for (int opt=0; opt < NUM_OPTIONS; ++opt) 
    {
        try 
        {
            const std::string& control( getControl( keys, vals[opt][1] ) );

            int start = xt;
            console.drawText(xt, yt, vals[opt][0].c_str(), text_col);
            xt += 7;
            console.drawText(xt, yt, " (", text_col);
            xt += 2;
            console.drawText(xt, yt, control.c_str(), hl_text_col);
            xt += control.length();
            console.drawText(xt, yt, ")", text_col);

            xt = start + colw;
            if (xt >= x_low+(cols*colw))
            {
                xt = x_low+2;
                ++yt;
            }
        }
        catch (...)
        {
        }
    }

    try
    {
        const std::string& back_control( getControl( keys, "Back" ) );
        console.drawText(x_low+2, y_high-2, "Cancel (", text_col);
        console.drawText(x_low+10, y_high-2, back_control.c_str(), hl_text_col);
        console.drawText(x_low+10+back_control.length(), y_high-2, ")", text_col);
    }
    catch (...)
    {
    }
}

bool DropDialog::drawsWholeWindow(void) const
{
	return false;
}



}
