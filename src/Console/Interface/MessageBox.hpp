#ifndef ALIENHACK_MESSAGE_BOX_HPP
#define	ALIENHACK_MESSAGE_BOX_HPP


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



class MessageBox : public RL_shared::InterfaceState
{
public:

	enum ColourScheme
	{
		Green, 
		Red
	};

	MessageBox( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys, 
		const std::string& message, int x_low, int y_low, int x_high, int y_high, ColourScheme scheme, int prompt_margin );

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
	boost::shared_ptr< RL_shared::IFunctionMap > m_key_map;
	int m_x_low, m_y_low, m_x_high, m_y_high;
	int m_margin;
	ColourScheme m_scheme;
	bool m_done;
};



}



#endif
