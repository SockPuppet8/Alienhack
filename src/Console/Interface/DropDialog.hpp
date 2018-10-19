#ifndef ALIENHACK_DROP_DIALOG_HPP
#define	ALIENHACK_DROP_DIALOG_HPP


#include "Interface/InterfaceState.hpp"
#include <string>


namespace RL_shared
{
	class IFunctionMap;
}

namespace AlienHack
{


class IDropDialogSelect
{
public:

	virtual ~IDropDialogSelect()
	{
	}

	//Return true if the simulation is to be advanced as a result, false otherwise. 
	virtual bool select( const std::string& ) = 0;
};



class DropDialog : public RL_shared::InterfaceState
{
public:

	enum ColourScheme
	{
		Green, 
		Red
	};

	DropDialog( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys, 
		boost::shared_ptr< IDropDialogSelect > select, 
		ColourScheme scheme );

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

    boost::shared_ptr< IDropDialogSelect > m_selector;

	ColourScheme m_scheme;

	bool m_done;
};



}



#endif
