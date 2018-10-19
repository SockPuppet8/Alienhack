#include "draw.hpp"
#include "../../Model/Objects/TerrainType.hpp"
#include "../../Model/Objects/ObjectType.hpp"
#include "../../Model/Objects/AHGameObject.hpp"
#include "../../Model/Objects/Projectile.hpp"
#include "../../Model/Objects/Pickup.hpp"
#include "../../Model/Objects/Weapon.hpp"
#include "../../Model/Objects/Armour.hpp"
#include "../../Model/Objects/Alien.hpp"
#include "../../Model/Objects/PlayerCharacter.hpp"
#include "../../Model/Objects/Explosion.hpp"
#include "../../Model/Objects/LaserTrail.hpp"
#include "../../Model/Objects/Fire.hpp"
#include "../../Model/Objects/Ping.hpp"
#include "../../Model/Objects/Decoration.hpp"
#include "../../Model/Objects/Terminal.hpp"
#include "../../Model/Objects/Furniture.hpp"
#include "../../Model/AHGameModel.hpp"
#include "../../Model/OverWorld.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/Zone.hpp"
#include "World-2DTiles/VisionField.hpp"
#include "Messages/UserMessageLog.hpp"
#include "Messages/UserMessageWindow.hpp"
#include "Console/Console.hpp"
#include "Algorithm/Algorithms-2D.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


struct DrawItem
{
	char	chr;
	Console::Colour fore;
	Console::Colour back;

	DrawItem(char in_chr, Console::Colour in_fore, Console::Colour in_back = Console::Black)
	: chr(in_chr), fore(in_fore), back(in_back)
	{
	}
};


struct LargeObject
{
	const AHGameObject* obj;
	int wx, wy;
	LargeObject( void ) : obj(0), wx(-1), wy(-1) {}
	LargeObject( const AHGameObject* in_obj, int in_wx, int in_wy ) 
		: obj( in_obj ), wx( in_wx ), wy( in_wy )
	{
	}
};
typedef std::vector< LargeObject > LargeObjects;


DrawItem getTerrainItemRepresentation(TerrainType val, bool visible, bool is_nest_level, bool is_countdown)
{
	switch(val)
	{
		case terrain::Wall: 
			if (is_countdown)
				return visible ? DrawItem('#', Console::BrightRed) : DrawItem('#', Console::Red);
			else
				return visible ? DrawItem('#', (is_nest_level ? Console::Green : Console::Cyan)) : DrawItem('#', Console::Blue);
		case terrain::Wall | terrain::Broken: 
			if (is_countdown)
				return visible ? DrawItem('%', Console::BrightRed) : DrawItem('%', Console::Red);
			else
				return visible ? DrawItem('%', (is_nest_level ? Console::Green : Console::Cyan)) : DrawItem('%', Console::Blue);
		case terrain::Room: 
			if (is_countdown)
				return visible ? DrawItem('.', Console::Red) : DrawItem(' ', Console::Red);
			else
				//return visible ? DrawItem('.', (is_nest_level ? Console::Green : Console::Blue)) : DrawItem(' ', Console::Blue);
				return visible ? DrawItem('.', Console::Blue) : DrawItem(' ', Console::Blue);
		case terrain::Door: 
			if (is_countdown)
				return visible ? DrawItem('-', Console::BrightRed) : DrawItem('-', Console::Red);
			else
				return visible ? DrawItem('-', Console::Cyan) : DrawItem('-', Console::Blue);
		case terrain::Door | terrain::Open: 
			if (is_countdown)
				return visible ? DrawItem('/', Console::BrightRed) : DrawItem('/', Console::Red);
			else
				return visible ? DrawItem('/', Console::Cyan) : DrawItem('/', Console::Blue);
		case terrain::Door | terrain::EW_facing: 
			if (is_countdown)
				return visible ? DrawItem('|', Console::BrightRed) : DrawItem('|', Console::Red);
			else
				return visible ? DrawItem('|', Console::Cyan) : DrawItem('|', Console::Blue);
		case terrain::Door | terrain::EW_facing | terrain::Open: 
			if (is_countdown)
				return visible ? DrawItem('/', Console::BrightRed) : DrawItem('/', Console::Red);
			else
				return visible ? DrawItem('/', Console::Cyan) : DrawItem('/', Console::Blue);
		case terrain::Door | terrain::Broken: 
		case terrain::Door | terrain::Broken | terrain::Open: 
		case terrain::Door | terrain::Broken | terrain::EW_facing: 
		case terrain::Door | terrain::Broken | terrain::EW_facing | terrain::Open: 
			if (is_countdown)
				return visible ? DrawItem('%', Console::Red) : DrawItem('%', Console::Red);
			else
				return visible ? DrawItem('%', Console::Cyan) : DrawItem('%', Console::Blue);
		case terrain::Vent:
			if (is_countdown)
				return visible ? DrawItem('X', Console::Black, Console::Red) : DrawItem('X', Console::Red);
			else
				return visible ? DrawItem('X', Console::Black, Console::Blue) : DrawItem('X', Console::Blue);
		case terrain::Hole:
			if (is_countdown)
				return visible ? DrawItem('O', Console::Red) : DrawItem('O', Console::Red);
			else
				return visible ? DrawItem('O', Console::Blue) : DrawItem('O', Console::Blue);
		case terrain::StairsUp:
			if (is_countdown)
				return visible ? DrawItem('>', Console::BrightRed) : DrawItem('>', Console::Red);
			else
				return visible ? DrawItem('>', Console::BrightCyan) : DrawItem('>', Console::Cyan);
		case terrain::StairsDown:
			if (is_countdown)
				return visible ? DrawItem('<', Console::BrightRed) : DrawItem('<', Console::Red);
			else
				return visible ? DrawItem('<', Console::BrightCyan) : DrawItem('<', Console::Cyan);
		case terrain::BrokenStairsUp:
			if (is_countdown)
				return visible ? DrawItem('>', Console::Red) : DrawItem(' ', Console::Red);
			else
				return visible ? DrawItem('>', Console::Blue) : DrawItem(' ', Console::Blue);
		case terrain::BrokenStairsDown:
			if (is_countdown)
				return visible ? DrawItem('<', Console::Red) : DrawItem(' ', Console::Red);
			else
				return visible ? DrawItem('<', Console::Blue) : DrawItem(' ', Console::Blue);
		case terrain::Bridge:
			return visible ? DrawItem('.', Console::Yellow) : DrawItem('.', is_countdown ? Console::Red : Console::Blue);
		case terrain::BlastDoor:
			return visible ? DrawItem('D', Console::Yellow) : DrawItem('D', is_countdown ? Console::Red : Console::Blue);
		case terrain::Machinery:
			return visible ? 
				DrawItem('&', is_countdown ? Console::BrightRed : Console::Cyan) : 
				DrawItem('&', is_countdown ? Console::Red : Console::Blue);
		case terrain::FireDoor:
			return visible ? DrawItem('.', Console::Yellow) : DrawItem('.', is_countdown ? Console::Red : Console::Blue);
		default:;
	}
	return DrawItem(' ', Console::Grey);
}

DrawItem getObjectItemRepresentation(WorldObjectType object, bool visible)
{
	return DrawItem(' ', Console::Grey);
}

DrawItem getObjectItemRepresentation(const AHGameObject& object, bool red_light)
{
	switch (object.type())
	{
		case objects::Player:
			return DrawItem('@', Console::Green);
		case objects::Alien:
		{
			const Alien& alien( dynamic_cast< const Alien& >(object) );
			if (alien.isInSubstructure())
				return DrawItem('?', Console::BrightGreen);
			else
			{
				using namespace aliens;
				switch( alien.alienType() )
				{
					case Juvenile:
						return DrawItem('a', Console::Green);
					case Harrier:
						return DrawItem('a', Console::BrightCyan);
					case Worker:
						return DrawItem('A', Console::Grey);
					case Warrior:
						return DrawItem('A', Console::Green);
					case Hunter:
						return DrawItem('A', Console::Cyan);
					case Elite:
						return DrawItem('A', Console::BrightGreen);
					case Slayer:
						return DrawItem('A', Console::BrightCyan);
					case Hulk:
						return DrawItem('A', Console::BrightYellow);
					case Queen:
						return DrawItem('Q', Console::BrightGreen);
					case Facehugger:
						return DrawItem('f', Console::Yellow);
					case Fasthugger:
						return DrawItem('f', Console::BrightCyan);
					case Spitter:
						return DrawItem('a', Console::BrightGreen);
					case DeathSpitter:
						return DrawItem('a', Console::BrightYellow);
					case Gladiator:
						return DrawItem('A', Console::Yellow);
					case Reaper:
						return DrawItem('A', Console::Magenta);
					default:;
				}
				return DrawItem('A', Console::BrightMagenta);
			}
		}
		case objects::Door:
			return DrawItem('?', Console::BrightMagenta, Console::Red);
		case objects::Pickup:
		{
			const Pickup& pickup( dynamic_cast<const Pickup&>(object) );
			switch( pickup.pickupType() )
			{
				case pickup::Medkit:
					return DrawItem('+', Console::BrightRed);
				case pickup::Neutraliser: 
					return DrawItem('!', Console::BrightCyan);
				case pickup::Ammo_9mm:
					return DrawItem('|', Console::Grey);
				case pickup::Ammo_shotgun: 
					return DrawItem('|', Console::Yellow);
				case pickup::Ammo_colt: 
					return DrawItem('|', Console::White);
				case pickup::Ammo_pulse:
					return DrawItem('|', Console::Cyan);
				case pickup::Ammo_flame:
					return DrawItem('|', Console::BrightRed);
				case pickup::Ammo_smartgun:
					return DrawItem('|', Console::BrightCyan);
				case pickup::Ammo_laser:
					return DrawItem('|', Console::Magenta);
				case pickup::Ammo_HEDP30mm:
					return DrawItem('|', Console::Green);
				case pickup::StunGrenade:
					return DrawItem('*', Console::BrightCyan);
				case pickup::FragGrenade:
					return DrawItem('*', Console::Grey);
				case pickup::IncGrenade:
					return DrawItem('*', Console::BrightYellow);
				case pickup::KrakGrenade:
					return DrawItem('*', Console::Magenta);
				case pickup::MotionTracker:
					return DrawItem('?', Console::BrightCyan);
				case pickup::CreditChip:
					return DrawItem('$', Console::Yellow);
				case pickup::MegaCredChip:
					return DrawItem('$', Console::BrightYellow);
				case pickup::DemoCharge:
					return DrawItem('*', Console::Green);
				default:;
			}
			break;
		}
		case objects::Armour:
		{
			using namespace armour;
			Console::Colour col = Console::BrightMagenta;
			const Armour& armour( dynamic_cast< const Armour& >(object) );
			switch(armour.armourType() & armour::BaseField)
			{
				case FlakJacket:		col = Console::Yellow; break;
				case RiotArmour:		col = Console::Cyan; break;
				case CombatArmour:		col = Console::Green; break;
				case PlasteelArmour:	col = Console::BrightYellow; break;
				case AdvCombatArmour:	col = Console::BrightGreen; break;
				case PowerArmour:		col = Console::BrightCyan; break;
				case ChemhazardSuit:	col = Console::Magenta; break;
				case ChemhazardArmour:	col = Console::BrightMagenta; break;
				default:;
			}
			return DrawItem(']', col);
		}
		case objects::Weapon:
		{
			using namespace weapon;
			Console::Colour col = Console::BrightMagenta;
			const Weapon& weapon( dynamic_cast< const Weapon& >(object) );
			switch(weapon.weaponType())
			{
				case Pistol:			col = Console::Grey;			break;
				case SMG:				col = Console::BrightBlue;		break;
				case PulseRifle:		col = Console::Cyan;			break;
				case SawnoffShotgun:	col = Console::Yellow;			break;
				case CombatShotgun:		col = Console::BrightYellow;	break;
				case Colt:				col = Console::White;			break;
				case Flamer:			col = Console::BrightRed;		break;
				case SmartGun:			col = Console::BrightCyan;		break;
				case Laser:				col = Console::Magenta;			break;
				case CanisterRifle:		col = Console::Green;			break;
				case MachinePistol:		col = Console::BrightGreen;		break;
				default:;
			}
			return DrawItem('/', col);
		}
		case objects::Decoration:
		{
			using namespace decoration;
			const Decoration& decor( dynamic_cast< const Decoration& >(object) );
			switch(decor.decorationType())
			{
				case Corpse:
					return DrawItem('%', Console::Red);
				case AlienCorpse:
					return DrawItem('%', Console::Green);
				case Blood:
					return DrawItem('.', Console::Red);
				default:;
			}
			return DrawItem('?', Console::BrightMagenta, Console::Red);
		}
		case objects::Terminal:
		{
			using namespace terminal;
			Console::Colour col = red_light ? Console::BrightRed : Console::Cyan;
			const Terminal& term( dynamic_cast< const Terminal& >(object) );
			if (term.isBroken())
				return DrawItem('%', col);
			switch(term.terminalType())
			{
				case Info:
					return DrawItem('i', col);
				case Master:
					return DrawItem('M', col);
				default:;
			}
			return DrawItem('?', Console::BrightMagenta, Console::Red);
		}
		case objects::Barrel:
		{
			return DrawItem('O', red_light ? Console::BrightRed : Console::Red);
		}
		case objects::Furniture:
		{
			using namespace furniture;
			const Furniture& furniture( dynamic_cast<const Furniture&>(object) );
			if (furniture.isBroken())
			{
				Console::Colour col( Console::BrightBlue ), red_col(Console::BrightRed);
				if (furniture::Junk == furniture.furnitureType())
					red_col = Console::Red;
				else if (furniture::Chair == furniture.furnitureType())
					red_col = Console::Red;
				else if (furniture::Piping == furniture.furnitureType())
					red_col = Console::Red;
				else if (furniture::Machinery == furniture.furnitureType())
					col = Console::Cyan;
				return DrawItem('%', red_light ? red_col : col);
			}
			switch( furniture.furnitureType() )
			{
				case Crate:
					return DrawItem('x', red_light ? Console::BrightRed : Console::BrightBlue);
				case CrateStack:
					return DrawItem('X', red_light ? Console::BrightRed : Console::BrightBlue);
				case Locker:
					return DrawItem('L', red_light ? Console::BrightRed : Console::BrightBlue);
				case Table:
					return DrawItem('T', red_light ? Console::BrightRed : Console::BrightBlue);
				case Chair:
					return DrawItem('+', red_light ? Console::Red : Console::BrightBlue);
				case Desk:
					return DrawItem('D', red_light ? Console::BrightRed : Console::BrightBlue);
				case Sofa:
					return DrawItem('S', red_light ? Console::BrightRed : Console::BrightBlue);
				case Junk:
					return DrawItem('%', red_light ? Console::Red : Console::BrightBlue);
				case Machinery:
					return DrawItem('&', red_light ? Console::BrightRed : Console::Cyan);
				case Piping:
					return DrawItem('n', red_light ? Console::Red : Console::BrightBlue);
				case Bed:
					return DrawItem('Z', red_light ? Console::Red : Console::BrightBlue);
				case Workbench:
					return DrawItem('B', red_light ? Console::Red : Console::BrightBlue);
				case Hopper:
					return DrawItem('W', red_light ? Console::Red : Console::BrightBlue);
				default:;
			}
			return DrawItem('?', Console::BrightMagenta, Console::Red);
		}
		case objects::Explosive:
		{
			return DrawItem('*', Console::Green);
		}
		case objects::Egg:
		{
			return DrawItem('e', Console::Yellow);
		}
		case objects::Projectile: 
		{
			const Projectile& projectile( dynamic_cast<const Projectile&>(object) );
			switch( projectile.projectileType() )
			{
				case projectile::Grenade:
					return DrawItem('*', Console::Grey);
				case projectile::Flame: 
					return DrawItem('*', rand() & 1 ? Console::BrightYellow : Console::BrightRed);
				case projectile::Laser: 
					return DrawItem('*', Console::BrightRed);
				case projectile::Shot:
					return DrawItem('*', Console::Grey);
				case projectile::Impact:
				{
					switch( projectile.impactType() )
					{
						case impact::Puff:
							return DrawItem('*', Console::Grey);
						case impact::Fire:
							return DrawItem('*', rand() & 1 ? Console::BrightYellow : Console::BrightRed);
						case impact::Laser:
							return DrawItem('*', rand() & 1 ? Console::White : Console::BrightRed);
						case impact::AlienBlood:
							return DrawItem('*', Console::BrightGreen);
						default:;
					}
					return DrawItem('*', Console::Red);
				}
				case projectile::Bullet:
				{
					int dx = projectile.finishX() - projectile.startX();
					int dz = projectile.finishZ() - projectile.startZ();
					int adx = dx < 0 ? -dx : dx;
					int adz = dz < 0 ? -dz : dz;
					if (adz > 2*adx)
						return DrawItem('|', Console::BrightYellow);
					if (adx > 2*adz)
						return DrawItem('-', Console::BrightYellow);
					if ((dx >= 0) == (dz >= 0))
						return DrawItem('/', Console::BrightYellow);
					return DrawItem('\\', Console::BrightYellow);
				}
				default:;
			}
			break;
		}
		case objects::Explosion:
			return DrawItem('*', Console::BrightYellow);
		case objects::Fire:
		{
			const Fire& fire_obj( dynamic_cast< const Fire& >(object) );
			static const Console::Colour flame_cols[3] = {Console::BrightYellow, Console::BrightRed, Console::Red};
			static const Console::Colour smoke_cols[2] = {Console::White, Console::Grey};
			GameTimeCoordinate time( fire_obj.timeRemaining() );
			if (fire_obj.isSmoke())
			{
				Console::Colour col = smoke_cols[ (time/200) & 1 ];
				return DrawItem('*', col);
			}
			else
			{
				Console::Colour col = flame_cols[ (time/200) % 3 ];
				return DrawItem('*', col);
			}
			break;
		}
		case objects::Ping:
		{
			const Ping& ping_obj( dynamic_cast< const Ping& >(object) );
			Console::Colour hi_col = Console::BrightGreen;
			Console::Colour lo_col = Console::Green;
			char symbol = '?';
			if (ping::Noise == ping_obj.pingType())
			{
				hi_col = Console::BrightYellow;
				lo_col = Console::Magenta;
				symbol = '!';
			}
			else if (ping::Danger == ping_obj.pingType())
			{
				hi_col = Console::BrightRed;
				lo_col = Console::Red;
				symbol = '!';
			}
			if (ping_obj.timeRemaining() > (ping_obj.timeFull() / 2))
				return DrawItem(symbol, hi_col);
			return DrawItem(symbol, lo_col);
		}
		default:;
	};
	return DrawItem('?', Console::BrightMagenta, Console::Red);
}

bool isLargeObject( const AHGameObject& game_obj )
{
	return (objects::Explosion == game_obj.type()) || (objects::LaserTrail == game_obj.type());
}

void drawLargeObject(
	Console& console, const AHGameModel& model, const AHGameObject& game_obj, 
	int x, int y, int x_low, int y_low, int x_high, int y_high
	)
{
	if (objects::Explosion == game_obj.type())
	{
		const Explosion& explosion_obj( dynamic_cast< const Explosion& >( game_obj ) );
		GameTimeCoordinate time( explosion_obj.timeRemaining() );
		if (   ( explosion::FragBlast   == explosion_obj.explosionType() ) 
			|| ( explosion::IncBlast    == explosion_obj.explosionType() )
			|| ( explosion::BarrelBlast == explosion_obj.explosionType() )
			)
		{
			const int cells[7][7] = 
			{
				{ -1, -1, 3, 3, 3, -1, -1 },
				{ -1,  3, 2, 2, 2,  3, -1 }, 
				{  3,  2, 1, 1, 1,  2,  3 }, 
				{  3,  2, 1, 0, 1,  2,  3 }, 
				{  3,  2, 1, 1, 1,  2,  3 }, 
				{ -1,  3, 2, 2, 2,  3, -1 }, 
				{ -1, -1, 3, 3, 3, -1, -1 }
			};
			const Console::Colour colours[5] = { Console::Red, Console::BrightRed, Console::BrightYellow, Console::BrightRed, Console::Red };
			int frame = -5 + (std::max)(0, (std::min)(9, (int)(time/50)));
			for (int iy=-3; iy<4; ++iy)
				for (int ix=-3; ix<4; ++ix)
				{
					int tx = x + ix;
					int ty = y + iy;
					if ((tx >= x_low) && (tx <= x_high) && (ty >= y_low) && (ty <= y_high))
					{
						int cellval = cells[iy+3][ix+3];
						if (-1 != cellval)
						{
							cellval += frame;
							if ((cellval >= 0) && (cellval < 5))
								console.draw(tx, ty, '*', colours[cellval]);
						}
					}
				}
		}
		else
		if ( explosion::StunFlash == explosion_obj.explosionType() )
		{
			const int cells[9][9] = 
			{
				{ -1, -1, 3, 3, 3, 3, 3, -1, -1 },
				{ -1,  3, 2, 2, 2, 2, 2,  3, -1 }, 
				{  3,  2, 1, 1, 1, 1, 1,  2,  3 }, 
				{  3,  2, 1, 0, 0, 0, 1,  2,  3 }, 
				{  3,  2, 1, 0, 0, 0, 1,  2,  3 }, 
				{  3,  2, 1, 0, 0, 0, 1,  2,  3 }, 
				{  3,  2, 1, 1, 1, 1, 1,  2,  3 }, 
				{ -1,  3, 2, 2, 2, 2, 2,  3, -1 }, 
				{ -1, -1, 3, 3, 3, 3, 3, -1, -1 },
			};
			const Console::Colour colours[5] = { Console::Blue, Console::BrightBlue, Console::Cyan, Console::BrightCyan, Console::White };
			int frame = -5 + (std::max)(0, (std::min)(9, (int)(time/50)));
			for (int iy=-4; iy<5; ++iy)
				for (int ix=-4; ix<5; ++ix)
				{
					int tx = x + ix;
					int ty = y + iy;
					if ((tx >= x_low) && (tx <= x_high) && (ty >= y_low) && (ty <= y_high))
					{
						int cellval = cells[iy+4][ix+4];
						if (-1 != cellval)
						{
							cellval += frame;
							if ((cellval >= 0) && (cellval < 5))
								console.draw(tx, ty, '*', colours[cellval]);
						}
					}
				}
		}
		else
		if (( explosion::KrakBlast == explosion_obj.explosionType() ) || ( explosion::DemoBlast == explosion_obj.explosionType() ))
		{
			const Console::Colour colours[5] = { Console::Red, Console::BrightRed, Console::BrightYellow, Console::BrightRed, Console::Red };
			int frame = (std::max)(0, (std::min)(4, (int)(time/100)));
			Console::Colour col = colours[ frame ];

			for (int iy=-1; iy<2; ++iy)
				for (int ix=-1; ix<2; ++ix)
				{
					int tx = x + ix;
					int ty = y + iy;
					if ((tx >= x_low) && (tx <= x_high) && (ty >= y_low) && (ty <= y_high))
						console.draw(tx, ty, '*', col);
				}
		}
	}
	else if (objects::LaserTrail == game_obj.type())
	{
		const LaserTrail& trail( dynamic_cast< const LaserTrail& >(game_obj) );
		int offx = trail.targetX();
		int offz = trail.targetZ();
		GameTimeCoordinate time( trail.timeRemaining() );
		DrawItem draw('*', Console::Red);
		if (time > 320)
		{
			draw.fore = Console::White;
			draw.back = Console::Red;
		}
		else if (time > 160)
		{
			draw.fore = Console::BrightRed;
		}

		class NoStop : public ISampleCellFunctor
		{
		public:
			virtual bool operator()(int x, int y)
			{
				return false;
			}
		};
		class DrawLine : public VisitCellFunctor
		{
			Console& m_console;
			int m_x_low, m_y_low, m_x_high, m_y_high;
			DrawItem m_draw;
		public:
			DrawLine( Console& console, int x_low, int y_low, int x_high, int y_high, const DrawItem& draw ) 
				: m_console(console), m_x_low(x_low), m_y_low(y_low), m_x_high(x_high), m_y_high(y_high), m_draw(draw)
			{
			}
			virtual void operator()(int x, int y)
			{
				if ((x >= m_x_low) && (x <= m_x_high) && (y >= m_y_low) && (y <= m_y_high))
					m_console.draw(x, y, m_draw.chr, m_draw.fore, m_draw.back);
			}
		};

		DrawLine drawfunctor( console, x_low, y_low, x_high, y_high, draw );
		lineCast(x, y, x+offx, y-offz, NoStop(), drawfunctor);
	}
}

const char * getStringForTerrain( TerrainType terrain )
{
	switch(terrain)
	{
		case terrain::Wall: 
			return "Wall";
		case terrain::Wall | terrain::Broken:
			return "Breached wall";
		case terrain::Room: 
			return "Room";
		case terrain::Door: 
		case terrain::Door | terrain::EW_facing: 
			return "Closed door";
		case terrain::Door | terrain::Open: 
		case terrain::Door | terrain::EW_facing | terrain::Open: 
			return "Open door";
		case terrain::Door | terrain::Broken: 
		case terrain::Door | terrain::EW_facing | terrain::Broken: 
		case terrain::Door | terrain::Open | terrain::Broken: 
		case terrain::Door | terrain::EW_facing | terrain::Open | terrain::Broken: 
			return "Destroyed door";
		case terrain::Vent:
			return "Ventilation shaft";
		case terrain::Hole:
			return "Hole";
		case terrain::StairsUp:
			return "Stairs up";
		case terrain::StairsDown:
			return "Stairs down";
		case terrain::BrokenStairsUp:
			return "Ruined stairs";
		case terrain::BrokenStairsDown:
			return "Ruined stairs";
		case terrain::Bridge:
			return "Bridge";
		case terrain::BlastDoor:
			return "Blast Doors";
		case terrain::Machinery:
			return "Machinery";
		case terrain::FireDoor:
			return "Room";
		default:;
	}
	return "Unknown";
}

std::string getStringForObject( const AHGameObject& object )
{
	bool status(true);
	if (objects::Weapon == object.type())
		status = false;
	if (objects::Armour == object.type())
		status = false;
	return object.getSelectName(status);
}

bool hasPrecedence( WorldObjectType obj1, WorldObjectType obj2 )
{
	if (objects::None == obj2)
		return true;
	if (objects::Projectile == obj1)
		return true;
	if (objects::Projectile == obj2)
		return false;
	if (objects::Player == obj1)
		return true;
	if (objects::Player == obj2)
		return false;
	if (objects::Alien == obj1)
		return true;
	if (objects::Alien == obj2)
		return false;
	if (objects::Fire == obj1)
		return true;
	if (objects::Fire == obj2)
		return false;
	if (objects::Ping == obj1)
		return true;
	if (objects::Ping == obj2)
		return false;
	if (objects::Explosive == obj1)
		return true;
	if (objects::Explosive == obj2)
		return false;
	if (objects::Weapon == obj1)
		return true;
	if (objects::Weapon == obj2)
		return false;
	if (objects::Armour == obj1)
		return true;
	if (objects::Armour == obj2)
		return false;
	if (objects::Pickup == obj1)
		return true;
	if (objects::Pickup == obj2)
		return false;
	return false;
}

bool terrainHasPriority(const World& world, DBKeyValue obj_key, TerrainType terrain_key)
{
	using namespace terrain;
	TerrainType type( getType(terrain_key) );
	if ((Door == type) || accessSubstructure(type) || (StairsUp == type) || (StairsDown == type))
	{
		if (world.objectExists(obj_key))
		{
			const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( world.object(obj_key) ) );
			if (objects::Decoration == game_obj.type())
				return true;
		}
	}
	return false;
}

bool shouldDraw( const AHGameObject& game_obj )
{
	return game_obj.shouldDraw();
}

DBKeyValue getFrontObject(const AHGameModel& model, const ObjectList& obj_list, LargeObjects& large_objs, int wx, int wy, bool draw_pings)
{
	const World& world( model.world() );
	DBKeyValue front_obj_key( INVALID_KEY );
	WorldObjectType front_obj_type( objects::None );

	BOOST_FOREACH( DBKeyValue obj_key, obj_list )
	{
		if (world.objectExists(obj_key))
		{
			const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >( world.object(obj_key) ) );

			if (!shouldDraw(game_obj))
				continue;

			if (isLargeObject(game_obj))
			{
				large_objs.push_back( LargeObject( &game_obj, wx, wy ) );
				continue;
			}

			if ((!draw_pings) && (objects::Ping == game_obj.type()))
			{
				const Ping& ping_obj( dynamic_cast< const Ping& >( game_obj ) );
				if (ping::Motion == ping_obj.pingType())
					continue;
			}

			WorldObjectType obj_type( game_obj.type() );
			if (hasPrecedence(obj_type, front_obj_type))
			{
				front_obj_key = obj_key;
				front_obj_type = obj_type;
			}
		}
	}
	return front_obj_key;
}


void drawFrame( RL_shared::Console& console, bool inred )
{
	drawOuterFrame( console, inred );
	drawInnerFrame( console, inred );
}

void drawOuterFrame( RL_shared::Console& console, bool inred )
{
	Console::ConsoleDims dims( console.getConsoleDimensions() );

	const Console::Colour frame_col = inred ? Console::Red : Console::Cyan;

	console.draw(0, 0, '/', frame_col);
	console.draw(dims.x-1, 0, '\\', frame_col);
	console.draw(0, dims.y-1, '\\', frame_col);
	console.draw(dims.x-1, dims.y-1, '/', frame_col);

	for (int dx = 1; dx < (dims.x-1); ++dx)
	{
		console.draw(dx, 0, '-', frame_col);
		console.draw(dx, dims.y-1, '-', frame_col);
	}
	for (int dy = 1; dy < (dims.y-1); ++dy)
	{
		console.draw(0, dy, '|', frame_col);
		console.draw(dims.x-1, dy, '|', frame_col);
	}
}

void drawInnerFrame( RL_shared::Console& console, bool inred )
{
	Console::ConsoleDims dims( console.getConsoleDimensions() );

	const Console::Colour frame_col = inred ? Console::Red : Console::Cyan;

	for (int dy = 1; dy < (dims.y-1); ++dy)
	{
		console.draw(43, dy, '|', frame_col);
	}
	for (int dx = 1; dx < 43; ++dx)
	{
		console.draw(dx, dims.y-6, '-', frame_col);
	}
	for (int dx = 44; dx < dims.x-1; ++dx)
	{
		console.draw(dx, 20, '-', frame_col);
	}

	console.draw(43, 0, '+', frame_col);
	console.draw(43, 20, '+', frame_col);
	console.draw(dims.x-1, 20, '+', frame_col);
	console.draw(0, dims.y-6, '+', frame_col);
	console.draw(43, dims.y-6, '+', frame_col);
	console.draw(43, dims.y-1, '+', frame_col);
}


void drawBar( RL_shared::Console& console, int amount, int max, int top, int start_x, int finish_x, int y, Console::Colour fillcol, Console::Colour backcol )
{
	int fill = 0;
	int len = finish_x - start_x + 1;
	int fill2 = len;

	if (top > 0)
	{
		double frac = (double)amount / (double)top;
		fill = (int)(frac * (double)len);
		double frac2 = (double)max / (double)top;
		fill2 = (int)(frac2 * (double)len);
	}

	int tx(0);
	if (fill > 0)
	{
		for (; tx <= fill; ++tx)
		{
			console.draw(start_x+tx, y, '#', fillcol);
		}
	}
	for (; tx <= fill2; ++tx)
	{
		console.draw(start_x+tx, y, '#', backcol);
	}
}

int drawObjectSelectName(
	Console& console, const World& world, 
	DBKeyValue object_key, 
	int x, int y, Console::Colour colour, 
	bool is_selected_weapon,
	bool redlight
	)
{
	if (world.objectExists(object_key))
	{
		const AHGameObject& game_obj(dynamic_cast< const AHGameObject& >( world.object(object_key) ));

		if (!is_selected_weapon)
		{
			std::string str( game_obj.getSelectName(true) );
			console.drawText(x, y, str.c_str(), colour);		
			return str.length();
		}
		else
		{
			const Weapon& weapon_obj( dynamic_cast< const Weapon& >( game_obj ) );

			std::string str( game_obj.getSelectName(false) );
			console.drawText(x, y, str.c_str(), colour);

			if (0 != weapon_obj.getAmmo())
			{
				std::string status_str(" (");
				status_str += boost::lexical_cast<std::string>(weapon_obj.getAmmo());
				status_str += ")";
				console.drawText(x + str.length(), y, status_str.c_str(), colour);		
				return str.length() + status_str.length();
			}
			else
			{
				int x2 = x + str.length();
				console.drawText(x2, y, " (reload)", redlight ? Console::Red : Console::Green);
				console.draw(x2+5, y, 'o', redlight ? Console::BrightRed : Console::BrightGreen);
				return str.length() + 9;
			}
		}
	}

	console.drawText(x, y, "None", colour);
	return 4;
}

void drawHUD( 
	RL_shared::Console& console, const AHGameModel& model, 
	int x_low, int y_low, 
	int x_high, int y_high, 
	RL_shared::GameTimeCoordinate time_step
	)
{
	const World& world( model.world() );
	DBKeyValue player_key( model.avatar() );
	if (!world.objectExists(player_key))
		return;
	const PlayerCharacter& player( dynamic_cast<const PlayerCharacter&>( world.object(player_key) ) );

	bool countdown( model.isCountdownActive() );

	int hp = player.getHP();
	int hp_max = player.getMaxHP();
	int hp_top = player.getTopHP();
	int hp_diff = hp_top - hp_max;
	int armour = player.getArmourHP(model);
	int armour_max = player.getArmourMaxHP(model);
	int ammo = player.getHeldWeaponAmmoStock(model);
	int ammo_max = player.getHeldWeaponAmmoStockMax(model);

	Console::Colour text_col = countdown ? Console::Red : Console::Green;
	Console::Colour text_hl_col = countdown ? Console::BrightRed : Console::BrightGreen;

	console.drawText(x_low, y_low  , "Life:", text_col);
	console.drawText(x_low, y_low+1, "Armr:", text_col);
	console.drawText(x_low, y_low+2, "Ammo:", text_col);

	drawBar(console, hp, hp_max, hp_top, x_low+6, x_high-9, y_low, Console::BrightRed, Console::Red);
	drawBar(console, armour, armour_max, armour_max, x_low+6, x_high-9, y_low+1, 
		countdown ? Console::BrightRed : Console::BrightCyan, countdown ? Console::Red : Console::Cyan);
	drawBar(console, ammo, ammo_max, ammo_max, x_low+6, x_high-9, y_low+2, 
		countdown ? Console::BrightRed : Console::BrightGreen, countdown ? Console::Red : Console::Green);

	std::string hp_str( (boost::format("%3d/%3d") % hp % hp_max).str() );
	std::string armour_str( (boost::format("%3d/%3d") % armour % armour_max).str() );
	std::string ammo_str( (boost::format("%3d/%3d") % ammo % ammo_max).str() );

	console.drawText(x_high-6, y_low, hp_str.c_str(), text_col);
	console.drawText(x_high-6, y_low+1, armour_str.c_str(), text_col);
	console.drawText(x_high-6, y_low+2, ammo_str.c_str(), text_col);

	int player_acid = player.getAcid();
	if (player_acid > 0)
	{
		std::string acid_str( (boost::format("Acid: %d") % player_acid).str() );
		console.drawText(x_low, y_low+3, acid_str.c_str(), Console::BrightYellow);
	}

	console.draw(x_low, y_low+5, 'A', text_hl_col);
	console.drawText(x_low+1, y_low+5, "rmour:", text_col);
	int armourstrlen = drawObjectSelectName( console, world, player.armour(), x_low+9, y_low+5, text_col, false, countdown );
	if (world.objectExists(player.armour()))
	{
		const Armour& player_armour( dynamic_cast< const Armour& >( world.object(player.armour()) ) );
		int armour_acid = player_armour.getAcid();
		if (armour_acid > 0)
		{
			std::string acid_str( (boost::format("[%d]") % armour_acid).str() );
			console.drawText(x_low+9+armourstrlen, y_low+5, acid_str.c_str(), Console::BrightYellow);
		}
	}

	bool sidearm_selected = PlayerCharacter::Sidearm == player.currentWeapon();
	Console::Colour sidearm_col = sidearm_selected ? text_hl_col : text_col;
	console.draw(x_low, y_low+6, 'S', text_hl_col);
	console.drawText(x_low+1, y_low+6, "idearm:", sidearm_col);
	drawObjectSelectName( console, world, player.sidearm(), x_low+9, y_low+6, sidearm_col, sidearm_selected, countdown );

	bool primary_selected = PlayerCharacter::Primary == player.currentWeapon();
	Console::Colour primary_col = primary_selected ? text_hl_col : text_col;
	console.draw(x_low, y_low+7, 'P', text_hl_col);
	console.drawText(x_low+1, y_low+7, "rimary:", primary_col);
	drawObjectSelectName( console, world, player.primary(), x_low+9, y_low+7, primary_col, primary_selected, countdown );

	int weight( player.getWeight(model) );
	if ((weight <= 0) && (world.objectExists(player.armour())))
	{
		console.drawText(x_low, y_low+8, "Weight:  N/A", text_col);
	}
	else
	{
		boost::format weight_frm("Weight:  %d.%d kg");
		weight_frm % (weight/1000) % (weight%1000);
		std::string weight_str( weight_frm.str() );
		console.drawText(x_low, y_low+8, weight_str.c_str(), text_col);

		int penalty( player.getWeightPenalty(model, weight) );
		if (penalty > 0)
		{
			boost::format penalty_str("-%d.%d%%");
			penalty_str % (penalty/10000) % ((penalty%10000)/100);
			console.drawText(x_low+weight_str.length()+1, y_low+8, penalty_str.str().c_str(), Console::Red);
		}
		else if (penalty < 0)
		{
			penalty *= -1;
			boost::format bonus_str("+%d.%d%%");
			bonus_str % (penalty/10000) % ((penalty%10000)/100);
			console.drawText(x_low+weight_str.length()+1, y_low+8, bonus_str.str().c_str(), Console::Cyan);
		}
	}

	console.drawText(x_low, y_low+10, "Grenades", text_col);

	console.drawText(x_low, y_low+11, "S un:", text_col);
	console.draw(x_low+1, y_low+11, 't', text_hl_col);
	int num_stun = (std::min)(9, player.num( pickup::StunGrenade ));
	console.drawText(x_low+6, y_low+11, (lexical_cast<std::string>(num_stun)).c_str(), text_col);

	console.drawText(x_low+9, y_low+11, "nc.: ", text_col);
	console.draw(x_low+8, y_low+11, 'I', text_hl_col);
	int num_inc = (std::min)(9, player.num( pickup::IncGrenade ));
	console.drawText(x_low+14, y_low+11, (lexical_cast<std::string>(num_inc)).c_str(), text_col);

	console.drawText(x_low+16, y_low+11, "F ag: ", text_col);
	console.draw(x_low+17, y_low+11, 'r', text_hl_col);
	int num_frag = (std::min)(9, player.num( pickup::FragGrenade ));
	console.drawText(x_low+22, y_low+11, (lexical_cast<std::string>(num_frag)).c_str(), text_col);

	console.drawText(x_low+25, y_low+11, "rak: ", text_col);
	console.draw(x_low+24, y_low+11, 'K', text_hl_col);
	int num_krak = (std::min)(9, player.num( pickup::KrakGrenade ));
	console.drawText(x_low+30, y_low+11, (lexical_cast<std::string>(num_krak)).c_str(), text_col);

	console.drawText(x_low, y_low+13, "Items", text_col);

	console.drawText(x_low+1, y_low+14, "ed.: ", text_col);
	console.draw(x_low, y_low+14, 'M', text_hl_col);
	int num_med = (std::min)(9, player.num( pickup::Medkit ));
	console.drawText(x_low+6, y_low+14, (lexical_cast<std::string>(num_med)).c_str(), text_col);

	console.drawText(x_low+9, y_low+14, "eut: ", text_col);
	console.draw(x_low+8, y_low+14, 'N', text_hl_col);
	int num_neut = (std::min)(9, player.num( pickup::Neutraliser ));
	console.drawText(x_low+14, y_low+14, (lexical_cast<std::string>(num_neut)).c_str(), text_col);

	console.drawText(x_low+16, y_low+14, "D mo: ", text_col);
	console.draw(x_low+17, y_low+14, 'e', text_hl_col);
	int num_demo = (std::min)(9, player.num( pickup::DemoCharge ));
	console.drawText(x_low+22, y_low+14, (lexical_cast<std::string>(num_demo)).c_str(), text_col);

	GameTimeCoordinate time( countdown ? model.getCountdownTime() : model.gameTime() );
	int ms = (time % 1000);
	int secs = (time / 1000) % 60;
	int mins = (time / 60000) % 60;
	int hours = (time / (60*60000)) % 60;
	std::string time_str( (boost::format("%02d:%02d:%02d:%03d") % hours % mins % secs % ms).str() );
	std::string full_time_str;
	if (countdown)
	{
		(full_time_str = "Reactor: ") += time_str;
		console.drawText(x_low, y_high, full_time_str.c_str(), mins < 10 ? Console::BrightYellow : Console::BrightRed);
	}
	else
	{
		(full_time_str = "Time: ") += time_str;
		console.drawText(x_low, y_high, full_time_str.c_str(), text_col);
	}
	if (time_step > 0)
	{
		std::string step_string(countdown ? "-" : "+");
		step_string += boost::lexical_cast< std::string >( time_step );
		console.drawText(x_low + time_str.length() + 1, y_high, step_string.c_str(), countdown ? Console::Magenta : Console::Cyan);
	}

	if (player.hasMotionDetection(model))
	{
		console.drawText(x_high-9, y_high, "-TRACKING-", text_col);
	}
}


void writeSelectedItems(
	RL_shared::Console& console, const AHGameModel& model, 
	RL_shared::DBKeyValue focus_zone, 
	int focus_x, int focus_z, 
	int vis_focus_x, int vis_focus_z, 
	bool inred, bool show_pings, bool show_items
)
{
	Console::ConsoleDims dims( console.getConsoleDimensions() );

	Console::Colour col = inred ? Console::Red : Console::Green;

	const int SELECTED_WINDOW_Y = dims.y-4;
	const int SELECTED_WINDOW_W = 40;

	const World& world( model.world() );
	const VisionField& visibility( model.visionField() );
	const Zone& zone( world.zone(focus_zone) );

	if (!zone.isWithin(focus_x, focus_z))
		return;

	bool outside_vision( !model.isVisible(focus_zone, focus_x, focus_z) );

	bool outside_range( false );

	{
		int dx = focus_x - vis_focus_x;
		int dz = focus_z - vis_focus_z;
		outside_range = ((dx*dx)+(dz*dz)) > PING_RANGE_SQUARED;
	}

	std::string selected_items_str_1;
	std::string selected_items_str_2;

	if (outside_vision)
	{
		selected_items_str_1 += "Outside vision";
	}
	else
	{
		TerrainType there( zone.terrainAt( focus_x, focus_z ) );
		selected_items_str_1 += getStringForTerrain( there );
	}

	bool not_all_could_fit( false );

	const ObjectList& objects( zone.objectsAt( focus_x, focus_z ) );
	BOOST_FOREACH( DBKeyValue obj_key, objects )
	{
		if (world.objectExists( obj_key ))
		{
			std::string obj_string;

			const AHGameObject& game_obj( dynamic_cast<const AHGameObject&>( world.object( obj_key ) ) );

			if (objects::Ping == game_obj.type())
				if (!show_pings)
				{
					const Ping& ping_obj( dynamic_cast< const Ping& >( game_obj ) );
					if (ping::Motion == ping_obj.pingType())
						continue;
				}

			if (show_items)
			{
				if (outside_vision 
					&& (objects::Pickup != game_obj.type()) && (objects::Armour != game_obj.type()) 
					&& (objects::Weapon != game_obj.type()) && (objects::Terminal != game_obj.type()) 
					)
					continue;
			}
			else
			{
				if (outside_range)
					continue;

				if (outside_vision && (objects::Ping != game_obj.type()))
					continue;
			}

			obj_string += getStringForObject( game_obj );

			if (game_obj.shouldDraw() && (!obj_string.empty()))
			{
				std::string new_obj_string(" | ");
				new_obj_string += obj_string;

				int len( new_obj_string.length() );
				if (selected_items_str_1.length() + len <= SELECTED_WINDOW_W)
					selected_items_str_1 += new_obj_string;
				else if (selected_items_str_2.empty() && (selected_items_str_2.length() + obj_string.length() <= SELECTED_WINDOW_W))
					selected_items_str_2 += obj_string;
				else if (selected_items_str_2.length() + len <= SELECTED_WINDOW_W)
					selected_items_str_2 += new_obj_string;
				else
					not_all_could_fit = true;
			}
		}
	}

	console.drawText(2, SELECTED_WINDOW_Y, selected_items_str_1.c_str(), col);
	console.drawText(2, SELECTED_WINDOW_Y+1, selected_items_str_2.c_str(), col);
	if (not_all_could_fit)
		console.drawText(2, SELECTED_WINDOW_Y+2, "...", col);
}


void drawZoneName( RL_shared::Console& console, const AHGameModel& model, DBKeyValue focus_zone, bool inred )
{
	const OverWorld& overworld( model.overworld() );
	OverWorld::BlockAndFloor bnf = overworld.getBlockAndFloor( focus_zone );
	if ((bnf.block < 0) || (bnf.floor < 0) || (bnf.floor >= 4))
		return;
	const char * floor_names[4] = {" Ground Floor ", " 1st Floor ", " 2nd Floor ", " 3rd Floor "};
	std::string block_name;
	std::string floor_name( floor_names[bnf.floor] );

	block_name += " ";
	block_name += overworld.blockName(bnf.block);
	block_name += " ";

	int block_name_start = WORLD_VIEW_X_LOW + ((WORLD_VIEW_X_HI-WORLD_VIEW_X_LOW)/2) - (block_name.length()/2);
	int floor_name_start = WORLD_VIEW_X_LOW + ((WORLD_VIEW_X_HI-WORLD_VIEW_X_LOW)/2) - (floor_name.length()/2);

	console.drawText(block_name_start, WORLD_VIEW_Y_LOW-1, block_name.c_str(), inred ? Console::Red : Console::Green);
	console.drawText(floor_name_start, WORLD_VIEW_Y_HI+1, floor_name.c_str(), inred ? Console::Red : Console::Green);
}


DrawItem getDrawItem(DBKeyValue draw_obj, const World& world, bool is_countdown)
{
	try
	{
		const WorldObject& obj( world.object(draw_obj) );
		const AHGameObject& game_obj( dynamic_cast<const AHGameObject&>(obj) );
		return getObjectItemRepresentation(game_obj, is_countdown);
	}
	catch(...)//(World::NoObjectWithKey&)
	{
	}
	return DrawItem('?', Console::BrightMagenta, Console::Red);
}


void drawWorld(
	Console& console, const AHGameModel& model, 
	DBKeyValue focus_zone, 
	int focus_x, int focus_z, 
	int vis_focus_x, int vis_focus_z, 
	int x_low, int x_high, 
	int y_low, int y_high, 
	cursor_type::Cursor cursor, 
	int cursor_offs_x, int cursor_offs_z, 
	bool no_visibility, 
	bool draw_pings
	)
{
	int window_size_x = x_high - x_low + 1;
	int window_size_y = y_high - y_low + 1;
	int window_origin_in_zone_x = focus_x - (window_size_x/2);
	int window_origin_in_zone_z = focus_z - (window_size_y/2);

	const World& world( model.world() );
	const VisionField& visibility( model.visionField() );
	const Zone& zone( world.zone(focus_zone) );

	bool is_countdown = model.isCountdownActive();
	const OverWorld& overworld( model.overworld() );
	OverWorld::BlockAndFloor bnf = overworld.getBlockAndFloor(focus_zone);
	bool is_nest_level = (9 == bnf.block) && (bnf.floor > 1);

	int vbase_x = (((int)visibility.sizeX() - window_size_x) / 2) - 1 + (window_size_x & 1);
	int vbase_z = (((int)visibility.sizeZ() - window_size_y) / 2) - 1 + (window_size_y & 1);
	vbase_x += focus_x - vis_focus_x;
	vbase_z += focus_z - vis_focus_z;

	LargeObjects large_objs;

	for (int iz=0; iz < window_size_y; ++iz)
	{
		for (int ix=0; ix < window_size_x; ++ix)
		{
			int wx = x_low+ix;
			int wy = y_high-iz;

			int zx = window_origin_in_zone_x + ix;
			int zz = window_origin_in_zone_z + iz;
			if ((zx < 0) || (zz < 0) || (zx >= zone.sizeX()) || (zz >= zone.sizeZ()))
			{
				console.draw( wx, wy, ' ', Console::Green, Console::Black );
				continue;
			}

			bool visible = false;
			if (!no_visibility)
			{
				int vx = vbase_x + ix;
				int vz = vbase_z + iz;
				if ((vx >= 0) && (vz >= 0))
				{
					visible = visibility.sample( (boost::uint16_t)vx, (boost::uint16_t)vz );
				}
			}

			DrawItem draw('?', Console::BrightMagenta, Console::Red);

			const ObjectList& objects( zone.objectsAt(zx, zz) );

			if (visible)
			{
				DBKeyValue draw_obj( getFrontObject( model, objects, large_objs, wx, wy, draw_pings ) );

				TerrainType terrain( zone.terrainAt(zx, zz) );

				if (terrainHasPriority(world, draw_obj, terrain))
					draw_obj = INVALID_KEY;

				if (INVALID_KEY != draw_obj)
				{
					draw = getDrawItem(draw_obj, world, is_countdown);
				}
				else
				{
					draw = getTerrainItemRepresentation( terrain, true, is_nest_level, is_countdown );
				}
			}
			else
			{
				bool drawn_obj( false );
				bool mapped( 0 != zone.recordedObjectAt(zx, zz) );
				bool ping_catchment( !no_visibility );

				if (mapped || ping_catchment)
				{
					int dx = zx - vis_focus_x;
					int dz = zz - vis_focus_z;
					ping_catchment = (((dx*dx)+(dz*dz)) < PING_RANGE_SQUARED);
				}

				if (mapped || ping_catchment)
				{
					GameTimeCoordinate ping_time(0);
					BOOST_FOREACH( DBKeyValue obj_key, objects )
					{
						if (world.objectExists(obj_key))
						{
							const AHGameObject& game_obj( dynamic_cast< const AHGameObject& >(world.object(obj_key)) );

							if (!game_obj.shouldDraw())
								continue;

							if (isLargeObject(game_obj))
							{
								large_objs.push_back( LargeObject( &game_obj, wx, wy ) );
								continue;
							}

							if (objects::Ping == game_obj.type())
							{
								if (!ping_catchment)
									continue;
								if (no_visibility)
									continue;
								const Ping& ping_obj( dynamic_cast< const Ping& >(game_obj) );
								if (draw_pings || (ping::Motion != ping_obj.pingType()))
								{
									if (ping_obj.timeRemaining() > ping_time)
									{
										ping_time = ping_obj.timeRemaining();
										draw = getObjectItemRepresentation(game_obj, is_countdown);
										drawn_obj = true;
										continue;
									}
								}
							}

							if ((!mapped) || (drawn_obj))
								continue;

							if ((objects::Pickup == game_obj.type()) || (objects::Armour == game_obj.type())
								|| (objects::Weapon == game_obj.type()) || (objects::Terminal == game_obj.type()))
							{
								draw = getObjectItemRepresentation(game_obj, is_countdown);
								draw.fore = is_countdown ? Console::BrightRed : Console::BrightBlue;
								draw.back = Console::Black;
								drawn_obj = true;
							}
						}
					}
				}

				if (!drawn_obj)
				{
					TerrainType terrain( zone.recordedTerrainAt(zx, zz) );
					draw = getTerrainItemRepresentation( terrain, false, is_nest_level, is_countdown );
				}
			}

			if ((focus_x + cursor_offs_x == zx) && (focus_z + cursor_offs_z == zz))
			{
				if (cursor_type::Target == cursor)
				{
					draw.fore = Console::BrightRed;
					draw.back = Console::Red;
					if (('.' == draw.chr) || (' ' == draw.chr))
					{
						draw.chr = 'X';
					}
				}
				else if (cursor_type::Look == cursor)
				{
					draw.fore = Console::BrightGreen;
					draw.back = Console::Green;
				}
			}

			console.draw( wx, wy, draw.chr, draw.fore, draw.back );
		}
	}

	BOOST_FOREACH( LargeObject& draw_obj, large_objs )
	{
		drawLargeObject(console, model, *(draw_obj.obj), draw_obj.wx, draw_obj.wy, x_low, y_low, x_high, y_high);
	}
}


}
