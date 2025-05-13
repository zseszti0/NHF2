#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <vector>

struct GameMats {
    int tier1 = 0;
    int tier2 = 0;
    int tier3 = 0;
};
#include "scene.h"
#include "UIAndDisplay.h"

class Game;


class Game {
    std::string playerName;
    GameMats mats;

    bool MMTutorial = false;
    bool COTutorial = false;
    bool ADTutorial = false;
    bool PFTutorial = false;

public:
    static Scene* currentScene;
    static std::vector<Character*> characters;

    static std::vector<UIElement*> preFightSlots;
    static std::vector<UIElement*> preFightConsistentUIBoundle;
    static std::vector<UIElement*> preFightCharChoosers;

    static std::vector<UIElement*> combatConsistentUIBoundle;
    static std::vector<UIElement*> combatAnimProps,combatCh1Cont,combatCh2Cont,combatCh3Cont,combatNATypeChooser,combatTurnOrderFIFO;
    static UIElement* combat_target;

    Game(const char* name, std::vector<int> materials, std::vector<bool> tutorials) {
        playerName = name;
        mats.tier1 = materials.at(0);
        mats.tier2 = materials.at(1);
        mats.tier3 = materials.at(2);

        MMTutorial = tutorials.at(0);
        COTutorial = tutorials.at(1);
        ADTutorial = tutorials.at(2);
        PFTutorial = tutorials.at(3);
    }

    bool GetTutorialNeed(const char* tutorialName) {
        std::string tutorial = tutorialName;
        if(tutorial == "MM")
            return !MMTutorial;
        else if(tutorial == "CO")
            return !COTutorial;
        else if(tutorial == "AD")
            return !ADTutorial;
        else if(tutorial == "PF")
            return !PFTutorial;
        return false;
    }
    void SetTutorialNeed(const char* tutorialName, bool value) {
        std::string tutorial = tutorialName;
        if(tutorial == "MM")
            MMTutorial = value;
        else if(tutorial == "CO")
            COTutorial = value;
        else if(tutorial == "AD")
            ADTutorial = value;
        else if(tutorial == "PF")
            PFTutorial = value;
    }

    void UpdateMats(GameMats newMats) {
        mats.tier1 += newMats.tier1;
        mats.tier2 += newMats.tier2;
        mats.tier3 += newMats.tier3;
    }
    GameMats& GetMats() {
        return mats;
    }
};

void ChangeScene(Scene* newScene);

#endif
