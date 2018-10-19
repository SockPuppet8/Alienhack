#ifndef ALIENHACK_CONSOLE_INTERFACE_GAME_EVENTS_HPP
#define	ALIENHACK_CONSOLE_INTERFACE_GAME_EVENTS_HPP


#include "../../Model/IGameEvents.hpp"
#include <boost/shared_ptr.hpp>


namespace RL_shared
{
	class MessageDisplay;
}


namespace AlienHack
{

	boost::shared_ptr< IGameEvents > makeGameEvents( RL_shared::MessageDisplay& );

}


#endif
