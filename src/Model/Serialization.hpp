#ifndef ALIENHACK_SERIALIZATION_HPP
#define	ALIENHACK_SERIALIZATION_HPP




//Disable this warning from deep inside boost somewhere.
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable: 4308 )
#endif

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/scoped_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>


//A useful macro for classes whose base is a pure virtual interface which is not serialized. 
# define BOOST_SERIALIZATION_WORKAROUND_BASE_AND_DERIVED(Base,Derived) \
namespace { \
  static int BOOST_PP_CAT(boost_serialization_workaround_init_, __LINE__) =    \
  (::boost::archive::detail::instantiate_ptr_serialization((Derived*)0,0,boost::archive::detail::adl_tag()), 3); \
  static int BOOST_PP_CAT(boost_serialization_workaround_init2_, __LINE__) = ( \
      ::boost::serialization::void_cast_register((Derived*)0,(Base*)0)         \
    , 3);                                                                      \
}




#include "AHGameModel.hpp"
#include "OverWorld.hpp"
#include "Objects/PlayerCharacter.hpp"
#include "Objects/Alien.hpp"
#include "Objects/Pickup.hpp"
#include "Objects/Armour.hpp"
#include "Objects/Weapon.hpp"
#include "Objects/Terminal.hpp"
#include "Objects/Explosion.hpp"
#include "Objects/Explosive.hpp"
#include "Objects/Fire.hpp"
#include "Objects/Barrel.hpp"
#include "Objects/Furniture.hpp"
#include "Objects/Decoration.hpp"
#include "Objects/Projectile.hpp"
#include "Objects/Ping.hpp"
#include "Objects/LaserTrail.hpp"
#include "Objects/Egg.hpp"
#include "Objects/PlayerStats.hpp"
#include "Actions/WaitAction.hpp"
#include "Actions/DiscreteMoveAction.hpp"
#include "Actions/DiscreteTurnAction.hpp"
#include "Actions/OpenDoorAction.hpp"
#include "Actions/CloseDoorAction.hpp"
#include "Actions/BreakDoorAction.hpp"
#include "Actions/WeaponAttackAction.hpp"
#include "Actions/PickupAction.hpp"
#include "Actions/PickupArmourAction.hpp"
#include "Actions/DropArmourAction.hpp"
#include "Actions/PickupWeaponAction.hpp"
#include "Actions/DropWeaponAction.hpp"
#include "Actions/DropItemAction.hpp"
#include "Actions/SwitchWeaponAction.hpp"
#include "Actions/ReloadWeaponAction.hpp"
#include "Actions/ThrowGrenadeAction.hpp"
#include "Actions/UseItemAction.hpp"
#include "Actions/SetChargeAction.hpp"
#include "Actions/UseTerminalAction.hpp"
#include "Actions/DirectAttackAction.hpp"
#include "Actions/EnterExitVentAction.hpp"
#include "Actions/HuggerAttackAction.hpp"
#include "Actions/SpitterAttackAction.hpp"
#include "Actions/TakeAmmoAction.hpp"


BOOST_CLASS_EXPORT_GUID(RL_shared::FastAction, "RL_shared::FastAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::CloseDoorAction, "AlienHack::CloseDoorAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::DirectAttackAction, "AlienHack::DirectAttackAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::DiscreteMoveAction, "AlienHack::DiscreteMoveAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::DiscreteTurnAction, "AlienHack::DiscreteTurnAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::DropArmourAction, "AlienHack::DropArmourAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::DropWeaponAction, "AlienHack::DropWeaponAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::DropItemAction, "AlienHack::DropItemAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::EnterExitVentAction, "AlienHack::EnterExitVentAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::HasCompletionCallback, "AlienHack::HasCompletionCallback")
BOOST_CLASS_EXPORT_GUID(AlienHack::IObserveActionComplete, "AlienHack::IObserveActionComplete")
BOOST_CLASS_EXPORT_GUID(AlienHack::OpenDoorAction, "AlienHack::OpenDoorAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::BreakDoorAction, "AlienHack::BreakDoorAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::PickupAction, "AlienHack::PickupAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::PickupArmourAction, "AlienHack::PickupArmourAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::PickupWeaponAction, "AlienHack::PickupWeaponAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::ReloadWeaponAction, "AlienHack::ReloadWeaponAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::SwitchWeaponAction, "AlienHack::SwitchWeaponAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::ThrowGrenadeAction, "AlienHack::ThrowGrenadeAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::UseItemAction, "AlienHack::UseItemAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::SetChargeAction, "AlienHack::SetChargeAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::UseTerminalAction, "AlienHack::UseTerminalAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::WaitAction, "AlienHack::WaitAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::WeaponAttackAction, "AlienHack::WeaponAttackAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::HuggerAttackAction, "AlienHack::HuggerAttackAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::SpitterAttackAction, "AlienHack::SpitterAttackAction")
BOOST_CLASS_EXPORT_GUID(AlienHack::TakeAmmoAction, "AlienHack::TakeAmmoAction")

BOOST_CLASS_EXPORT_GUID(AlienHack::AcidSplashable, "AlienHack::AcidSplashable")
BOOST_CLASS_EXPORT_GUID(AlienHack::Alien, "AlienHack::Alien")
BOOST_CLASS_EXPORT_GUID(AlienHack::AlienActionComplete, "AlienHack::AlienActionComplete")
BOOST_SERIALIZATION_WORKAROUND_BASE_AND_DERIVED(AlienHack::IObserveActionComplete, AlienHack::AlienActionComplete);
BOOST_CLASS_EXPORT_GUID(AlienHack::Armour, "AlienHack::Armour")
BOOST_CLASS_EXPORT_GUID(AlienHack::Barrel, "AlienHack::Barrel")
BOOST_CLASS_EXPORT_GUID(AlienHack::Decoration, "AlienHack::Decoration")
BOOST_CLASS_EXPORT_GUID(AlienHack::Explosion, "AlienHack::Explosion")
BOOST_CLASS_EXPORT_GUID(AlienHack::Explosive, "AlienHack::Explosive")
BOOST_CLASS_EXPORT_GUID(AlienHack::Egg, "AlienHack::Egg")
BOOST_CLASS_EXPORT_GUID(AlienHack::Fire, "AlienHack::Fire")
BOOST_CLASS_EXPORT_GUID(AlienHack::Furniture, "AlienHack::Furniture")
BOOST_CLASS_EXPORT_GUID(AlienHack::AHGameObject, "AlienHack::AHGameObject")
BOOST_CLASS_EXPORT_GUID(AlienHack::LaserTrail, "AlienHack::LaserTrail")
BOOST_CLASS_EXPORT_GUID(AlienHack::Pickup, "AlienHack::Pickup")
BOOST_CLASS_EXPORT_GUID(AlienHack::Ping, "AlienHack::Ping")
BOOST_CLASS_EXPORT_GUID(AlienHack::PlayerCharacter, "AlienHack::PlayerCharacter")
BOOST_CLASS_EXPORT_GUID(AlienHack::Projectile, "AlienHack::Projectile")
BOOST_CLASS_EXPORT_GUID(AlienHack::Terminal, "AlienHack::Terminal")
BOOST_CLASS_EXPORT_GUID(AlienHack::Weapon, "AlienHack::Weapon")
BOOST_CLASS_EXPORT_GUID(AlienHack::PlayerStats, "AlienHack::PlayerStats")

BOOST_CLASS_EXPORT_GUID(AlienHack::OverWorld, "AlienHack::OverWorld")

BOOST_CLASS_EXPORT_GUID(AlienHack::IExecuteImpactAction, "AlienHack::IExecuteImpactAction");
BOOST_CLASS_EXPORT_GUID(AlienHack::GrenadeExplode, "AlienHack::GrenadeExplode");
BOOST_SERIALIZATION_WORKAROUND_BASE_AND_DERIVED(AlienHack::IExecuteImpactAction, AlienHack::GrenadeExplode);
BOOST_CLASS_EXPORT_GUID(AlienHack::DoWeaponDamage, "AlienHack::DoWeaponDamage");
BOOST_SERIALIZATION_WORKAROUND_BASE_AND_DERIVED(AlienHack::IExecuteImpactAction, AlienHack::DoWeaponDamage);


#if defined(_MSC_VER)
#pragma warning( pop )
#endif


#include "World-2DTiles/Zone.hpp"
#include "World-2DTiles/World.hpp"
#include "World-2DTiles/VisionField.hpp"
#include "WorldObjects/WorldObject.hpp"
#include "WorldObjects/Actor.hpp"
#include "ActionEngine/ActionEngine.hpp"




#endif
