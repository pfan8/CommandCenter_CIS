#include "Common.h"
#include "CCBot.h"
#include "JSONTools.h"
#include "Util.h"

#include "sc2utils/sc2_manage_process.h"
#include "sc2api/sc2_api.h"

#include <fstream>

int main(int argc, char* argv[]) 
{
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) 
    {
        std::cout << "Unable to find or parse settings." << std::endl;
        return 1;
    }
    
    std::string config = JSONTools::ReadFile("BotConfig.txt");
    if (config.length() == 0)
    {
        std::cerr << "Config file could not be found, and is required for starting the bot\n";



        std::cerr << "Please read the instructions and try again\n";
        exit(-1);
    }

    std::ifstream file("BotConfig.txt");
    json j;
    file >> j;

    /*if (parsingFailed)
    {
        std::cerr << "Config file could not be parsed, and is required for starting the bot\n";
        std::cerr << "Please read the instructions and try again\n";
        exit(-1);
    }*/

    std::string botRaceString;
    std::string enemyRaceString;
    std::string mapString;
    int stepSize = 1;
    sc2::Difficulty enemyDifficulty = sc2::Difficulty::Easy;

    if (j.count("SC2API") && j["SC2API"].is_object())
    {
        const json & info = j["SC2API"];
        JSONTools::ReadString("BotRace", info, botRaceString);
        JSONTools::ReadString("EnemyRace", info, enemyRaceString);
        JSONTools::ReadString("MapFile", info, mapString);
        JSONTools::ReadInt("StepSize", info, stepSize);
        JSONTools::ReadInt("EnemyDifficulty", info, enemyDifficulty);
    }
    else
    {
        std::cerr << "Config file has no 'Game Info' object, required for starting the bot\n";
        std::cerr << "Please read the instructions and try again\n";
        exit(-1);
    }

    // Add the custom bot, it will control the players.
    CCBot bot;

    
    // WARNING: Bot logic has not been thorougly tested on step sizes > 1
    //          Setting this = N means the bot's onFrame gets called once every N frames
    //          The bot may crash or do unexpected things if its logic is not called every frame
    coordinator.SetStepSize(stepSize);
    coordinator.SetRealtime(false);

    coordinator.SetParticipants({
		sc2::CreateParticipant(Util::GetRaceFromString(botRaceString), &bot),
		sc2::CreateComputer(Util::GetRaceFromString(enemyRaceString), enemyDifficulty)
	});

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(mapString);

	auto info = bot.Observation()->GetGameInfo();
	/*std::ofstream save_file("buildable_info.txt");
	int true_map_width_ = info.width;
	int true_map_height_ = info.height;
	save_file << "true_map_width:" << true_map_width_ << std::endl;
	save_file << "true_map_height:" << true_map_height_ << std::endl;


	int playable_map_width_ = static_cast<int>(info.playable_max.x - bot.Observation()->GetGameInfo().playable_min.x);
	int playable_map_height_ = static_cast<int>(info.playable_max.y - bot.Observation()->GetGameInfo().playable_min.y);
	save_file << "playable_map_width:" << playable_map_width_ << std::endl;
	save_file << "playable_map_height:" << playable_map_height_ << std::endl;

	for (int i = 0; i < true_map_width_; ++i)
	{
		for (int j = 0; j < true_map_height_; ++j)
		{
			const unsigned char encoded_placement = info.placement_grid.data[i + ((info.height - 1) - j) * info.width];
			save_file << (encoded_placement == 255) ? 1 : 0;
		}
		save_file << "\n";
	}
	save_file.close();*/


    // Step forward the game simulation.
    while (true) 
    {
        coordinator.Update();
    }

    return 0;
}