#include "BuyPerks.hpp"
#include "draw.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "../../Model/AHGameModel.hpp"
#include "Input/IFunctionMap.hpp"
#include "Console/Console.hpp"
#include "assert.hpp"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



BuyPerks::BuyPerks( boost::weak_ptr< InterfaceStateMachine > ism, boost::shared_ptr< RL_shared::IFunctionMap > keys )
: InterfaceState( ism ), m_key_map(keys), m_selected(player_perks::NumPerks), m_done(false)
{
	ASSERT( m_key_map );
}

void BuyPerks::enterFromParent( AGameModel& )
{
}
void BuyPerks::exitToParent( AGameModel& )
{
}
void BuyPerks::enterFromChild( AGameModel& )
{
}
void BuyPerks::exitToChild( AGameModel& )
{
}

BuyPerks::CommandResult BuyPerks::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	using namespace player_perks;

	bool redraw(true);

	if (m_key_map->isFunction(input, "Back"))
	{
		m_done = true;
	}
	else if (m_key_map->isFunction(input, "Up"))
	{
		if (First >= m_selected)
			m_selected = NumPerks;
		else
			--m_selected;
	}
	else if (m_key_map->isFunction(input, "Down"))
	{
		if (NumPerks == m_selected)
			m_selected = First;
		else
			++m_selected;
	}
	else if (m_key_map->isFunction(input, "OK"))
	{
		if (NumPerks == m_selected)
		{
			m_done = true;
		}
		else
		{
			AHGameModel& model( dynamic_cast< AHGameModel& >( in_model ) );
			World& world( model.world() );
			DBKeyValue player_key( model.avatar() );
			if (world.objectExists(player_key))
			{
				PlayerCharacter& player( dynamic_cast< PlayerCharacter& >( world.object(player_key) ) );
				player.buyPerk( (Perk)m_selected );
			}
		}
	}
	else
	{
		redraw = false;
	}

	return CommandResult( false, redraw );
}

bool BuyPerks::finished(void)
{
	return m_done;
}

void BuyPerks::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	const AHGameModel& model( dynamic_cast< const AHGameModel& >( in_model ) );

	Console& console( dynamic_cast<Console&>(window) );

	console.clearScreen();
	drawOuterFrame(console, model.isCountdownActive());

	Console::Colour frame_col( Console::Cyan ), text_col( Console::Green ), hl_col( Console::BrightGreen ), bought_col( Console::BrightCyan );
	if (model.isCountdownActive())
	{
		frame_col = Console::Red;
		text_col = Console::Red;
		hl_col = Console::BrightRed;
		bought_col = Console::BrightYellow;
	}

	const World& world( model.world() );
	DBKeyValue player_key( model.avatar() );
	if (world.objectExists(player_key))
	{
		const PlayerCharacter& player( dynamic_cast< const PlayerCharacter& >( world.object(player_key) ) );

		boost::format xp_str("XP: %d");
		console.drawText(2, 2, (boost::format("XP: %d") % player.getXP()).str().c_str(), hl_col); 

		console.drawText(23, 2, "Perk", text_col);
		console.drawText(45, 2, "XP Cost", text_col);

		int perky = 4;

		using namespace player_perks;

		for (int pi = First; pi < NumPerks; ++pi)
		{
			if (m_selected == pi)
			{
				Console::Colour col = (player.hasPerk((Perk)pi)) ? bought_col : (player.canBuyPerk( (Perk)pi )) ? hl_col : Console::Black;
				console.drawText(20, perky, perkName((Perk)pi), col, text_col);
			}
			else
			{
				Console::Colour col = (player.hasPerk((Perk)pi)) ? bought_col : (player.canBuyPerk( (Perk)pi )) ? hl_col : text_col;
				console.drawText(20, perky, perkName((Perk)pi), col);
			}

			std::string cost_str( boost::lexical_cast<std::string>( perkXPCost((Perk)pi) ) );
			console.drawText(45, perky, cost_str.c_str(), (m_selected == pi) ? hl_col : text_col);

			if (m_selected == pi)
			{
				console.draw(18, perky, '>', Console::BrightYellow);
				console.draw(46 + cost_str.length(), perky, '<', Console::BrightYellow);
			}

			++perky;
		}

		++perky;

		if (NumPerks == m_selected)
		{
			console.drawText(20, perky, "Finish", hl_col, text_col);
			console.draw(18, perky, '>', Console::BrightYellow);
			console.draw(27, perky, '<', Console::BrightYellow);
		}
		else
		{
			console.drawText(20, perky, "Finish", text_col);
		}

		std::string description_str("Exit this menu");
		if (NumPerks != m_selected)
		{
			description_str = perkDescription((Perk)m_selected);
		}
		int centerx = 39-(description_str.length()/2);
		console.drawText(centerx, 38, description_str.c_str(), text_col);
	}
	else
	{
		console.drawText(2, 2, "Shouldn't be here. Press ESC", Console::BrightGreen);
	}
}

bool BuyPerks::drawsWholeWindow(void) const
{
	return true;
}



}
