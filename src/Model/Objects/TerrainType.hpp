#ifndef ALIENHACK_TERRAIN_TYPE_HPP
#define	ALIENHACK_TERRAIN_TYPE_HPP



namespace AlienHack
{
	typedef boost::uint16_t TerrainType;


	namespace terrain
	{
		enum
		{
			Empty = 0,
			Room, 
			Wall, 
			Door, 
			Vent, 
			StairsUp, 
			StairsDown, 
			Bridge, 
			BlastDoor, 
			Machinery, 
			FireDoor, 
			Hole, 

			Open = 1<<15,
			EW_facing = 1<<14, 
			Broken = 1<<13,

			BrokenStairsUp = StairsUp | Broken,
			BrokenStairsDown = StairsDown | Broken,

			Flags = Open | EW_facing | Broken, 
			Types = ~Flags
		};

		inline TerrainType getType( TerrainType type )
		{
			return type & Types;
		}

		inline bool isBroken( TerrainType type )
		{
			return (0 != (type & Broken));
		}

		inline bool isOpen( TerrainType type )
		{
			return (0 != (type & Open));
		}

		inline bool isBreakable( TerrainType type )
		{
			TerrainType temp( getType(type) );
			return (Door == temp) && (!isOpen(type)) && (!isBroken(type));
		}

		inline bool isClear( TerrainType type )
		{
			TerrainType temp( getType(type) );
			return (Room == temp) || (Vent == temp) || (Hole == temp);
		}

		inline bool isPassable( TerrainType type, bool ignore_doors = false )
		{
			TerrainType temp( getType(type) );
			return isBroken(type) 
				|| ((temp != Wall) 
						&& ((temp != Door) || ignore_doors || isOpen(type)) 
						&& (temp != Machinery)
						&& (temp != BlastDoor)
						);
		}

        inline bool accessSubstructure( TerrainType type )
        {
			TerrainType temp( getType(type) );
			return (Vent == temp) || (Hole == temp);
        }
	}
}



#endif
