#ifndef ALIENHACK_GAME_MODEL_HPP_INCLUDED
#define	ALIENHACK_GAME_MODEL_HPP_INCLUDED



#include "OverWorld.hpp"
#include "Game/GameModel.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/VisionField.hpp"
#include "ActionEngine/ActionEngine.hpp"
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>




namespace AlienHack
{



class IGameEvents;
class PlayerStats;
class Furniture;


class AHGameModel : public RL_shared::GameModel
{
public:

	AHGameModel(void);

	enum GameType
	{
		Normal,
		Quick
	};

	GameType gameType(void)	const		{ return m_game_type; }
	void setGameType(GameType type)		{ m_game_type = type; }

	virtual void clear(void);

	virtual void updateVision(void);
	virtual void updateHearing(void);

	virtual void registerObjectInZone( RL_shared::DBKeyValue obj_key, RL_shared::DBKeyValue zone_key );
	virtual void unRegisterObjectInZone( RL_shared::DBKeyValue obj_key, RL_shared::DBKeyValue zone_key );

	void setGameEventsObserver( boost::weak_ptr<IGameEvents> in )
	{
		m_game_events = in;
	}
	boost::shared_ptr<IGameEvents> gameEventsObserver(void) const	{ return m_game_events.lock(); }

	OverWorld& overworld(void)				{ return m_overworld; }
	const OverWorld& overworld(void) const	{ return m_overworld; }

	void registerPlayerStats( boost::shared_ptr< PlayerStats > );

	bool avatarIsVisibleFrom( RL_shared::DBKeyValue zone_key, int zone_x, int zone_z ) const;

	void spawnAlien( RL_shared::DBKeyValue zone_key, bool fast_forward = false );
	void alertAliensInRadius( RL_shared::DBKeyValue zone, int x, int z, int range );

	bool isCountdownActive(void) const							
	{ 
		return m_countdown_expired || ((m_countdown_time > 0) && (m_countdown_time <= countdownBecomesVisible())); 
	}
	bool isCountdownStarted(void) const
	{
		return (m_countdown_time > 0) || m_countdown_expired;
	}
	bool isCountdownExpired(void) const							{ return m_countdown_expired; }
	//void setCountdownTime(RL_shared::GameTimeCoordinate time)	{ m_countdown_time = time; }
	RL_shared::GameTimeCoordinate getCountdownTime(void) const	{ return m_countdown_time; }
	void startCountdown(RL_shared::GameTimeCoordinate delay);
	void abortCountdown(void);

	bool isQueenFound(void) const	{ return m_queen_is_found; }
	void notifyFoundTheQueen(void);

	bool isQueenDead(void) const	{ return m_queen_is_dead; }
	void notifyKilledTheQueen(void);

	void increaseEndgameDanger(void);

	bool hasPlayerEscaped(void) const;
	bool doesPlayerGetTheGoodEnding(void) const;
	bool worldExplodes(void) const;

	void mapZone( RL_shared::Zone& zone );
	void mapCurrentZone(void);

	bool usedMasterTerminal(void) const	{ return m_used_master_terminal; }
	void notifyUsedMasterTerminal(void);

	void destroyedSomething( const Furniture& );

	std::string mortem(void) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::GameModel >(*this);
        ar & m_overworld;
		ar & m_aliens_in_zone_lookup;
		ar & m_player_stats;
	    ar & m_spawn_alien_timer;
	    ar & m_countdown_time;
	    //ar & m_game_events; //don't serialize objects from the interface component.
	    ar & m_game_type;
	    ar & m_queen_is_found;
	    ar & m_queen_is_dead;
	    ar & m_countdown_expired;
	    ar & m_used_master_terminal;
	    ar & m_spawned_first;
    }

private:

	OverWorld m_overworld;

	RL_shared::VisionField m_reverse_vision;

	typedef std::vector< RL_shared::DBKeyValue > Objects;
	typedef std::map< RL_shared::DBKeyValue, Objects > ObjectsInZoneMap;
	ObjectsInZoneMap m_aliens_in_zone_lookup;

	typedef std::vector< boost::shared_ptr< PlayerStats > > PlayerStatsVector;
	PlayerStatsVector m_player_stats;

	RL_shared::GameTimeCoordinate m_spawn_alien_timer;
	RL_shared::GameTimeCoordinate m_countdown_time;

	boost::weak_ptr<IGameEvents> m_game_events;

	GameType m_game_type;

	bool m_queen_is_found;
	bool m_queen_is_dead;
	bool m_countdown_expired;
	bool m_used_master_terminal;
	bool m_spawned_first;


	AHGameModel(AHGameModel&);
	AHGameModel& operator=(AHGameModel&);

	virtual void updateObjects( RL_shared::GameTimeCoordinate time_step, const RL_shared::World::WorldObjects& objects_to_update );
	void updateSpawning( RL_shared::GameTimeCoordinate time_step );
    void spawnHole(void);

	virtual void advanceGameTime( RL_shared::GameTimeCoordinate time_step );
	RL_shared::GameTimeCoordinate countdownBecomesVisible(void) const;

public:

	void checkAliens(void) const;
};



}



#endif
