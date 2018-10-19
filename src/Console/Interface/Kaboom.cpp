#include "Kaboom.hpp"
#include "Console/Console.hpp"
#include <ctime>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



Kaboom::Kaboom( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism )
: InterfaceState( ism ), m_drawn( false )
{
}

void Kaboom::enterFromParent( AGameModel& )
{
}
void Kaboom::exitToParent( AGameModel& )
{
}
void Kaboom::enterFromChild( AGameModel& )
{
}
void Kaboom::exitToChild( AGameModel& )
{
}

Kaboom::CommandResult Kaboom::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	return CommandResult( false, true );
}

bool Kaboom::finished(void)
{
	return m_drawn;
}


void Kaboom::draw( AOutputWindow& window, AGameModel& ) const
{
	Console& console( dynamic_cast<Console&>(window) );

	int num_frames = 0;

	for (; num_frames < 75; ++num_frames)
	{

		console.clearScreen();

		Console::ConsoleDims dims( console.getConsoleDimensions() );

		const Console::Colour backcol[7] = {Console::White, Console::BrightYellow, Console::BrightRed, Console::Red, Console::Black, Console::Black, Console::Black};
		const Console::Colour forecol[7] = {Console::White, Console::White, Console::White, Console::BrightYellow, Console::BrightRed, Console::Red, Console::Black};

		int frame = (std::min)(6, num_frames/5);

		for (int y=0; y<dims.y; ++y)
			for (int x=0; x<dims.x; ++x)
			{
				char chr(' ');
				if (rand() & 1)
					chr = '*';
				if (0 == (rand() & 3))
					chr = '#';

				int frame2 = num_frames > 40 ? frame : (std::max)(0, frame - (rand() & 1));

				console.draw(x, y, chr, forecol[frame2], backcol[frame]);
			}

		console.updateScreen();

		//TODO: replace with sleep call?
		std::clock_t time_now( std::clock() );
		std::clock_t time_wait( time_now + (CLOCKS_PER_SEC / 30) );
		while ((std::clock() >= time_now) && (std::clock() < time_wait));
	}

	m_drawn = true;
}

bool Kaboom::drawsWholeWindow(void) const
{
	return true;
}



}
