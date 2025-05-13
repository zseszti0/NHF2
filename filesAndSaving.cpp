#include "filesAndSaving.h"

#include "combat.h"

std::vector<Character*> LoadCharactersFromFile(const char* filename) {
    std::ifstream file(filename);

    std::vector<Character*> characters;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream filestream(line);
        std::string name, rgbStr;
        std::vector<double> stats(11);
        bool b1, b2, b3;

        filestream >> name >> rgbStr;

        SDL_Color color;
        sscanf(rgbStr.c_str(), "%hhu,%hhu,%hhu", &color.r, &color.g, &color.b);
        color.a = 255; // default alpha

        for (double& stat : stats) filestream >> stat;
        filestream >> std::boolalpha >> b1 >> b2 >> b3;

        std::cout << "creating : " << name << std:: endl;
        characters.push_back(new Character(name.c_str(),stats, color, b1, b2, b3));
    }

    return characters;
}

Enemy* LoadEnemyFromFile(const char *filename) {
    std::ifstream file(filename);

    std::string line;

    std::getline(file, line);
    std::istringstream filestream(line);
    std::string name;
    std::vector<double> stats(8);
    bool b;

    filestream >> name;


    for (double& stat : stats) filestream >> stat;
    filestream >> std::boolalpha >> b;

    std::cout << "creating : " << name << std:: endl;

    return new Enemy(name.c_str(),stats,b);
}

Game* LoadProfileFromFile(const char* filename) {
    std::ifstream file(filename);

    std::string line;

    std::getline(file, line);
    std::istringstream filestream(line);
    std::string name;
    std::vector<int> stats(3);
    std::vector<bool> tuts(4);

    filestream >> name;


    for (int& stat : stats) filestream >> stat;
    for (auto && tut : tuts) {
        bool b;
        filestream >> std::boolalpha >> b;
        std::cout << "tut : " << b << std:: endl;
        tut = b;
    }


    std::cout << "creating : " << name << std:: endl;


    return new Game(name.c_str(),stats,tuts);
}

void UpdateProfileTutorial(const char* tutorialName) {
    const char* profileFileName = "./savFiles/profile.txt";
    std::ifstream inFile(profileFileName);

    // Read the existing profile data
    if (!inFile) {
        std::cerr << "Error: Unable to open profile.txt for reading.\n";
        return;
    }

    std::string name;
    int stat1, stat2, stat3;
    bool tutorials[4];

    inFile >> name >> stat1 >> stat2 >> stat3;
    for (int i = 0; i < 4; ++i) {
        inFile >> std::boolalpha >> tutorials[i];
    }
    inFile.close();

    // Update the specific tutorial as "false"
    if (std::string(tutorialName) == "MMTutorial") {
        tutorials[0] = true;
    } else if (std::string(tutorialName) == "COTutorial") {
        tutorials[1] = true;
    } else if (std::string(tutorialName) == "ADTutorial") {
        tutorials[2] = true;
    } else if (std::string(tutorialName) == "PFTutorial") {
        tutorials[3] = true;
    } else {
        std::cerr << "Error: Unknown tutorial name '" << tutorialName << "'.\n";
        return;
    }

    // Write the updated profile data back to file
    std::ofstream outFile(profileFileName);
    if (!outFile) {
        std::cerr << "Error: Unable to open profile.txt for writing.\n";
        return;
    }

    outFile << name << " " << stat1 << " " << stat2 << " " << stat3 << " ";
    for (int i = 0; i < 4; ++i) {
        outFile << std::boolalpha << tutorials[i] << (i < 3 ? " " : "");
    }
    outFile.close();

    std::cout << "Successfully updated tutorial '" << tutorialName << "' to false.\n";
}
void UpdateProfileData(Game* profile) {
    // 1. Update profile.txt (materials)
    {
        const char* profileFileName = "./savFiles/profile.txt";
        std::ifstream inFile(profileFileName);
        
        if (!inFile) {
            std::cerr << "Error: Unable to open profile.txt for reading.\n";
            return;
        }

        // Read existing data to preserve name and tutorial states
        std::string name;
        int dummy1, dummy2, dummy3;  // We'll replace these with new values
        bool tutorials[4];

        inFile >> name >> dummy1 >> dummy2 >> dummy3;
        for (int i = 0; i < 4; ++i) {
            inFile >> std::boolalpha >> tutorials[i];
        }
        inFile.close();

        // Write updated data back
        std::ofstream outFile(profileFileName);
        if (!outFile) {
            std::cerr << "Error: Unable to open profile.txt for writing.\n";
            return;
        }

        outFile << name << " "
                << profile->GetMats().tier1 << " "
                << profile->GetMats().tier2 << " "
                << profile->GetMats().tier3 << " ";
        
        for (int i = 0; i < 4; ++i) {
            outFile << std::boolalpha << tutorials[i] << (i < 3 ? " " : "");
        }
        outFile.close();
    }

    // 2. Update characters.txt
    {
        const char* charactersFileName = "./savFiles/characters.txt";
        std::ifstream inFile(charactersFileName);
        
        if (!inFile) {
            std::cerr << "Error: Unable to open characters.txt for reading.\n";
            return;
        }

        // Read all lines and update stats for each character
        std::vector<std::string> updatedLines;
        std::string line;
        size_t characterIndex = 0;

        while (std::getline(inFile, line)) {
            if (characterIndex < Game::characters.size()) {
                Character* character = Game::characters[characterIndex];
                
                std::istringstream iss(line);
                std::string name, rgbStr;
                std::vector<double> stats(11);
                bool b1, b2, b3;

                // Read the original line
                iss >> name >> rgbStr;
                for (double& stat : stats) iss >> stat;
                iss >> std::boolalpha >> b1 >> b2 >> b3;

                // Create updated line with new stats
                std::ostringstream updatedLine;
                updatedLine << name << " " << rgbStr << " "
                           << character->GetStat("baseatk") << " "    // Update ATK (first stat)
                           << character->GetStat("maxhp") << " ";     // Update HP (second stat)

                // Keep stats[2] through stats[8] unchanged
                for (size_t i = 2; i < 10; ++i) {
                    updatedLine << stats[i] << " ";
                }

                updatedLine << character->GetLevel() << " "          // Update level (last stat)
                           << std::boolalpha << b1 << " " << b2 << " " << b3;

                updatedLines.push_back(updatedLine.str());
                characterIndex++;
            }
        }
        inFile.close();

        // Write all updated lines back to file
        std::ofstream outFile(charactersFileName);
        if (!outFile) {
            std::cerr << "Error: Unable to open characters.txt for writing.\n";
            return;
        }

        for (size_t i = 0; i < updatedLines.size(); ++i) {
            outFile << updatedLines[i] << (i < updatedLines.size() - 1 ? "\n" : "");
        }
        outFile.close();
    }
}