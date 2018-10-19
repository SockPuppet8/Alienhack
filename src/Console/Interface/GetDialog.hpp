#ifndef ALIENHACK_GET_DIALOG_HPP
#define	ALIENHACK_GET_DIALOG_HPP


#include "Interface/InterfaceState.hpp"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>



namespace RL_shared
{
	class IFunctionMap;
}

namespace AlienHack
{



class AHGameObject;



class IGetDialogSelect
{
public:

	virtual ~IGetDialogSelect()
	{
	}

	//Return true if the simulation is to be advanced as a result, false otherwise. 
	virtual bool select( boost::shared_ptr< AHGameObject > ) = 0;
};


class GetDialog : public RL_shared::InterfaceState
{
public:

	typedef std::vector< boost::shared_ptr< AHGameObject > > AHGameObjectList;

	enum ColourScheme
	{
		Green, 
		Red
	};

	GetDialog( 
		boost::shared_ptr< const AHGameObjectList > objs, 
		boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys, 
		boost::shared_ptr< IGetDialogSelect > select, 
		ColourScheme scheme 
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

	boost::shared_ptr< const AHGameObjectList > m_objs;

	std::string m_back_control;

	boost::shared_ptr< RL_shared::IFunctionMap > m_key_map;

    boost::shared_ptr< IGetDialogSelect > m_selector;

	ColourScheme m_scheme;

	int m_index;
	int m_width;

	bool m_done;
};



}



#endif
