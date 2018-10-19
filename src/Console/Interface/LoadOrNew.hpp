#ifndef ALIENHACK_LOAD_OR_NEW_HPP
#define	ALIENHACK_LOAD_OR_NEW_HPP


#include "Interface/InterfaceState.hpp"
#include <string>


namespace RL_shared
{
	class IFunctionMap;
}

namespace AlienHack
{


class LoadOrNew : public RL_shared::InterfaceState
{
public:

	LoadOrNew( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, boost::shared_ptr< RL_shared::IFunctionMap > keys );

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

	bool loadedGame(void) const	{ return m_loaded; }
	bool cancelled(void) const	{ return m_cancelled; }

private:

	boost::shared_ptr< RL_shared::IFunctionMap > m_key_map;

	int m_selected;

	bool m_done;
	bool m_loaded;
	bool m_cancelled;
};



}



#endif
