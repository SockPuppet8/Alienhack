#ifndef ALIENHACK_MODEL_OVERWORLD_HPP
#define	ALIENHACK_MODEL_OVERWORLD_HPP


#include "Objects/Alien.hpp"
#include "DBKeyValue.hpp"
#include <boost/cstdint.hpp>
#include <string>


namespace AlienHack
{


class AHGameModel;


void initialiseWorld(AHGameModel& model);

void spawnSwarm( AHGameModel& model, RL_shared::DBKeyValue zone_key );
void spawnAlien( AHGameModel&, RL_shared::DBKeyValue zone_key, bool fast_forward = false );
void spawnAlien( AHGameModel& model, RL_shared::DBKeyValue zone_key, aliens::Type type_to_spawn, bool fast_forward = false );


//Stores the arrangement of zones in the game world.
//
// Block layout:
//
//       +--+
//       | 9|
//       +--+
//
// +--+  +--+  +--+
// | 6|  | 7|  | 8|
// +--+  +--+  +--+
//
// +--+  +--+  +--+
// | 3|  | 4|  | 5|
// +--+  +--+  +--+
//
// +--+  +--+  +--+
// | 0|  | 1|  | 2|
// +--+  +--+  +--+
//
class OverWorld
{
public:

	OverWorld(void);

	void reset(void);

	enum {NUM_BLOCKS = 10, FLOOR_MIN = 0, MAX_FLOORS = 4};
	enum Direction {None=-1, North = 0, South = 1, East = 2, West = 3, NW=4, NE=5, SW=6, SE=7};

	enum BlockType
	{
		Unknown = 0, 
		Accommodation,
		Medical,
		Administration,
		Workshop,
		Storage,
		Security,
		Engineering, 
		Reactor
	};

	RL_shared::DBKeyValue getFloorKey( int block, int floor ) const;
	void setFloorKey( int block, int floor, RL_shared::DBKeyValue );

	int exitsToBlock( int block, int floor, Direction ) const;
	void setExitsToBlock( int block, int floor, Direction, int target_block );

	BlockType blockType( int block ) const;
	const std::string& blockName( int block ) const;

	void setBlockType( int block, BlockType );
	void setBlockName( int block, const std::string& );

	int level( int block, int floor ) const;
	void setLevel( int block, int floor, int level );

	struct BlockAndFloor
	{
		int block, floor;
		BlockAndFloor(void) : block(-1), floor(-1)
		{
		}
		BlockAndFloor(int in_block, int in_floor) : block(in_block), floor(in_floor)
		{
		}
	};
	BlockAndFloor getBlockAndFloor( RL_shared::DBKeyValue zone_key ) const;

	bool isMapped(int block, int floor) const;
	void setMapped(int block, int floor);

	bool isVisited(int block, int floor) const;
	void setVisited(int block, int floor);

	bool isDark(int block, int floor) const;
	void setDark(int block, int floor);

	int getNeighbouringBlock(int block, int dx, int dz) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & m_block_names;
		ar & m_zone_keys;
		ar & m_level;
		ar & m_exits_to_block;
		ar & m_block_types;
		ar & m_floor_flags;
	}

private:

	std::string m_block_names[NUM_BLOCKS];

	RL_shared::DBKeyValue m_zone_keys[NUM_BLOCKS][MAX_FLOORS];

	int m_level[NUM_BLOCKS][MAX_FLOORS];

	int m_exits_to_block[NUM_BLOCKS][MAX_FLOORS][4];

	BlockType m_block_types[NUM_BLOCKS];

	enum
	{
		Mapped = 1, 
		Visited = 2,
		Dark = 4
	};
	boost::uint32_t m_floor_flags[NUM_BLOCKS][MAX_FLOORS];
};


void initialiseOverWorld( OverWorld& );


}


#endif
