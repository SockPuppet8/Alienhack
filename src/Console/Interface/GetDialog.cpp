#include "GetDialog.hpp"
#include "../../Model/Objects/AHGameObject.hpp"
#include "Input/IFunctionMap.hpp"
#include "Console/Console.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
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


GetDialog::GetDialog( 
	shared_ptr< const AHGameObjectList > objs, 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	boost::shared_ptr< IGetDialogSelect > selector, 
	ColourScheme scheme
	)
: InterfaceState( ism )
, m_key_map(keys)
, m_selector(selector)
, m_scheme(scheme)
, m_objs(objs)
, m_index(0)
, m_width(0)
, m_done(false)
{
	ASSERT( m_key_map );
	ASSERT( m_selector );
	ASSERT( m_objs && m_objs->size() > 0 );

	BOOST_FOREACH( shared_ptr<AHGameObject> item, (*m_objs) )
	{
		int len = item->getThereIsString(true).length();
		m_width = (std::max)( len, m_width );
	}

	m_back_control = getControl( m_key_map->getFunctions(), "Back" );
}

void GetDialog::enterFromParent( AGameModel& )
{
}
void GetDialog::exitToParent( AGameModel& )
{
}
void GetDialog::enterFromChild( AGameModel& )
{
}
void GetDialog::exitToChild( AGameModel& )
{
}

GetDialog::CommandResult GetDialog::interpretInput( const AUserInputItem& input, AGameModel& in_model )
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

        if (m_key_map->isFunction(input, "OK"))
		{
			advance = m_selector->select( (*m_objs)[m_index] );
			redraw = true;
			m_done = true;
		}
		else if (m_key_map->isFunction(input, "Up"))
		{
			redraw = true;
			--m_index;
			if (m_index < 0)
				m_index = m_objs->size() - 1;
		}
		else if (m_key_map->isFunction(input, "Down"))
		{
			redraw = true;
			++m_index;
			if (m_index >= (int)m_objs->size())
				m_index = 0;
		}
	}

	return CommandResult( advance, redraw );
}

bool GetDialog::finished(void)
{
	return m_done;
}

void GetDialog::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );
	Console::ConsoleDims dims( console.getConsoleDimensions() );

	Console::Colour frame_col   = Green == m_scheme ? Console::Cyan : Console::Red;
	Console::Colour text_col    = Green == m_scheme ? Console::Green : Console::Red;
	Console::Colour hl_text_col = Green == m_scheme ? Console::BrightGreen : Console::BrightRed;

    int maxlen = m_width + 8;
    int rows = (std::min)((dims.y / 2), (int)m_objs->size());
    int x_low = (dims.x - maxlen) / 2;
    int x_high = x_low + maxlen;
    int y_size = 8 + rows;
    int y_low = (dims.y/2) - (y_size/2) - (y_size&1);
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


    console.drawText(x_low+2, y_low+2, "Select item:", text_col);

	int m_window_start = 0;
	if ((int)m_objs->size() > rows)
		m_window_start = (std::min)( (int)m_objs->size()-rows, (std::max)(0, m_index - (rows/2)) );

    for (int row=0; row < rows; ++row) 
    {
		int index = m_window_start + row;

		std::string text( (*m_objs)[index]->getThereIsString(true) );

		console.drawText( x_low + 4, y_low + 4 + row, text.c_str(), (index == m_index) ? hl_text_col : text_col);

		if (index == m_index)
		{
			console.draw( x_low + 2, y_low + 4 + row, '>', Console::BrightYellow);
			console.draw( x_low + 4 + text.length() + 1, y_low + 4 + row, '<', Console::BrightYellow);
		}
    }


    console.drawText(x_low+2, y_high-2, "Cancel (", text_col);
    console.drawText(x_low+10, y_high-2, m_back_control.c_str(), hl_text_col);
    console.drawText(x_low+10 + m_back_control.length(), y_high-2, ")", text_col);
}

bool GetDialog::drawsWholeWindow(void) const
{
	return false;
}



}
