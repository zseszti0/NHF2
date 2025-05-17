/*
Overarching Design Considerations
•	Game is the central orchestrator of your entire system. It’s a singleton-like wrapper that:
o	Initializes key game data (like player info and progression),
o	Controls scene transitions (ChangeScene),
o	Manages shared state across all systems (like materials, tutorial flags, etc.),
•	Serves as a static access point for common UI containers used in multiple scenes.
•	It's intentionally global in scope — not in the C++ static sense, but in design — because it holds game-wide truth: what the player owns, where they are, and what they've already seen. While this may seem “weird” or “everything-at-once,” it's actually quite fitting for a single-player, scene-driven game with persistent state and UI. A built global game context, and that’s exactly what this is.
*/
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

/*
Purpose: Acts as a central runtime context for the game. Responsible for:
o	Holding persistent player state (name, materials),
o	Handling scene transitions (ChangeScene()),
o	Tracking one-time tutorial progress,
o	Storing static UI containers used across multiple scenes.

Helper Structs/Enums:
struct GameMats {
int tier1 = 0;
int tier2 = 0;
int tier3 = 0;
};

Key Members:
•	static Scene¬* currentScene;
•	static preset ui boundles for various scenes.
•	bool MM/CP/AD/PF Tutorials - Four booleans representing whether the player has completed one-time tutorials for specific scenes. Loaded from and saved to file.
•	GameMats mats;

Core Functions:
•	void Set/GetGameMats ()
Enables acesseing the players materials for e.g. leveling up characters.
•	void Set/GetTutorialNeed()
Acessing one-time tutorial need, and adjusting it when done.
*/
class Game {
    std::string playerName;
    GameMats mats;

    /*
    One-Time Tutorial System
Purpose: A very thoughtful and player-friendly feature.
    Overview:
•	Every scene (Scene, Fight, CharacterOverview, etc.) has:
o	A tutorial overlay (Sprite*),
o	A bool flag indicating whether the tutorial should be shown.
•	On first entry, the scene checks with Game if the tutorial for that scene has already been completed:
o	If not, it activates the overlay and disables other input.
o	Once the player interacts with the tutorial (e.g., dismisses it), the scene sets a local tutorialsActive= true, then saves it to local files.
•	This is remembered using the save file — only ever played once, unless the player manually resets progress.
Design Considerations:
•	Scene behavior is adjusted while tutorials are active — they "lock" other buttons/interactions to prevent confusion.
•	A player-first, UX-minded approach. Tutorial-only input mode is smart and avoids overwhelming players.

    */
    bool MMTutorial = false;
    bool COTutorial = false;
    bool ADTutorial = false;
    bool PFTutorial = false;

public:
    /*
    Overarching Design Considerations:
•	Scene-Driven, Global State Management: Game is the glue — enabling clean per-scene design while still offering persistent state access across everything.
•	Minimal and Focused: Despite its central role, it avoids bloating by only holding essentials: scene control, player identity, game materials, and tutorial state.
•	Expandable: Easy to add more static containers or flags in the future (e.g., daily rewards, settings).
•	Safe Static Usage: Since everything is static, it’s globally accessible — but controlled through encapsulated interfaces like InitGame() and ChangeScene().

    */
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
