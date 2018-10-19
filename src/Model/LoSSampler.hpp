#ifndef ALIENHACK_LOS_SAMPLER_HPP
#define	ALIENHACK_LOS_SAMPLER_HPP


#include "DBKeyValue.hpp"
#include "Algorithm/Algorithms-2D.hpp"


namespace RL_shared
{
	class Zone;
	class World;
}

namespace AlienHack
{



class IIsBlockingObject
{
public:
	virtual ~IIsBlockingObject()
	{
	}
	virtual bool isBlocking( RL_shared::DBKeyValue ) const = 0;
};



class LoSSampler : public RL_shared::ISampleCellFunctor
{
	const RL_shared::Zone& m_zone;
	const IIsBlockingObject* m_object_tester;
public:
	int finish_x;
	int finish_z;
	int last_clear_x;
	int last_clear_z;
	RL_shared::DBKeyValue hit_obj;
	bool hit;

	LoSSampler( const RL_shared::Zone& zone, IIsBlockingObject* object_tester, int dest_x, int dest_z )
		: m_zone( zone ), m_object_tester( object_tester ), 
		finish_x(dest_x), finish_z(dest_z), last_clear_x(dest_x), last_clear_z(dest_z), 
		hit_obj( RL_shared::INVALID_KEY ), hit(false)
	{
	}
	virtual bool operator()(int x, int y);
};



class LineOfSightObjectTester : public IIsBlockingObject
{
	const RL_shared::World& m_world;

public:

	explicit LineOfSightObjectTester( const RL_shared::World& world )
		: m_world(world)
	{
	}

	virtual bool isBlocking( RL_shared::DBKeyValue key ) const;
};

class LineOfFireObjectTester : public IIsBlockingObject
{
	const RL_shared::World& m_world;

public:

	explicit LineOfFireObjectTester( const RL_shared::World& world )
		: m_world(world)
	{
	}

	virtual bool isBlocking( RL_shared::DBKeyValue key ) const;
};



}


#endif
