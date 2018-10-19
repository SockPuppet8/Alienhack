#ifndef ALIENHACK_KABOOM_HPP
#define	ALIENHACK_KABOOM_HPP


#include "Interface/InterfaceState.hpp"


namespace AlienHack
{


class AHGameModel;


class Kaboom : public RL_shared::InterfaceState
{
public:

	explicit Kaboom( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism );

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

	virtual bool needsInput(void) const	{ return false; }

private:

	mutable bool m_drawn;
};



}



#endif
