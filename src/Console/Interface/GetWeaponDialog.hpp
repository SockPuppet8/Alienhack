#ifndef ALIENHACK_GET_WEAPON_DIALOG_HPP
#define	ALIENHACK_GET_WEAPON_DIALOG_HPP


#include "ChoiceDialog.hpp"
#include "Interface/InterfaceState.hpp"
#include <string>



namespace AlienHack
{


class Weapon;


class GetWeaponDialog : public ChoiceDialog
{
public:

	enum Action
	{
		TakeAmmo = 0, 
		TakeWeapon = 1
	};

	GetWeaponDialog( boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
		boost::shared_ptr< Weapon > weapon, 
		boost::shared_ptr< RL_shared::IFunctionMap > keys, 
		boost::shared_ptr< IChoiceDialogSelect > select, 
		ColourScheme scheme );

    virtual CommandResult interpretInput( const RL_shared::AUserInputItem&, RL_shared::AGameModel& );

};



}



#endif
