#ifndef ALIENHACK_CHARSCREEN_HPP
#define	ALIENHACK_CHARSCREEN_HPP


#include "Interface/InterfaceState.hpp"


namespace RL_shared
{
	class IFunctionMap;
}

namespace AlienHack
{


class AHGameModel;


class CharScreen : public RL_shared::InterfaceState
{
public:

	CharScreen( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, boost::shared_ptr< RL_shared::IFunctionMap > keys );

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

	int m_menu_option;

	enum MenuOption
	{
		Back = 0, 
		Perks, 
		NumMenuOptions
	};

	bool m_done;
};



}



#endif
