#include "CharScreen.hpp"
#include "BuyPerks.hpp"
#include "draw.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "../../Model/Objects/Armour.hpp"
#include "../../Model/Objects/Weapon.hpp"
#include "../../Model/Objects/Pickup.hpp"
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



CharScreen::CharScreen( boost::weak_ptr< InterfaceStateMachine > ism, boost::shared_ptr< RL_shared::IFunctionMap > keys )
: InterfaceState( ism ), m_key_map(keys), m_menu_option(0), m_done(false)
{
	ASSERT( m_key_map );
}

void CharScreen::enterFromParent( AGameModel& )
{
}
void CharScreen::exitToParent( AGameModel& )
{
}
void CharScreen::enterFromChild( AGameModel& )
{
}
void CharScreen::exitToChild( AGameModel& )
{
}

CharScreen::CommandResult CharScreen::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	if (m_key_map->isFunction(input, "OK"))
	{
		if (Perks == m_menu_option)
		{
			shared_ptr< BuyPerks > newstate( new BuyPerks( interfaceStateMachine(), m_key_map ) );
			setNextState( newstate );
			return CommandResult( false, true );
		}
		else
		{
			m_done = true;
		}
	}
	else if (m_key_map->isFunction(input, "Back"))
	{
		m_done = true;
	}
	else if (m_key_map->isFunction(input, "Up"))
	{
		--m_menu_option;
		if (m_menu_option < 0)
			m_menu_option = NumMenuOptions-1;

		return CommandResult( false, true );
	}
	else if (m_key_map->isFunction(input, "Down"))
	{
		++m_menu_option;
		if (m_menu_option >= NumMenuOptions)
			m_menu_option = 0;

		return CommandResult( false, true );
	}

	return CommandResult( false, false );
}

bool CharScreen::finished(void)
{
	return m_done;
}

namespace
{
	void drawItemQuantity( Console& console, int x, int y, const char * name, int amount, int max, Console::Colour col )
	{
		std::string str(name);
		str += boost::lexical_cast< std::string >( amount );
		(str += "/") += boost::lexical_cast< std::string >( max );
		console.drawText(x, y, str.c_str(), col);
	}
}

void CharScreen::draw( AOutputWindow& window, AGameModel& in_model ) const
{
	const AHGameModel& model( dynamic_cast< const AHGameModel& >( in_model ) );

	Console& console( dynamic_cast<Console&>(window) );

	console.clearScreen();
	drawOuterFrame(console, model.isCountdownActive());

	Console::Colour frame_col( Console::Cyan ), text_col( Console::Green ), hl_col( Console::BrightGreen );
	if (model.isCountdownActive())
	{
		frame_col = Console::Red;
		text_col = Console::Red;
		hl_col = Console::BrightRed;
	}

	for (int dy=1; dy<39; ++dy)
		console.draw(12, dy, '|', frame_col);

	console.draw(12, 0, '+', frame_col);
	console.draw(12, 39, '+', frame_col);

	console.drawText(2, 2+Back, "Return", (Back == m_menu_option) ? hl_col : text_col);
	console.drawText(2, 2+Perks, "Perks", (Perks == m_menu_option) ? hl_col : text_col);

	const World& world( model.world() );
	DBKeyValue player_key( model.avatar() );
	if (world.objectExists(player_key))
	{
		const PlayerCharacter& player( dynamic_cast< const PlayerCharacter& >( world.object(player_key) ) );

		const int startx = 15;

		console.drawText(startx, 2, player.name().c_str(), frame_col);

		console.drawText(startx, 4, (boost::format("XP: %d") % player.getXP()).str().c_str(), text_col);

		{
			std::string hp_str( (boost::format("HP: %d/%d") % player.getHP() % player.getMaxHP()).str() );
			console.drawText(startx, 6, hp_str.c_str(), text_col);
			int player_acid( player.getAcid() );
			if (player_acid > 0)
			{
				std::string acid_str;
				((acid_str += "[") += boost::lexical_cast<std::string>(player_acid)) += "]";
				console.drawText(startx + hp_str.length() + 1, 6, acid_str.c_str(), Console::BrightYellow);
			}
		}

		{
			std::string armour_str("Armour : ");
			DBKeyValue armour_key( player.armour() );
			int armour_acid(0);
			if (!world.objectExists(armour_key))
			{
				armour_str += "None";
			}
			else
			{
				const Armour& armour( dynamic_cast< const Armour& >( world.object(armour_key) ) );
				armour_str += armour.getSelectName(true);
				armour_acid = armour.getAcid();
			}
			console.drawText(startx, 8, armour_str.c_str(), text_col);
			if (armour_acid > 0)
			{
				std::string acid_str;
				((acid_str += "[") += boost::lexical_cast<std::string>(armour_acid)) += "]";
				console.drawText(startx + armour_str.length(), 8, acid_str.c_str(), Console::BrightYellow);
			}
		}

		{
			std::string sidearm_str("Sidearm: ");
			DBKeyValue sidearm_key( player.sidearm() );
			if (!world.objectExists( sidearm_key ))
			{
				sidearm_str += "None";
			}
			else
			{
				const Weapon& weapon( dynamic_cast< const Weapon& >( world.object(sidearm_key) ) );
				sidearm_str += weapon.getSelectName(true);
			}
			console.drawText(startx, 9, sidearm_str.c_str(), text_col);
		}

		{
			std::string primary_str("Primary: ");
			DBKeyValue primary_key( player.primary() );
			if (!world.objectExists( primary_key ))
			{
				primary_str += "None";
			}
			else
			{
				const Weapon& weapon( dynamic_cast< const Weapon& >( world.object(primary_key) ) );
				primary_str += weapon.getSelectName(true);
			}
			console.drawText(startx, 10, primary_str.c_str(), text_col);
		}

		const int grenades_start = 12;
		console.drawText(startx, grenades_start  , "Grenades", text_col);
		console.drawText(startx, grenades_start+1, "--------", text_col);

		drawItemQuantity(console, startx, grenades_start+2, "Stun: ", player.num( pickup::StunGrenade ), player.max( pickup::StunGrenade ), text_col);
		drawItemQuantity(console, startx, grenades_start+3, "Inc.: ", player.num( pickup::IncGrenade ), player.max( pickup::IncGrenade ), text_col);
		drawItemQuantity(console, startx, grenades_start+4, "Frag: ", player.num( pickup::FragGrenade ), player.max( pickup::FragGrenade ), text_col);
		drawItemQuantity(console, startx, grenades_start+5, "Krak: ", player.num( pickup::KrakGrenade ), player.max( pickup::KrakGrenade ), text_col);

		const int items_start = grenades_start+7;
		console.drawText(startx, items_start  , "Items", text_col);
		console.drawText(startx, items_start+1, "-----", text_col);

		drawItemQuantity(console, startx, items_start+2, "Medkit:            ", player.num( pickup::Medkit ), player.max( pickup::Medkit ), text_col);
		drawItemQuantity(console, startx, items_start+3, "Neutraliser:       ", player.num( pickup::Neutraliser ), player.max( pickup::Neutraliser ), text_col);
		drawItemQuantity(console, startx, items_start+4, "Demolition Charge: ", player.num( pickup::DemoCharge ), player.max( pickup::DemoCharge ), text_col);

		const int ammo_start = items_start+6;
		console.drawText(startx, ammo_start  , "Ammo", text_col);
		console.drawText(startx, ammo_start+1, "----", text_col);

		drawItemQuantity(console, startx, ammo_start+2, "9mm      : ", player.num( pickup::Ammo_9mm ), player.max( pickup::Ammo_9mm ), text_col);
		drawItemQuantity(console, startx, ammo_start+3, "12g      : ", player.num( pickup::Ammo_shotgun ), player.max( pickup::Ammo_shotgun ), text_col);
		drawItemQuantity(console, startx, ammo_start+4, ".44      : ", player.num( pickup::Ammo_colt ), player.max( pickup::Ammo_colt ), text_col);
		drawItemQuantity(console, startx, ammo_start+5, "Fuel     : ", player.num( pickup::Ammo_flame ), player.max( pickup::Ammo_flame ), text_col);
		drawItemQuantity(console, startx, ammo_start+6, "M309     : ", player.num( pickup::Ammo_pulse ), player.max( pickup::Ammo_pulse ), text_col);
		drawItemQuantity(console, startx, ammo_start+7, "M250     : ", player.num( pickup::Ammo_smartgun ), player.max( pickup::Ammo_smartgun ), text_col);
		drawItemQuantity(console, startx, ammo_start+8, "PP slug  : ", player.num( pickup::Ammo_laser ), player.max( pickup::Ammo_laser ), text_col);
		drawItemQuantity(console, startx, ammo_start+9, "HEDP 30mm: ", player.num( pickup::Ammo_HEDP30mm ), player.max( pickup::Ammo_HEDP30mm ), text_col);

		if (player.num( pickup::MotionTracker ))
			console.drawText(startx, ammo_start+11, "Motion Tracker: Yes", text_col);
		else
			console.drawText(startx, ammo_start+11, "Motion Tracker: No", text_col);

		const int perksx = 50;
		int perksy = 4;

		console.drawText(perksx+3, perksy,   "Perks", text_col);
		console.drawText(perksx+3, perksy+1, "-----", text_col);
		perksy += 2;

		for (int pi = player_perks::First; pi < player_perks::NumPerks; ++pi)
		{
			using namespace player_perks;
			if (player.hasPerk( (Perk)pi ))
			{
				console.drawText(perksx, perksy, perkName((Perk)pi), text_col);
				++perksy;
			}
		}
	}
}

bool CharScreen::drawsWholeWindow(void) const
{
	return true;
}



}
