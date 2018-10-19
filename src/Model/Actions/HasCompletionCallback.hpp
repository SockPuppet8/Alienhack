#ifndef ALIENHACK_HAS_COMPLETION_CALLBACK_HPP
#define	ALIENHACK_HAS_COMPLETION_CALLBACK_HPP


#include "ActionEngine/ActionEngine.hpp"
#include <boost/shared_ptr.hpp>


namespace AlienHack
{


class AHGameModel;


class IObserveActionComplete
{
public:

	virtual ~IObserveActionComplete()
	{
	}

	virtual void notifyActionComplete( AHGameModel& model ) = 0;
};


class HasCompletionCallback : public RL_shared::BaseAction
{
public:

	void setActionCompleteObserver( boost::shared_ptr< IObserveActionComplete > observer )
	{
		m_observer = observer;
	}

protected:

	void onActionComplete( AHGameModel& model )
	{
		if (m_observer)
		{
			m_observer->notifyActionComplete(model);
		}
		m_observer.reset();
	}

public:

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< RL_shared::BaseAction >(*this);
		ar & m_observer;
	}

private:

	boost::shared_ptr< IObserveActionComplete > m_observer;

};


}


#endif
