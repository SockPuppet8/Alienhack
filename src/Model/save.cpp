#include "save.hpp"
#include "../Model/Serialization.hpp"
#include "version.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>



namespace AlienHack
{




const char * getSaveFileName(AHGameModel& model)
{
	if (AHGameModel::Quick == model.gameType())
		return "quick.txt";
	return "save.txt";
}


bool isSaveFile(const char * filename)
{
	if (boost::filesystem::exists(filename))
	{

		std::ifstream ifs(filename);

		if (ifs.is_open())
		{
			std::string firstline;
			std::getline(ifs, firstline);
			if (0 == firstline.compare(0, 18, "AlienHack savefile")) {

				int ver_maj, ver_min, ver_incr;
				if (std::sscanf(firstline.c_str(), "AlienHack savefile %d %d %d", &ver_maj, &ver_min, &ver_incr) != 3)
					return false;

				if (ver_maj < GAMEMODEL_SAVE_LAST_COMPATIBLE_MAJOR)
					return false;

				if (ver_maj == GAMEMODEL_SAVE_LAST_COMPATIBLE_MAJOR)
				{
					if (ver_min < GAMEMODEL_SAVE_LAST_COMPATIBLE_MINOR)
						return false;

					if (ver_min == GAMEMODEL_SAVE_LAST_COMPATIBLE_MINOR)
						if (ver_incr < GAMEMODEL_SAVE_LAST_COMPATIBLE_INCREMENTAL)
							return false;
				}

				return true;
			}
		}
	}

	return false;
}


bool saveGame(AHGameModel& model, const char * filename)
{
	std::ofstream ofs(filename);

	if (ofs.is_open())
	{
		ofs << "AlienHack savefile " << 
			boost::lexical_cast<std::string>(GAMEMODEL_VERSION_MAJOR) << " " << 
			boost::lexical_cast<std::string>(GAMEMODEL_VERSION_MINOR) << " " << 
			boost::lexical_cast<std::string>(GAMEMODEL_VERSION_INCREMENTAL) << " ##\n";

		boost::archive::text_oarchive oa(ofs);

		oa << model;

		return !ofs.fail();
	}

	return false;
}

bool loadGame(AHGameModel& model, const char * filename)
{
	bool success(false);

	{
		std::ifstream ifs(filename);

		if (ifs.is_open())
		{
			std::string firstline;
			std::getline(ifs, firstline); // Skip header and version. Version should have already been checked by isSaveFile.

			boost::archive::text_iarchive ia(ifs);

			ia >> model;

			success = !ifs.fail();
		}
	}

	if (success)
	{
		try
		{
			boost::filesystem::remove( filename );
		}
		catch( ... )
		{
		}

		return true;
	}

	return false;
}


}
