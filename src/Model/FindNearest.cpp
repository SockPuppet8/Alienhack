#include "FindNearest.hpp"
#include "AHGameModel.hpp"
#include "Objects/ObjectType.hpp"
#include "Objects/Alien.hpp"
#include "World-2DTiles/VisionField.hpp"
#include "World-2DTiles/Zone.hpp"
#include <boost/foreach.hpp>


namespace AlienHack
{


using namespace RL_shared;
using namespace boost;


DBKeyValue findNearestInterestingThing( const AHGameModel& model, bool threats_only )
{
	const World& world( model.world() );

	DBKeyValue avatar_key( model.avatar() );
	if (!world.objectExists(avatar_key))
		return INVALID_KEY;

	const WorldObject& avatar( world.object(avatar_key) );
	WorldObject::WorldLocation loc( avatar.location() );
	if (!world.zoneExists(loc.zone))
		return INVALID_KEY;

	const Zone& zone( world.zone( loc.zone ) );
	const VisionField& vision( model.visionField() );

	int ox(0), oz(0);
	vision.getFieldOrigin(ox, oz);

	int nds( (vision.sizeX()*vision.sizeX()) + (vision.sizeZ()*vision.sizeZ()) );
	DBKeyValue nobj( INVALID_KEY );

	for (int iz=0; iz < ((int)vision.sizeZ()); ++iz)
		for (int ix=0; ix < ((int)vision.sizeX()); ++ix)
		{
			if (vision.sample((uint16_t)ix, (uint16_t)iz))
			{
				int tx = ox+ix;
				int tz = oz+iz;
				if ((tx >= 0) && (tz >= 0) && (tx < zone.sizeX()) && (tz < zone.sizeZ()))
				{
					const ObjectList& objects( zone.objectsAt(tx, tz) );

					BOOST_FOREACH( DBKeyValue obj_key, objects )
					{
						if (world.objectExists(obj_key))
						{
							const AHGameObject& obj( dynamic_cast<const AHGameObject&>( world.object(obj_key) ) );
							WorldObjectType type( obj.type() );

							bool interesting( obj.isInteresting(threats_only) );

							if (threats_only)
								interesting &= (objects::Alien == type);

							if (interesting)
							{
								int dx = tx - loc.x;
								int dz = tz - loc.z;
								int ds = (dx*dx)+(dz*dz);
								if (ds < nds)
								{
									nds = ds;
									nobj = obj_key;
								}
							}
						}
					}
				}
			}
		}

	return nobj;
}



}
