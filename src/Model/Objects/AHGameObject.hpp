#ifndef ALIENHACK_GAME_OBJECT_HPP
#define	ALIENHACK_GAME_OBJECT_HPP


#include "Game/GameObject.hpp"
#include <boost/shared_ptr.hpp>
#include <string>


namespace RL_shared
{
	class Zone;
}

namespace AlienHack
{


class AHGameModel;
class PlayerStats;


typedef RL_shared::WorldObjectType WorldObjectType;
typedef RL_shared::TerrainType TerrainType;


namespace damage
{
	enum Type
	{
		None = 0,
		Bullet, 
		Fire, 
		Explosion, 
		Acid, 
		Melee,
		Laser,
		Lance
	};
}


class AHGameObject : public RL_shared::GameObject
{
public:

	explicit AHGameObject( WorldObjectType type ) 
	: RL_shared::GameObject(type)
	{
	}

	virtual void notifyTimeStep( RL_shared::GameModel&, RL_shared::GameTimeCoordinate dt );
	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe( RL_shared::GameModel& ) const;
	virtual bool removeMe( AHGameModel& ) const;

public:
	virtual bool shouldDraw(void) const;

protected:
	virtual int takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration );
public:
	virtual int takeDamage( AHGameModel& model, RL_shared::DBKeyValue attacker, damage::Type type, int amount, int penetration );
	virtual void stun( AHGameModel& model, RL_shared::GameTimeCoordinate time );

	virtual bool blocksDoor(void) const;
	virtual bool blocksVision(void) const;
	virtual bool blocksProjectiles(void) const;

	virtual void acidSplash( AHGameModel&, int amount, int max );
	virtual void neutraliseAcid( AHGameModel& );


	virtual std::string getThereIsString(bool show_status) const = 0;
	virtual std::string getSelectName(bool show_status) const = 0;
	virtual std::string getStatusString(void) const;

	virtual bool isInteresting(bool as_threat) const;

	virtual boost::shared_ptr< PlayerStats > stats(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::GameObject >(*this);
	}

private:

	virtual void onMove(AHGameModel&)	{}
	virtual void onMove(RL_shared::IGameModel&);

};


}


#endif
