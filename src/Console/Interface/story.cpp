#include "story.hpp"


namespace AlienHack
{

namespace story_text
{

	const std::string background_text_1 = 
		 ////////////////////////////////////////////////////////////////
		"Space is a vast and desolate place. Even with faster-than-light \n"
		"travel, help can be months or years away.\n\n"
		"For that reason, the ICC takes it seriously when communication \n"
		"with a colony or outpost is lost; seriously enough to send a \n"
		"Recon and Rescue Mission. A Recon and Rescue Mission is a full \n"
		"military unit, prepared for anything from a small civil war to a\n"
		"natural or industrial disaster.\n\n"
		"Or at least, so goes the theory. Normally, Recon and Rescue \n"
		"Missions find nothing but a downed transmitter and boredom when \n"
		"they reach their far-flung destinations.\n\n"
		"That's how you come to find yourself aboard the ICCS Aquila, \n"
		"travelling to the Yakunoco outpost on SV-171, a barren planetoid\n"
		"in an unremarkable yellow dwarf system. Like most onboard, you \n"
		"aren't really expecting to awake from travel hibernation to \n"
		"anything more than a few weeks shore-leave in the ass end of \n"
		"nowhere, followed by a trip back to collect your paycheck... but\n"
		"this time, you are in for a rude awakening."
		;

	const std::string background_text_2 = 
		 ////////////////////////////////////////////////////////////////
		"On approach into SV-171 orbit, something hits the Aquila. You \n"
		"don't know what it was; the damage reporting systems think it \n"
		"was an impact with a large swarm of stellar debris, a freakish \n"
		"billion-to-one piece of bad luck that no ship could survive. \n"
		"Damage to the Aquila is severe. Emergency systems activate, and \n"
		"your cryo-pod - with you still soundly asleep inside - is swiftly\n"
		"loaded into an EEV and jettisoned into space.\n\n"
		"When you awake, you are on the bleak rocky surface of SV-171, \n"
		"where your EEV has made a rough landing. For whatever reason - \n"
		"damage to critical systems, maybe - your cryo-pod was the only \n"
		"one loaded into the vessel. There is no signal from the Aquila, \n"
		"from any other EEVs, or from the colony itself, which is most of \n"
		"a day's travel away. You're on your own.\n"
		;

	const std::string background_text_3 = 
		 ////////////////////////////////////////////////////////////////
		"When you arrive at the colony complex, dusk is falling on SV-171.\n" 
		"The stars are hard, icy points of light shining down through the \n"
		"planetoid's thin but breathable atmosphere. \n\n"
		"As the buildings come into view, your hopes of rescue and \n"
		"respite take a sharp dive. The buildings are dark and silent; \n"
		"there is no sign of activity or life. \n\n"
		"Uninviting though it is, you know that your only hope of survival\n"
		"and escape lies within those walls. After a quick and fruitless \n"
		"scout around the perimeter, you approach the nearest entrance, \n"
		"as a vicious-looking storm rolls in with alarming speed over the\n"
		"desolate rocky plain. \n\n"
		"The doors still have power, at least; they open readily. You \n"
		"prepare your weapon and step into the forbidding gloom beyond, \n"
		"wondering what happened here, and what might be waiting for you..."
		;

	const std::string ending_text_1 = 
		 ////////////////////////////////////////////////////////////////
		"As the entrance doors open, far too slowly, you feel sure that\n"
		"something is going to jump on you from behind. But nothing does,\n" 
		"and you race through the open portal. Cold darkness on a bleak\n"
		"rockball of a planet has never seemed so welcoming, but a voice\n" 
		"inside keeps wondering if they will come after you.\n\n"
		"If they are, there is no sign of it yet, though you keep looking\n" 
		"behind you. Nothing interrupts your flight as you leave the\n" 
		"dead colony of ghosts and terror behind you.\n\n"
		;

	const std::string ending_text_bad = 
		 ////////////////////////////////////////////////////////////////
		"But you have left it too late. The colony is still in sight when\n" 
		"the night suddenly turns white. You feel searing heat on your\n"
		"back, and barely have time to feel cheated before you are\n"
		"annihilated.\n\n\n"
		"                          The End"
		;

	const std::string ending_text_good_1 = 
		 ////////////////////////////////////////////////////////////////
		"With the strength of desperation you stumble and scramble for\n" 
		"mile after mile, wondering how long you have. Eventually, with\n" 
		"the colony still just visible in the distance, you come to a\n" 
		"gully and think that you can't risk being caught in the open any\n" 
		"longer.\n\n" 
		"Just in time; no sooner have you got into the deepest crevice,\n" 
		"behind the biggest rock you can find, than the night above you\n" 
		"is lit white. You curl up and start praying. A terrifying\n" 
		"thunderclap sounds, just above your head. You can't even hear\n" 
		"yourself yelling out with all the air left in your lungs. Then\n" 
		"whatever air you have left is smashed out of you as the ground\n" 
		"heaves up and slaps you with an open hand made out of several\n" 
		"million tons of rock.\n\n" 
		"You must have blacked out for a bit, because the next thing you\n" 
		"know, everything is calm. Somehow, you seem to be in one piece -\n" 
		"or at least, you still have all the bits that the aliens hadn't\n" 
		"already chewed off.\n\n" 
		"Eventually, you succeed in picking yourself up. The mushroom\n" 
		"cloud is visible before you even make it out of the gully.\n" 
		"Nothing else is moving on the plain."
		;

	const std::string ending_text_good_2 = 
		 ////////////////////////////////////////////////////////////////
		"You failed to send your distress signal, but you realise that\n" 
		"there is still hope. The destruction of the colony should have\n" 
		"been picked up by monitoring satellites, and that is bound to\n" 
		"get someone's attention. But what will they make of it? One ship\n" 
		"lost, one colony utterly destroyed... Invasion? War? You hope\n" 
		"that whoever turns up next will be much better prepared.\n\n" 
		"In the meantime, however, you are stuck here. With nowhere else\n" 
		"to go, you return to your crashed escape pod, to await rescue -\n" 
		"if it's coming. With the last remaining juice in your cryo-pod,\n" 
		"maybe you can hold out long enough.\n\n"
		"Fortunately, you have in your personal equipment one item which\n" 
		"no soldier in the god-forsaken corners of the galaxy should be\n" 
		"without: a deck of cards. A few thousand rounds of solitaire\n" 
		"should make a nice change of pace from dodging slavering death\n" 
		"in the darkness...\n\n"
		"                          The End"
		;

	const std::string quick_intro_text = 
		" You know the drill. Enter the infested reactor\n" //47
		" and kill the alien queen!                     \n";

	const std::string intro_text = 
		" You have a bad feeling about these dark    \n" //44
		" and empty rooms. The bloodstains and signs \n"
		" of bizarre, severe structural damage do    \n"
		" nothing to ease your fears. What kind of a \n"
		" war happened here?                         \n"
		"\n"
		" Regardless,  your first priority is to     \n"
		" report what happened to your ship. To do   \n"
		" that  you  will  need  to  get  to  the    \n"
		" administration block,  and get access to   \n"
		" the colony's FTL transmitter.              \n"
		"\n"
		" You take out your firearm and creep into   \n"
		" the gloom...                              ";

	const std::string found_queen_text = 
		" A huge shape looms out of the blackness.    \n" //45
		"\n"
		" Instantly, you know that you have found the \n"
		" matriarch of the alien horde.               \n"
		"\n" 
		" It lets out a hissing rasp of primal fury.  \n"
		" You sense that a moment of reckoning has    \n"
		" arrived...                                  ";

	const std::string killed_queen_text = 
		" The alien queen collapses in a pile of thrashing limbs, \n" //57
		" and dies with a final bubbling screech.                 \n"
		"\n" 
		" No sooner have you caught your breath than you hear a   \n"
		" distant hissing and scrabbling sound that seems to be   \n"
		" coming from all around, and increasing in volume.       \n"
		"\n"
		" You don't like the sound of that.";

	const std::string started_countdown_text = 
		" Suddenly the lights go out.                     \n" //49
		"\n"
		" A moment later, dim emergency lighting returns. \n"
		" A recorded voice echoes over the tannoy:        \n"
		"\n"
		" \"WARNING. Runaway reactor failure is imminent.  \n"
		" All personnel must evacuate immediately and     \n"
		" proceed to minimum safe distance. Repeat: all   \n"
		" personnel must evacuate immediately. This is    \n"
		" not a drill.\"                                   \n"
		"\n"
		" You think you had better get out of here!       \n";

	const std::string used_master_terminal_text = 
		///////////////////////////////////////////////////
		" Your heart sinks as you try to access the FTL \n"
		" array and diagnostic error messages pour onto the \n"
		" screen. The array is unresponsive and without \n"
		" power. Your distress signal isn't going anywhere. \n\n"
		" What do you do now? Monitoring systems indicate a \n"
		" large and invasive foreign presence deep inside \n"
		" the colony's main reactor. Perhaps the source of \n"
		" the alien menace is there? If you could destroy \n"
		" it, maybe you would have a chance at survival in \n"
		" this terrible place until help comes. \n\n"
		" The reactor is reached from the base of the \n"
		" Engineering block, close to your current location.\n" 
		" With dread in your heart, you open the blast doors\n"
		" to the reactor access tunnel, and prepare yourself\n"
		" to go and confront the terror.\n";

}

}
