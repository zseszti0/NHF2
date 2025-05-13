#include "gameEngine.h"

Scene* Game::currentScene = nullptr;
std::vector<Character*> Game::characters;

std::vector<UIElement*> Game::preFightSlots;
std::vector<UIElement*> Game::preFightConsistentUIBoundle;
std::vector<UIElement*> Game::preFightCharChoosers;

std::vector<UIElement*> Game::combatConsistentUIBoundle;
std::vector<UIElement*> Game::combatAnimProps;
std::vector<UIElement*> Game::combatCh1Cont;
std::vector<UIElement*> Game::combatCh2Cont;
std::vector<UIElement*> Game::combatCh3Cont;
std::vector<UIElement*> Game::combatNATypeChooser;
std::vector<UIElement*> Game::combatTurnOrderFIFO;

UIElement* Game::combat_target = nullptr;
void ChangeScene(Scene* newScene) {
    Game::currentScene = newScene;
    newScene->Init();
}