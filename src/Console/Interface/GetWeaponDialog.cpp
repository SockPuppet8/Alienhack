#include "GetWeaponDialog.hpp"
#include "../../Model/Objects/Weapon.hpp"
#include "Input/IFunctionMap.hpp"
#include <xutility>



namespace AlienHack
{


using namespace RL_shared;
using namespace boost;




GetWeaponDialog::GetWeaponDialog( 
	boost::weak_ptr< RL_shared::InterfaceStateMachine > ism, 
	boost::shared_ptr< Weapon > weapon, 
	boost::shared_ptr< RL_shared::IFunctionMap > keys, 
	boost::shared_ptr< IChoiceDialogSelect > action, 
	ColourScheme scheme
	)
: ChoiceDialog( ism, keys, "", 25, 19, 55, 21, scheme, "Take ammo", "Take weapon", action, 1 )
{
}


GetWeaponDialog::CommandResult GetWeaponDialog::interpretInput( const AUserInputItem& input, AGameModel& in_model )
{
	if (!m_done)
	{
        if (m_key_map->isFunction(input, "Back"))
        {
            m_done = true;
        	return CommandResult( false, true );
        }
	}

	return ChoiceDialog::interpretInput(input, in_model);
}


}
