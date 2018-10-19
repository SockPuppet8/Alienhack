#include "ConsoleView/ConsoleView.hpp"
#include "../Model/AHGameModel.hpp"
#include "Interface/Title.hpp"
#include "Interface/InterfaceStateMachine.hpp"
#include <stdlib.h>
#include <ctime>
#include <iostream>


using namespace AlienHack;
using namespace RL_shared;


void main(void)
{
	try
	{
		unsigned int seed = static_cast<unsigned int>(time(0));
		srand(seed);

		AHGameModel game;

		boost::shared_ptr< InterfaceStateMachine > ism( new InterfaceStateMachine() );
		InterfaceStatePtr initial_state( new Title( ism ) );
		ism->pushInitialState( game, initial_state );

		ConsoleView view( game, *ism );
		view.run();
	}
	catch(std::exception& err)
	{
		std::cerr << "Error: " << err.what();
	}
	catch(const char * string)
	{
		std::cerr << "Error: " << string;
	}
	catch(...)
	{
		std::cerr << "Unknown exception!";
	}
}

