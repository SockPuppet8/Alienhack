#ifndef ALIENHACK_ALIEN_HPP
#define	ALIENHACK_ALIEN_HPP


#include "AHGameObject.hpp"
#include "../Actions/HasCompletionCallback.hpp"
#include "Algorithm/Algorithms-2D.hpp"
#include <vector>


namespace RL_shared
{
	class World;
	class Zone;
}

namespace AlienHack
{



void soundAlertAliens(AHGameModel& model, RL_shared::DBKeyValue zone, int x, int z, int range);



class Alien;
void alienMakesSound(AHGameModel& model, Alien& alien, int volume);



namespace aliens
{
	enum Type
	{
		None = -1,
		Juvenile = 0,
		Warrior,
		Gladiator,
		Elite,
		Harrier,
		Hunter,
		Slayer, 
		Reaper,
		Facehugger,
		Fasthugger,
		Spitter,
		DeathSpitter,
		Hulk,
		Queen,
		Worker,

		End,
		NumTypes = End
	};

	inline bool isLargeAlien(Type type)
	{
		return (Queen == type) || (Hulk == type);
	}
	inline bool isSmallAlien(Type type)
	{
		return (Juvenile == type) || (Harrier == type);
	}
	inline bool isHugger(Type type)
	{
		return (Facehugger == type) || (Fasthugger == type);
	}
	inline bool isSneaky(Type type)
	{
		return (Hunter == type) || (Slayer == type) || (Reaper == type);
	}
	inline bool isRangedAttacker(Type type)
	{
		return (Spitter == type) || (DeathSpitter == type);
	}

	int getEvasion(Type type);
	int getAttackRange(Type type);
	int getRangedAttackAmount(Type type);
}

namespace alien_behaviour
{
	enum Behaviour
	{
		none = 0,
		chasing_target, 
		path_to_target,
		path_to_vent,
		moving_in_substructure, 
		ambushing
	};
};




class Alien : public AHGameObject
{
public:

	typedef alien_behaviour::Behaviour Behaviour;

	Alien( aliens::Type type, bool in_substructure );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;
	bool canOverlapOrBreak(const RL_shared::World&, const RL_shared::Zone&, int, int) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;
	virtual bool alwaysProcess(void) const;

	virtual int takeDamage( AHGameModel&, RL_shared::DBKeyValue attacker, damage::Type type, int amount, int penetration );
	virtual void stun( AHGameModel& model, RL_shared::GameTimeCoordinate time );

	virtual bool blocksDoor(void) const;

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	void doAI( AHGameModel& );

	void soundAlert(RL_shared::World& world, const RL_shared::Zone& zone, int x, int z, int error);

	bool isInSubstructure(void) const	{ return alien_behaviour::moving_in_substructure == m_behaviour; }
//	Behaviour currentAction(void) const	{ return m_behaviour; }
	bool canSeePlayer(void) const		{ return m_sees_player; }
	bool playerHasSeenMe(void) const	{ return m_player_has_seen_me; }

	bool pickExitVent(RL_shared::World&, const RL_shared::Zone&, WorldObject::WorldLocation target_loc);

	int getHP(void) const	{ return m_HP; }

	bool isStunned(void) const	{ return m_stun_timer > 0; }

	aliens::Type alienType(void) const	{ return m_alien_type; }

	virtual bool isInteresting(bool threat) const
	{
		return (!isInSubstructure()) && ((!threat) || (aliens::Worker != alienType()));
	}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< AHGameObject >(*this);
		ar & m_path_to_vent;
		ar & m_path_to_target;
		ar & m_target;
		ar & m_target_out_vent;
		ar & m_ai_timer;
		ar & m_stun_timer;
		ar & m_last_seen_player;
		ar & m_behaviour;
		ar & m_random_searches;
		ar & m_blocked_on_path;
		ar & m_HP;
		ar & m_alien_type;
		ar & m_sees_player;
		ar & m_activated;
		ar & m_player_has_seen_me;
	}

	//Default constructor intended only for serialization use.
	Alien(void);

private:

	typedef RL_shared::Path Path;

	Path m_path_to_vent;
	Path m_path_to_target;

	WorldLocation m_target;
	WorldLocation m_target_out_vent;

	RL_shared::GameTimeCoordinate m_ai_timer;
	RL_shared::GameTimeCoordinate m_stun_timer;
	RL_shared::GameTimeCoordinate m_last_seen_player;

	Behaviour m_behaviour;

	int m_random_searches;
	int m_blocked_on_path;

	int m_HP;

	aliens::Type m_alien_type;

	bool m_sees_player;
	bool m_activated;
	bool m_player_has_seen_me;


	bool findVent(RL_shared::World& world, const RL_shared::Zone& zone, WorldObject::WorldLocation target_loc);
	bool findExitVent(RL_shared::World&, const RL_shared::Zone&, WorldObject::WorldLocation target_loc);
	bool findPathToTarget(RL_shared::World& world, const RL_shared::Zone& zone, WorldObject::WorldLocation target_loc);

	bool enterVent(AHGameModel&);
	bool exitVent(AHGameModel&);

	void pickNewTarget( const RL_shared::World&, const RL_shared::Zone& );
	bool queenJump( AHGameModel& );

	//Workaround for incompatibility between boost::serialization and boost::enable_shared_from_this
	boost::shared_ptr< Alien > getSharedFromThis( RL_shared::World& );

	friend class EnterExitVentAction;
};




class AlienActionComplete : public IObserveActionComplete
{
	boost::weak_ptr< Alien > m_weak_alien;

public: 

	explicit AlienActionComplete( boost::shared_ptr< Alien > alien )
		: m_weak_alien( alien )
	{
	}

	virtual void notifyActionComplete( AHGameModel& model )
	{
		boost::shared_ptr< Alien > alien( m_weak_alien.lock() );
		if (alien)
			alien->doAI(model);
	}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & m_weak_alien;
    }

	//Default constructor intended only for serialization use.
	AlienActionComplete(void)
	{
	}
};



}


#endif
