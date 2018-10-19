#ifndef ALIENHACK_DECORATION_HPP
#define	ALIENHACK_DECORATION_HPP


#include "AHGameObject.hpp"


namespace AlienHack
{


namespace decoration
{
	enum Type
	{
		None = -1,
		Corpse = 0,
		AlienCorpse, 
		Blood, 

		End,
		NumTypes = End
	};
}


class Decoration : public AHGameObject
{
public:

	explicit Decoration( decoration::Type );

	virtual bool canOverlap(const RL_shared::WorldObject&) const;
	virtual bool canOverlap(TerrainType) const;

	virtual void notifyTimeStep( AHGameModel&, RL_shared::GameTimeCoordinate dt );
	virtual bool removeMe(AHGameModel&) const;
	virtual bool shouldDraw(void) const;

	virtual bool blocksDoor(void) const;

	virtual std::string getThereIsString(bool) const;
	virtual std::string getSelectName(bool) const;

	decoration::Type decorationType(void) const	{ return m_decor_type; }

	void disappear(void);

	virtual bool isInteresting(bool) const
	{
		return false;
	}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & boost::serialization::base_object< AHGameObject >(*this);
		ar & m_decor_type;
		ar & m_disappear;
	}

	//Default constructor intended only for serialization use.
	Decoration(void);

private:

	decoration::Type m_decor_type;
	bool m_disappear;

};


}



#endif
