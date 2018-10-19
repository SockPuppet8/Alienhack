#ifndef ALIENHACK_FIND_NEAREST_HPP
#define	ALIENHACK_FIND_NEAREST_HPP


#include "DBKeyValue.hpp"


namespace AlienHack
{



class AHGameModel;

RL_shared::DBKeyValue findNearestInterestingThing( const AHGameModel&, bool threats_only );



}



#endif
