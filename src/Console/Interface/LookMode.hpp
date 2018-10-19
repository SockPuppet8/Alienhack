#ifndef ALIENHACK_LOOK_MODE_HPP
#define	ALIENHACK_LOOK_MODE_HPP


#include "Interface/InterfaceState.hpp"


namespace RL_shared
{
	class MessageDisplay;
	class IFunctionMap;
}

namespace AlienHack
{


class AHGameModel;


class LookMode : public RL_shared::InterfaceState
{
public:

	LookMode( 
		boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys, 
		RL_shared::MessageDisplay&, 
		int x, int z, int block, int floor
		);

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

	boost::shared_ptr< RL_shared::IFunctionMap > m_key_map;

	RL_shared::MessageDisplay& m_msgs;

	int m_lx;
	int m_lz;
	int m_lb;
	int m_lf;
	bool m_done;
};



}



#endif
