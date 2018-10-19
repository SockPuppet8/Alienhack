#ifndef ALIENHACK_SAVE_HPP
#define	ALIENHACK_SAVE_HPP



namespace AlienHack
{


class AHGameModel;

const char * getSaveFileName(AHGameModel& model);
bool isSaveFile(const char * filename);
bool saveGame(AHGameModel& model, const char * filename);
bool loadGame(AHGameModel& model, const char * filename);


}


#endif
