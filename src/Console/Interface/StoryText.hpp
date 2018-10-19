#ifndef ALIENHACK_STORYTEXT_HPP
#define	ALIENHACK_STORYTEXT_HPP


#include "Interface/InterfaceState.hpp"
#include <vector>
#include <string>


namespace AlienHack
{


class AHGameModel;


class StoryText : public RL_shared::InterfaceState
{
public:

	StoryText( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, const std::string& text, int left_margin );

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

	std::vector< std::string > m_text_lines;
	int m_left_margin;
	bool m_done;
};



}



#endif
