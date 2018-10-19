#ifndef ALIENHACK_PLAYING_GAME_HPP
#define	ALIENHACK_PLAYING_GAME_HPP


#include "ActionEngine/ActionEngine.hpp"
#include "Messages/Console/MessageDisplay.hpp"
#include "Interface/InterfaceState.hpp"
#include <boost/shared_ptr.hpp>


namespace RL_shared
{
	class IFunctionMap;
}


namespace AlienHack
{


class AHGameModel;
class IGameEvents;


class PlayingGame : public RL_shared::InterfaceState
{
public:

	PlayingGame( 
		boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys,
		bool loaded 
		);

    virtual void enterFromParent( RL_shared::AGameModel& );
    virtual void exitToParent( RL_shared::AGameModel& );
    virtual void enterFromChild( RL_shared::AGameModel& );
    virtual void exitToChild( RL_shared::AGameModel& );

    virtual CommandResult interpretInput( const RL_shared::AUserInputItem&, RL_shared::AGameModel& );

	virtual void notifyAHGameModelAdvance( RL_shared::AGameModel&, RL_shared::GameTimeCoordinate dt, bool is_current_state );

    virtual bool finished(void);

    virtual void draw( RL_shared::AOutputWindow&, RL_shared::AGameModel& ) const;

    virtual bool drawsWholeWindow(void) const;

	virtual bool needsInput(void) const	{ return true; }

private:

	RL_shared::MessageDisplay m_msgs;

	boost::shared_ptr< RL_shared::IFunctionMap > m_key_map;
	//shared_ptr because the game model object has a longer lifetime than even the topmost interface state. 
	boost::shared_ptr< IGameEvents > m_game_events;

	RL_shared::GameTimeCoordinate m_last_input;

	bool m_quit;
	bool m_loaded_game;

	bool m_move_only;
	bool m_turn_only;
	bool m_move_lock;
	bool m_turn_lock;
	bool m_activating_object;
	bool m_placing_charge;

	bool m_first_look_mode;
	bool m_shown_story_text_1;
	bool m_shown_story_text_2;
	bool m_shown_story_text_3;
	bool m_shown_intro_text;
	bool m_shown_master_text;
	bool m_shown_found_queen_text;
	bool m_shown_killed_queen_text;
	bool m_shown_countdown_start_text;
	bool m_shown_ending_text_1;
	bool m_good_ending;
	bool m_shown_bad_ending_text;
	bool m_shown_good_ending_text_1;
	bool m_shown_good_ending_text_2;
	bool m_shown_death_msg;
	bool m_shown_mortem;

	void writeMortem( RL_shared::AGameModel& in_model );
	void showMortem();
};



}



#endif
