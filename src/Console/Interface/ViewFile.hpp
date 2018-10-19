#ifndef ALIENHACK_VIEWFILE_HPP
#define	ALIENHACK_VIEWFILE_HPP


#include "Interface/InterfaceState.hpp"
#include <string>
#include <vector>
#include <sstream>


namespace RL_shared
{
	class IFunctionMap;
}

namespace AlienHack
{


class AHGameModel;


class ViewFile : public RL_shared::InterfaceState
{
public:

	enum ColourScheme
	{
		Normal, 
		Red
	};

	ViewFile( 
		boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys, 
		std::istringstream& contents, 
		ColourScheme
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

	std::vector< std::string > m_contents;
	mutable std::vector< std::string > m_lines;
	mutable int m_last_window_w;
	mutable int m_last_window_h;

	int m_offset;

	ColourScheme m_colour_scheme;

	bool m_done;


	void readContents(std::istringstream& contents);
	void makeLines(int ww, int wh) const;
};



}



#endif
