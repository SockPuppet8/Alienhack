#ifndef ALIENHACK_TITLE_HPP
#define	ALIENHACK_TITLE_HPP


#include "Interface/InterfaceState.hpp"
#include <boost/shared_ptr.hpp>
#include <vector>


namespace RL_shared
{
	class IFunctionMap;
}

namespace AlienHack
{


class AHGameModel;
class LoadOrNew;
class GameTypeMenu;


class Title : public RL_shared::InterfaceState
{
public:

	explicit Title( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism );

    virtual void enterFromParent( RL_shared::AGameModel& );
    virtual void exitToParent( RL_shared::AGameModel& );
    virtual void enterFromChild( RL_shared::AGameModel& );
    virtual void exitToChild( RL_shared::AGameModel& );

    virtual CommandResult interpretInput( const RL_shared::AUserInputItem&, RL_shared::AGameModel& );

	virtual void notifyAHGameModelAdvance( RL_shared::AGameModel&, RL_shared::GameTimeCoordinate, bool )
	{
	}

    virtual bool finished(void);

    virtual void draw( RL_shared::AOutputWindow&, RL_shared::AGameModel& ) const;
    virtual bool drawsWholeWindow(void) const;

	virtual bool needsInput(void) const	{ return true; }

private:

	boost::shared_ptr< LoadOrNew > m_load_menu;
	boost::shared_ptr< GameTypeMenu > m_game_menu;
	boost::shared_ptr< RL_shared::IFunctionMap > m_key_map;
	boost::shared_ptr< std::vector<char> > m_background;

	bool m_done;


	void startGame(bool);
	void makeBackground(void);
};



}



#endif
