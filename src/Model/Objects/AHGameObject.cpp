#include "AHGameObject.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include "ObjectType.hpp"
#include "../AHGameModel.hpp"
#include <boost/foreach.hpp>



namespace AlienHack
{


using namespace RL_shared;


void AHGameObject::onMove(RL_shared::IGameModel& model)
{
	onMove( dynamic_cast< AHGameModel& >(model) );
}


void AHGameObject::notifyTimeStep( RL_shared::GameModel& model, RL_shared::GameTimeCoordinate dt )
{
	notifyTimeStep( dynamic_cast< AHGameModel& >( model ), dt );
}

void AHGameObject::notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate )
{
}

bool AHGameObject::removeMe(RL_shared::GameModel& model) const
{
	return removeMe( dynamic_cast< AHGameModel& >( model ) );
}

bool AHGameObject::removeMe(AHGameModel&) const
{
	return false;
}

bool AHGameObject::shouldDraw(void) const
{
	return true;
}

int AHGameObject::takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration )
{
	return 0;
}
int AHGameObject::takeDamage( AHGameModel& model, DBKeyValue, damage::Type type, int amount, int penetration )
{
	return takeDamage(model, type, amount, penetration);
}
void AHGameObject::stun( AHGameModel& model, RL_shared::GameTimeCoordinate time )
{
}

void AHGameObject::acidSplash( AHGameModel&, int, int )
{
}

void AHGameObject::neutraliseAcid( AHGameModel& )
{
}

bool AHGameObject::blocksDoor(void) const
{
	return false;
}

bool AHGameObject::blocksVision(void) const
{
	return false;
}

bool AHGameObject::blocksProjectiles(void) const
{
	return false;
}

std::string AHGameObject::getStatusString(void) const
{
	return "";
}

bool AHGameObject::isInteresting(bool) const
{
	return (type() < objects::First_Transient) && (type() != objects::Player);
}

boost::shared_ptr< PlayerStats > AHGameObject::stats(void) const
{
	return boost::shared_ptr< PlayerStats >();
}


}

