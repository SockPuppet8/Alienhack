#ifndef ALIENHACK_TERMINAL_HPP
#define	ALIENHACK_TERMINAL_HPP


#include "AHGameObject.hpp"


namespace AlienHack
{


namespace terminal
{
	enum Type
	{
		None = -1,
		Info = 0,
		Master, 

		End,
		NumTypes = End
	};
}


class Terminal : public AHGameObject
{
public:

	Terminal( terminal::Type, bool broken );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool blocksDoor(void) const;

	virtual int takeDamage( AHGameModel& model, damage::Type type, int amount, int penetration );
	virtual void acidSplash( AHGameModel&, int amount, int max );

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	terminal::Type terminalType(void) const	{ return m_term_type; }

	bool isBroken(void) const	{ return m_broken; }

	virtual bool isInteresting(bool) const
	{
		return !isBroken();
	}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<AHGameObject>(*this);
		ar & m_term_type;
		ar & m_broken;
	}

	//Default constructor intended only for serialization use.
	Terminal(void);

private:

	terminal::Type m_term_type;
	bool m_broken;
};


}



#endif
