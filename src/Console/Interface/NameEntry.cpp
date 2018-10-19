#include "NameEntry.hpp"
#include "draw.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "World-2DTiles/World.hpp"
#include "ConsoleView/KeyMap.hpp"
#include "assert.hpp"


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



namespace keys
{
	const int BACKSPACE = 8;
}



NameEntry::NameEntry( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, boost::shared_ptr< RL_shared::IFunctionMap > keys )
: InterfaceState( ism ), m_key_map(keys), m_done(false)
{
	ASSERT( m_key_map );
}

void NameEntry::enterFromParent( AGameModel& )
{
}
void NameEntry::exitToParent( AGameModel& )
{
}
void NameEntry::enterFromChild( AGameModel& )
{
}
void NameEntry::exitToChild( AGameModel& )
{
}

NameEntry::CommandResult NameEntry::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	const KeyPress& keypress( dynamic_cast<const KeyPress&>(input) );

	if (m_key_map->isFunction(input, "OK"))
	{
		if (!m_name.empty())
		{
			AHGameModel& model( dynamic_cast< AHGameModel& >( in_model ) );
			World& world( model.world() );
			if (world.objectExists( model.avatar() ))
			{
				PlayerCharacter& player( dynamic_cast< PlayerCharacter& >( world.object( model.avatar() ) ) );
				player.setName( m_name );
			}
		}
		
		m_done = true;
	}
	else 
	if ((!keypress.value.second) &&
		    (
			((keypress.value.first >= 'a') && (keypress.value.first <= 'z')) || 
			((keypress.value.first >= 'A') && (keypress.value.first <= 'Z')) || 
			((keypress.value.first >= '0') && (keypress.value.first <= '9')) || 
			(keypress.value.first == ' ')
			))
	{
		m_name += keypress.value.first;
		return CommandResult( false, true );
	}
	else
	if ((keys::BACKSPACE == keypress.value.first) && (!m_name.empty()))
	{
		m_name.erase( --m_name.end() );
		return CommandResult( false, true );
	}

	return CommandResult( false, false );
}

bool NameEntry::finished(void)
{
	return m_done;
}


void NameEntry::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	Console& console( dynamic_cast<Console&>(window) );

	console.clearScreen();

	console.drawText(30, 18, "Enter your name:", Console::Green);
	console.drawText(30, 20, "________________", Console::Green);
	console.drawText(30, 20, m_name.c_str(), Console::Green);

	console.draw(30+m_name.length(), 20, '_', Console::Black, Console::Green);
}

bool NameEntry::drawsWholeWindow(void) const
{
	return true;
}



}
