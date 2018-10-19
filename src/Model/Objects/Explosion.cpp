#include "Explosion.hpp"
#include "ObjectType.hpp"
#include "TerrainType.hpp"
#include "Fire.hpp"
#include "Alien.hpp"
#include "../AHGameModel.hpp"
#include "../LoSSampler.hpp"
#include "World-2DTiles/Zone.hpp"
#include "Algorithm/Algorithms-2D.hpp"
#include "assert.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;



namespace explosion
{
	Type getExplosionType( pickup::Type type )
	{
		switch( type )
		{
			case pickup::FragGrenade:	return FragBlast;
			case pickup::IncGrenade:	return IncBlast;
			case pickup::KrakGrenade:	return KrakBlast;
			case pickup::StunGrenade:	return StunFlash;
			default:;
		}
		ASSERT(false);
		return None;
	}

	GameTimeCoordinate getExplosionTime( Type type )
	{
		return 500;
	}


	const GameTimeCoordinate GRENADE_STUN_TIME = 5000;
	const GameTimeCoordinate INC_FIRE_TIME = 10000;
	const GameTimeCoordinate INC_FIRE_VARIANCE = 1000;
	const GameTimeCoordinate INC_SMOKE_TIME = 3000;
	const int INC_FIRE_DAMAGE = 4;


	int getExplosionRange(Type type)
	{
		switch(type)
		{
			case FragBlast:
				return 4;
			case IncBlast:
				return 2;
			case StunFlash:
				return 5;
			case KrakBlast:
				return 1;
			case BarrelBlast: 
				return 4;
			case DemoBlast:
				return 2;
			default:;
		}
		return 4;
	}

	int getExplosionSoundRange(Type type)
	{
		switch(type)
		{
			case FragBlast:
				return 25;
			case IncBlast:
				return 10;
			case StunFlash:
				return 10;
			case KrakBlast:
				return 15;
			case BarrelBlast:
				return 30;
			case DemoBlast:
				return 40;
			default:;
		}
		return 20;
	}



void makeExplosion(
	AHGameModel& model, 
	DBKeyValue caused_by, 
	WorldObject::WorldLocation hit_loc, 
	Type explosion_type, 
	bool flame_special_1, 
	bool flame_special_2 
	)
{
	World& world(model.world());

	if (world.zoneExists(hit_loc.zone))
	{
		Zone& zone( world.zone(hit_loc.zone) );

		shared_ptr< Explosion > expl_obj( new Explosion( model, explosion_type ) );
		DBKeyValue expl_key( world.addWorldObject( expl_obj ) );
		expl_obj->moveTo(model, hit_loc, true);

		int sound_range = getExplosionSoundRange(explosion_type);
		soundAlertAliens(model, hit_loc.zone, hit_loc.x, hit_loc.z, sound_range);

		if (KrakBlast == explosion_type)
		{
			for (int offz=-1; offz < 2; ++offz)
				for (int offx=-1; offx < 2; ++offx)
				{
					int tx = hit_loc.x + offx;
					int tz = hit_loc.z + offz;
					if (zone.isWithin(tx,tz))
					{
						const ObjectList& objects( zone.objectsAt(tx, tz) );
						BOOST_FOREACH( DBKeyValue object_key, objects )
						{
							if (world.objectExists(object_key))
							{
								AHGameObject& game_obj( dynamic_cast< AHGameObject& >( world.object(object_key) ) );
								game_obj.takeDamage( model, caused_by, damage::Explosion, 10, 3 );
								game_obj.takeDamage( model, caused_by, damage::Explosion, 10, 3 );
								game_obj.takeDamage( model, caused_by, damage::Explosion, 10, 3 );
							}
						}
					}
				}
		}
		else
		if ((FragBlast == explosion_type) || (StunFlash == explosion_type) || (BarrelBlast == explosion_type))
		{
			int radius = getExplosionRange(explosion_type);
			int radius_sq = radius * radius;
			for (int offz=-radius; offz <= radius; ++offz)
				for (int offx=-radius; offx <= radius; ++offx)
				{
					if ( ((offx*offx) + (offz*offz)) > radius_sq )
						continue;

					int tx = hit_loc.x + offx;
					int tz = hit_loc.z + offz;
					if (zone.isWithin(tx,tz))
					{
						bool makefire = ((BarrelBlast == explosion_type) && (1 == (rand()&7)));

						const ObjectList& objects( zone.objectsAt(tx, tz) );

						if ((!objects.empty()) || (makefire))
						{
							if (!terrain::isPassable(zone.terrainAt(tx, tz)))
								continue;
							LoSSampler hit_test(zone, 0, tx, tz);
							lineCast( hit_loc.x, hit_loc.z, tx, tz, hit_test, VisitCellFunctor() );
							if ((hit_test.finish_x != tx) || (hit_test.finish_z != tz))
								continue;
						}

						BOOST_FOREACH( DBKeyValue object_key, objects )
						{
							if (world.objectExists(object_key))
							{
								AHGameObject& game_obj( dynamic_cast< AHGameObject& >( world.object(object_key) ) );
								if (StunFlash == explosion_type)
								{
									game_obj.stun( model, GRENADE_STUN_TIME );
								}
								else if (FragBlast == explosion_type)
								{
									game_obj.takeDamage( model, caused_by, damage::Explosion, 9, 0 );
									game_obj.takeDamage( model, caused_by, damage::Explosion, 9, 0 );
								}
								else if (BarrelBlast == explosion_type)
								{
									game_obj.takeDamage( model, caused_by, damage::Explosion, 7, 0 );
									game_obj.takeDamage( model, caused_by, damage::Explosion, 7, 0 );
								}
							}
						}

						if (makefire)
						{
							GameTimeCoordinate fire_time  = 4*(INC_FIRE_TIME  - INC_FIRE_VARIANCE + (rand() % (2*INC_FIRE_VARIANCE)));
							GameTimeCoordinate smoke_time = 2*(INC_SMOKE_TIME - INC_FIRE_VARIANCE + (rand() % (2*INC_FIRE_VARIANCE)));
							shared_ptr< Fire > fire( new Fire( fire_time, smoke_time, INC_FIRE_DAMAGE/2 ) );
							DBKeyValue fire_key( world.addWorldObject( fire ) );
							fire->moveTo( model, WorldObject::WorldLocation( hit_loc.zone, tx, tz ), true );
						}
					}
				}
		}
		else if (IncBlast == explosion_type)
		{
			int rangemax(4), rangemin(-2), rangesq(8);
			if (flame_special_2)
			{
				rangemax = 5; 
				rangemin = -3;
				rangesq = 11;
			}
			for (int offz=rangemin; offz <= rangemax; ++offz)
				for (int offx=rangemin; offx <= rangemax; ++offx)
				{
					if (((offx*offx)+(offz*offz)) >= rangesq)
						continue;

					int tx = hit_loc.x + offx;
					int tz = hit_loc.z + offz;
					if (zone.isWithin(tx,tz))
					{
						LoSSampler hit_test(zone, 0, tx, tz);
						lineCast( hit_loc.x, hit_loc.z, tx, tz, hit_test, VisitCellFunctor() );
						if ((hit_test.finish_x != tx) || (hit_test.finish_z != tz))
							continue;

						GameTimeCoordinate fire_time  = INC_FIRE_TIME  - INC_FIRE_VARIANCE + (rand() % (2*INC_FIRE_VARIANCE));
						GameTimeCoordinate smoke_time = INC_SMOKE_TIME - INC_FIRE_VARIANCE + (rand() % (2*INC_FIRE_VARIANCE));
						shared_ptr< Fire > fire( new Fire( fire_time, smoke_time, INC_FIRE_DAMAGE*(flame_special_1?2:1) ) );
						DBKeyValue fire_key( world.addWorldObject( fire ) );
						fire->moveTo( model, WorldObject::WorldLocation( hit_loc.zone, tx, tz ), true );
					}
				}
		}
	}
}

void demoBlastDamage(AHGameModel& model, World& world, Zone& zone, int bx, int bz, int tx, int tz, DBKeyValue caused_by, int damage)
{
	ASSERT( damage > 0 );

	if (!zone.isWithin(tx, tz))
		return;

	const ObjectList& objects( zone.objectsAt(tx, tz) );

	if (!objects.empty())
	{
		int dx = tx-bx;
		int dz = bz-tz;

		if (2 <= (dx*dx)+(dz*dz))
		{
			LoSSampler hit_test(zone, 0, tx, tz);
			lineCast( bx, bz, tx, tz, hit_test, VisitCellFunctor() );
			if ((hit_test.finish_x != tx) || (hit_test.finish_z != tz))
				return;
		}
	}

	BOOST_FOREACH( DBKeyValue object_key, objects )
	{
		if (world.objectExists(object_key))
		{
			AHGameObject& game_obj( dynamic_cast< AHGameObject& >( world.object(object_key) ) );
			game_obj.takeDamage(model, caused_by, damage::Explosion, damage, rand()%damage);
		}
	}
}

void makeDemoExplosion(AHGameModel& model, DBKeyValue caused_by, WorldObject::WorldLocation hit_loc, int dx, int dz)
{
	World& world(model.world());

	if (world.zoneExists(hit_loc.zone))
	{
		Zone& zone( world.zone(hit_loc.zone) );

		shared_ptr< Explosion > expl_obj( new Explosion( model, DemoBlast ) );
		DBKeyValue expl_key( world.addWorldObject( expl_obj ) );
		expl_obj->moveTo(model, hit_loc, true);

		int sound_range = getExplosionSoundRange(DemoBlast);
		soundAlertAliens(model, hit_loc.zone, hit_loc.x, hit_loc.z, sound_range);

		int bx = hit_loc.x+dx;
		int bz = hit_loc.z+dz;
		if (zone.isWithin(bx, bz) && zone.isWithin(bx+dx, bz+dz) && (terrain::Wall == zone.terrainAt(bx, bz)))
		{
			TerrainType next = zone.terrainAt(bx+dx, bz+dz);
			if (terrain::isClear(next) || terrain::isBroken(next))
			{
				zone.putTerrain(terrain::Wall | terrain::Broken, bx, bz);

				if (zone.isWithin(bx-1, bz) && (terrain::Door == terrain::getType( zone.terrainAt(bx-1, bz) )))
					zone.putTerrain( zone.terrainAt(bx-1, bz) | terrain::Broken, bx-1, bz );
				if (zone.isWithin(bx+1, bz) && (terrain::Door == terrain::getType( zone.terrainAt(bx+1, bz) )))
					zone.putTerrain( zone.terrainAt(bx+1, bz) | terrain::Broken, bx+1, bz );
				if (zone.isWithin(bx, bz-1) && (terrain::Door == terrain::getType( zone.terrainAt(bx, bz-1) )))
					zone.putTerrain( zone.terrainAt(bx, bz-1) | terrain::Broken, bx, bz-1 );
				if (zone.isWithin(bx, bz+1) && (terrain::Door == terrain::getType( zone.terrainAt(bx, bz+1) )))
					zone.putTerrain( zone.terrainAt(bx, bz+1) | terrain::Broken, bx, bz+1 );

				//Damage close to the explosive
				{
					for (int tz = hit_loc.z-1; tz <= hit_loc.z+1; ++tz)
						for (int tx = hit_loc.x-1; tx <= hit_loc.x+1; ++tx)
						{
							demoBlastDamage(model, world, zone, hit_loc.x, hit_loc.z, tx, tz, caused_by, 20);
						}
				}

				int sdx = (0 == dx) ? 1 : 0;
				int sdz = (0 == dz) ? 1 : 0;

				//Damage levels and pattern for dx=1, dz=0:
				//   3
				//  123
				// X123
				//  123
				//   3

				demoBlastDamage(model, world, zone, bx, bz, bx+dx+sdx, bz+dz+sdz, caused_by, 30);
				demoBlastDamage(model, world, zone, bx, bz, bx+dx    , bz+dz    , caused_by, 30);
				demoBlastDamage(model, world, zone, bx, bz, bx+dx-sdx, bz+dz-sdz, caused_by, 30);
				demoBlastDamage(model, world, zone, bx, bz, bx+(2*dx)+sdx, bz+(2*dz)+sdz, caused_by, 20);
				demoBlastDamage(model, world, zone, bx, bz, bx+(2*dx)    , bz+(2*dz)    , caused_by, 20);
				demoBlastDamage(model, world, zone, bx, bz, bx+(2*dx)-sdx, bz+(2*dz)-sdz, caused_by, 20);
				demoBlastDamage(model, world, zone, bx, bz, bx+(2*dx)+sdx, bz+(2*dz)+(2*sdz), caused_by, 10);
				demoBlastDamage(model, world, zone, bx, bz, bx+(2*dx)+sdx, bz+(3*dz)+sdz, caused_by, 10);
				demoBlastDamage(model, world, zone, bx, bz, bx+(2*dx)    , bz+(3*dz)    , caused_by, 10);
				demoBlastDamage(model, world, zone, bx, bz, bx+(2*dx)-sdx, bz+(3*dz)-sdz, caused_by, 10);
				demoBlastDamage(model, world, zone, bx, bz, bx+(2*dx)-sdx, bz+(2*dz)-(2*sdz), caused_by, 10);
			}
		}
	}
}


}



Explosion::Explosion(
	AHGameModel& model, 
	explosion::Type type
	)
	: AHGameObject( objects::Explosion )
	, m_explosion_type( type )
	, m_time_full( 0 )
	, m_time_remaining( 0 )
{
	m_time_full = 
		m_time_remaining = getExplosionTime( type );

	model.incFastMovingObjects();
}

//Constructor for serialization use.
Explosion::Explosion(void)
	: AHGameObject( objects::Explosion )
	, m_explosion_type( explosion::None )
	, m_time_full( 0 )
	, m_time_remaining( 0 )
{
}

bool Explosion::canOverlap(const RL_shared::WorldObject&) const
{
	return true;
}
bool Explosion::canOverlap(TerrainType) const
{
	return true;
}

void Explosion::notifyTimeStep( AHGameModel& model, GameTimeCoordinate dt )
{
	m_time_remaining -= dt;

	if (m_time_remaining <= 0)
		m_time_remaining = 0;
}

bool Explosion::removeMe(AHGameModel&) const
{
	return m_time_remaining <= 0;
}

bool Explosion::shouldDraw(void) const
{
	return (m_time_remaining > 0);
}

std::string Explosion::getThereIsString(bool) const
{
	return "";
}

std::string Explosion::getSelectName(bool) const
{
	return "Explosion";
}


}
