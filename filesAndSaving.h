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
