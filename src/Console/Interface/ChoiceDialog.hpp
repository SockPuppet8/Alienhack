#ifndef ALIENHACK_CHOICE_DIALOG_HPP
#define	ALIENHACK_CHOICE_DIALOG_HPP


#include "Interface/InterfaceState.hpp"
#include <string>
#include <vector>


namespace RL_shared
{
	class IFunctionMap;
}

namespace AlienHack
{


class AHGameModel;


class IChoiceDialogSelect
{
public:

	virtual ~IChoiceDialogSelect()
	{
	}

	//Return true if the simulation is to be advanced as a result, false otherwise. 
	virtual bool select( int option ) = 0;
};



class ChoiceDialog : public RL_shared::InterfaceState
{
public:

	enum ColourScheme
	{
		Green, 
		Red
	};

	ChoiceDialog( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys, 
		const std::string& message, int x_low, int y_low, int x_high, int y_high, ColourScheme scheme, 
		const std::string& button1, const std::string& button2, 
		boost::shared_ptr< IChoiceDialogSelect > action, 
		int default_button );

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

	std::vector< std::string > m_message_lines;
	std::string m_button_text[2];

protected:
	boost::shared_ptr< RL_shared::IFunctionMap > m_key_map;

private:
	int m_x_low, m_y_low, m_x_high, m_y_high;

	boost::shared_ptr< IChoiceDialogSelect > m_action_call;

	int m_selected;

	ColourScheme m_scheme;

protected:
	bool m_done;
};



}



#endif
