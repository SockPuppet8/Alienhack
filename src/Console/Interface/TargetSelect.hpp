#ifndef ALIENHACK_TARGET_SELECT_HPP
#define	ALIENHACK_TARGET_SELECT_HPP


#include "Interface/InterfaceState.hpp"
#include "Console/Console.hpp"
#include "DBKeyValue.hpp"
#include <boost/shared_ptr.hpp>
#include <utility>


namespace RL_shared
{
	class MessageDisplay;
	class IFunctionMap;
}

namespace AlienHack
{


class AHGameModel;


class ITargetSelectCallback
{
public:

	virtual ~ITargetSelectCallback()
	{
	}

	//Arguments are offset from avatar.
	//Return value: 
	//First boolean indicates whether the TargetSelect state should exit
	//Second boolean indicates whether the model should be advanced.
	virtual std::pair< bool, bool > select( AHGameModel&, int x, int z ) = 0;
};


class TargetSelect : public RL_shared::InterfaceState
{
public:

	TargetSelect( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys, 
		boost::shared_ptr< ITargetSelectCallback >, RL_shared::MessageDisplay&, const RL_shared::KeyCode& initiating_key );

    virtual void enterFromParent( RL_shared::AGameModel& );
    virtual void exitToParent( RL_shared::AGameModel& );
    virtual void enterFromChild( RL_shared::AGameModel& );
    virtual void exitToChild( RL_shared::AGameModel& );

    virtual CommandResult interpretInput( const RL_shared::AUserInputItem&, RL_shared::AGameModel& );

	virtual void notifyAHGameModelAdvance( RL_shared::AGameModel&, RL_shared::GameTimeCoordinate dt, bool is_current_state )
	{
	}

    virtual bool finished(void);

    virtual void draw( RL_shared::AOutputWindow&, RL_shared::AGameModel& ) const;
    virtual bool drawsWholeWindow(void) const;

	virtual bool needsInput(void) const	{ return true; }

	void jumpToObject( const AHGameModel&, RL_shared::DBKeyValue object );

private:

	boost::shared_ptr< RL_shared::IFunctionMap > m_key_map;

	RL_shared::KeyCode m_init_key;

	RL_shared::MessageDisplay& m_msgs;

	boost::shared_ptr< ITargetSelectCallback >	m_callback;

	int m_tx;
	int m_tz;

	bool m_done;
};



}



#endif
