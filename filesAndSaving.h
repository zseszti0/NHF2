/*
Purpose: Handles file I/O for saving and loading game data.
Key Functions:
•	LoadCharactersFromFile()
•	LoadEnemyFromFile()
•	Save/load materials, progress, and tutorial state.
Key Concepts:
•	Tight integration with gameEngine.h, characters, and scene tutorials.
•	Text-based file format for ease of debugging and flexibility.
*/
#ifndef FILESANDSAVING_H
#define FILESANDSAVING_H

#include <fstream>
#include <sstream>

#include "gameEngine.h"
#include "charactersAndEnemies.h"

std::vector<Character*> LoadCharactersFromFile(const char* filename);
Enemy* LoadEnemyFromFile(const char* filename);
Game* LoadProfileFromFile(const char* filename);

void UpdateProfileTutorial(const char* tutorialName);
void UpdateProfileData(Game* profile);

#endif //FILESANDSAVING_H
