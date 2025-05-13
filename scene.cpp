#include "scene.h"
#include "UIAndDisplay.h"
#include "animation.h"
#include "filesAndSaving.h"
#include <cmath>
#include <memory>
#include <sstream>
#include <string>

// Static member definitions for Game and Scene
SDL_Renderer* Scene::renderer = nullptr;


///Scene
///
Scene::Scene(const char* n, const char* texturePath) {
    name = n;

    SDL_Surface* surface = IMG_Load(texturePath);
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    } else {
        bg = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    std::cout << "Scene: " << name << " created" <<std::endl;

}

void Scene::Init() {
    if (tutorialsActive) {
        if (tutorialsOverlay->GetCurrentStateIndex() == tutorialsOverlay->GetNumberOfStates()-1) {
            tutorialsOverlay->SetVisible(false);
            tutorialsOverlay->Disable(true);
            tutorialsActive = false;

            UpdateProfileTutorial(tutorialsOverlay->GetName().c_str());

            tutorialsOverlay = nullptr;

            for (auto& element : uiBoundle) {
                auto button = dynamic_cast<Button*>(element);
                if (button) {
                    button->Disable(false);
                }
            }
            return;
        }

        tutorialsOverlay->SetVisible(true);
        tutorialsOverlay->Disable(false);
        for (auto& element : uiBoundle) {
            auto button = dynamic_cast<Button*>(element);
            if (button) {
                button->Disable(true);
            }
        }
    }
}
void Scene::TutorialsMenuNext() {
    if (tutorialsOverlay->GetCurrentStateIndex() == tutorialsOverlay->GetNumberOfStates()-1) {
        tutorialsOverlay->SetVisible(false);
        tutorialsOverlay->Disable(true);
        tutorialsActive = false;

        UpdateProfileTutorial(tutorialsOverlay->GetName().c_str());

        tutorialsOverlay = nullptr;

        for (auto& element : uiBoundle) {
            auto button = dynamic_cast<Button*>(element);
            if (button) {
                button->Disable(false);
            }
        }
        return;
    }
    tutorialsOverlay->ChangeState(tutorialsOverlay->GetCurrentStateIndex()+1);
}


void Scene::LoadScene() {
    //get the renderer size for full screen background.
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    SDL_Rect destposition = {0, 0, windowWidth, windowHeight};

    SDL_RenderCopy(renderer, bg, nullptr, &destposition); // Render the background

    for(auto& element: uiBoundle) {
        element->Render();
    }
    if (tutorialsOverlay) tutorialsOverlay->Render();
}

void Scene::AddUIElement(UIElement* newElement) {
    uiBoundle.push_back(newElement);
}
void Scene::AddUIElement(std::vector<UIElement*>& newElements) {
    for(auto& element : newElements) {
        this->AddUIElement(element);
    }
}

void Scene::CheckButtonsOnClick(int mouseX, int mouseY) {
    for(auto element : uiBoundle) {
        auto button = dynamic_cast<Button*>(element);
        if (button) {
            button->HandleClick(mouseX, mouseY);
        }
    }
    if(tutorialsOverlay)
        tutorialsOverlay->HandleClick(mouseX, mouseY);
}
void Scene::CheckButtonsHover(int mouseX, int mouseY) {
    for(auto element : uiBoundle) {
        auto button = dynamic_cast<Button*>(element);
        if (button) {
            button->HandleHover(mouseX, mouseY);

        }
    }
}


void ListingScene::EQPositionTriggers(SDL_Rect area, bool horizontal) {
    EQPosUIElements(area,triggers,horizontal);
}

void ListingScene::LoadScene() {
    //get the renderer size for full screen background.
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    SDL_Rect destposition = {0, 0, windowWidth, windowHeight};

    SDL_RenderCopy(renderer, bg, nullptr, &destposition); // Render the background


    for(auto& element: triggers) {
        element->Render();
    }
    for(auto& boundle: menus) {
        for(auto& element: boundle) {
            element->Render();
        }
    }

    for(auto& element: uiBoundle) {
        element->Render();
    }

    if (tutorialsOverlay) tutorialsOverlay->Render();
}
void ListingScene::Reset() {
    currentMenu = 0;
    for(int i = 1 ; i < menus.size(); i++) {
        for(auto element : menus.at(i)) {
            element->SetVisible(false);
        }
    }
    for(auto element : menus.at(0)) {
        element->SetVisible(true);
    }
    this->ChagneMenu(0);
}

void ListingScene::CheckButtonsOnClick(int mouseX, int mouseY) {
    for(auto element : uiBoundle) {
        auto button = dynamic_cast<Button*>(element);
        if (button) {
            button->HandleClick(mouseX, mouseY);
        }
    }
    for(auto button : triggers) {
        button->HandleClick(mouseX, mouseY);
    }
    for(const auto& boundle : menus) {
        for(auto element: boundle) {
            auto button = dynamic_cast<Button*>(element);
            if (button) {
                button->HandleClick(mouseX, mouseY);
            }
        }
    }

    if(tutorialsOverlay)
        tutorialsOverlay->HandleClick(mouseX, mouseY);
}
void ListingScene::CheckButtonsHover(int mouseX, int mouseY) {
    for(auto element : uiBoundle) {
        auto button = dynamic_cast<Button*>(element);
        if (button) {
            button->HandleHover(mouseX, mouseY);
        }
    }
    for(auto button : triggers) {
        button->HandleHover(mouseX, mouseY);
    }
    for(const auto& boundle : menus) {
        for(auto element: boundle) {
            auto button = dynamic_cast<Button*>(element);
            if (button) {
                button->HandleHover(mouseX, mouseY);
            }
        }
    }
}

void CharOverview::LoadScene() {
    //get the renderer size for full screen background.
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    SDL_Rect destposition = {0, 0, windowWidth, windowHeight};

    SDL_RenderCopy(renderer, bg, nullptr, &destposition); // Render the background

    activeCharacter->GetSprite()->Render();
    for(auto& element: uiBoundle) {
        element->Render();
    }

    for(auto& element: triggers) {
        element->Render();
    }
    for(auto& boundle: menus) {
        for(auto& element: boundle) {
            element->Render();
        }
    }

    for(auto& element: statTexts) {
        element->Render();
    }

    if (tutorialsOverlay) tutorialsOverlay->Render();
}
void CharOverview::Init() {
    if (tutorialsActive) {
        if (tutorialsOverlay->GetCurrentStateIndex() == tutorialsOverlay->GetNumberOfStates()-1) {
            tutorialsOverlay->SetVisible(false);
            tutorialsOverlay->Disable(true);
            tutorialsActive = false;

            UpdateProfileTutorial(tutorialsOverlay->GetName().c_str());

            tutorialsOverlay = nullptr;

            for (auto& element : uiBoundle) {
                auto button = dynamic_cast<Button*>(element);
                if (button) {
                    button->Disable(false);
                }
            }
            return;
        }

        tutorialsOverlay->SetVisible(true);
        tutorialsOverlay->Disable(false);
        for (auto& element : uiBoundle) {
            auto button = dynamic_cast<Button*>(element);
            if (button) {
                button->Disable(true);
            }
        }
    }

    for(auto& ch : characters) {
        ch->GetSprite()->ChangeState("splash");

        ch->GetSprite()->GetTransform().position = SDL_Rect{603,143,850,850};
        ch->GetSprite()->GetTransform().scrRect = SDL_Rect{250,250,2500,2500};
        ch->GetSprite()->SetVisible(false);
    }
    this->UpdateMatsText();
    this->ChagneMenu(0);
}
void CharOverview::ChagneMenu(int i) {
    for(auto element : menus.at(currentMenu)) {
        element->SetVisible(false);
    }
    currentMenu = i;
    for(auto element : menus.at(currentMenu)) {
        element->SetVisible(true);
    }

    activeCharacter->GetSprite()->SetVisible(false);
    activeCharacter = characters.at(i);
    activeCharacter->GetSprite()->SetVisible(true);

    statTexts.at(0)->ChangeText(activeCharacter->GetName().c_str());
    for(auto it = statTexts.begin() + 1; it != statTexts.end(); it++) {

        double value = (activeCharacter->GetStat((*it)->GetName().c_str()));
        std::ostringstream out;
        out.precision(10); // high precision to preserve real data
        out << std::fixed << value;
        std::string result = out.str();

        // Remove trailing zeros
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);

        // If there's a trailing dot, remove it too
        if (result.back() == '.') {
            result.pop_back();
        }

        std::string text = (*it)->GetName() + ": " + result;
        (*it)->ChangeText(text.c_str());
    }
    this->UpdateLevelText();
}
void CharOverview::Level() {

    int level = activeCharacter->GetLevel();
    GameMats matsNeeded;
    if (1 <= level && level < 5) {
        matsNeeded.tier1 = 1;
        matsNeeded.tier2 = 5;
        matsNeeded.tier3 = 10 + (level+1)*5;
    }
    else if (5 <= level && level < 7) {
        matsNeeded.tier1 = 5;
        matsNeeded.tier2 = 10 + (level-1)*2;
        matsNeeded.tier3 = 10 + (level+1)*5;
    }
    else if (7 <= level && level < 10) {
        matsNeeded.tier1 = 10 + (level-1)*1;
        matsNeeded.tier2 = 10 + (level-1)*5;
        matsNeeded.tier3 = 10;
    }
    bool canUpgrade = matsNeeded.tier1 <= mats.tier1 && matsNeeded.tier2 <= mats.tier2 && matsNeeded.tier3 <= mats.tier3;
    if (canUpgrade) {
        activeCharacter->LevelUp();
        mats.tier1 -= matsNeeded.tier1;
        mats.tier2 -= matsNeeded.tier2;
        mats.tier3 -= matsNeeded.tier3;
        this->UpdateMatsText();
        ChagneMenu(currentMenu);
    }
}

void CharOverview::UpdateMatsText() {
    for(auto& element : uiBoundle) {
        if (element->GetName() == "matTier1Text") {
            auto label = dynamic_cast<Text*>(element);
            label->ChangeText(std::to_string(mats.tier1).c_str());
        }
        if (element->GetName() == "matTier2Text") {
            auto label = dynamic_cast<Text*>(element);
            label->ChangeText(std::to_string(mats.tier2).c_str());
        }
        if (element->GetName() == "matTier3Text") {
            auto label = dynamic_cast<Text*>(element);
            label->ChangeText(std::to_string(mats.tier3).c_str());
        }
    }
}
void CharOverview::UpdateLevelText() {
    for(auto& element : uiBoundle) {
        if (element->GetName() == "level") {
            auto label = dynamic_cast<Text*>(element);
            std::string text = "Lv. " + std::to_string(activeCharacter->GetLevel());
            label->ChangeText(text.c_str());
        }
    }
}


void PreFight::CheckButtonsOnClick(int mouseX, int mouseY) {
    for(auto element : uiBoundle) {
        auto button = dynamic_cast<Button*>(element);
        if (button) {
            if (button->HandleClick(mouseX, mouseY) && button->GetName() == "startCombat") {
                this->StartFight();
            }
        }
    }

    for (int i = 0; i < Game::characters.size(); i++) {
        auto button = dynamic_cast<Button*>(Game::preFightCharChoosers.at(i+1));
        if (button) {
            if (button->HandleClick(mouseX, mouseY)) {
                bool didSmth = this->AddCharacter(Game::characters.at(i));
                if(Game::preFightCharChoosers.at(i+1)->GetTransform().scale == 1.0f){Game::preFightCharChoosers.at(i+1)->GetTransform().scale = 0.8f;}
                else if(didSmth){Game::preFightCharChoosers.at(i+1)->GetTransform().scale = 1.0f;}
            }
        }
    }

    if(tutorialsOverlay)
        if (tutorialsOverlay->HandleClick(mouseX, mouseY)) {
            this->TutorialsMenuNext();
        }
}
bool PreFight::AddCharacter(Character* newCharacter) {
    int i = 0 ;
    for(auto iter = characters.begin(); iter != characters.end();) {
        if((*iter)->GetName() == newCharacter->GetName()) {
            characters.erase(iter);
            alreadyChosen--;

            for(size_t j =0; j < 3; j++) {
                charSlotCont.at(3+j)->SetVisible(false);
                charSlotCont.at(i)->Tint(SDL_Color{0,0,0});
            }

            for(i = 0; i<characters.size(); i++) {
                charSlotCont.at(3+i)->SetVisible(true);
                charSlotCont.at(3+i)->SetTexture(characters.at(i)->GetSprite()->GetTextureAt("base"));
                charSlotCont.at(i)->Tint(characters.at(i)->color);
            }
            return true;
        }
        else {
            ++iter;
            ++i;
        }
    }
    if(alreadyChosen < 3) {
        characters.push_back(newCharacter);

        charSlotCont.at(3+alreadyChosen)->SetTexture(newCharacter->GetSprite()->GetTextureAt("base"));
        charSlotCont.at(3+alreadyChosen)->SetVisible(true);
        charSlotCont.at(alreadyChosen)->Tint(newCharacter->color);
        alreadyChosen++;
        return true;
    }
    return false;
}
void PreFight::LoadEnemies(Enemy* e) {
    enemies.push_back(e);
}
void PreFight::LoadEnemies(std::vector<Enemy*> es) {
    for (auto& enemy :es) {
        this->LoadEnemies(enemy);
    }
}

void PreFight::LoadScene() {
    //get the renderer size for full screen background.
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    SDL_Rect destposition = {0, 0, windowWidth, windowHeight};

    SDL_RenderCopy(renderer, bg, nullptr, &destposition); // Render the background

    for(size_t i = 0; i < 3;i++) {
        charSlotCont.at(i)->Render();
        charSlotCont.at(i+3)->Render();
    }
    for(auto& element: uiBoundle) {
        element->Render();
    }

    if (tutorialsOverlay) tutorialsOverlay->Render();
}

void PreFight::Reset() {
    for(size_t j =0; j < 3; j++) {
        charSlotCont.at(3+j)->SetVisible(false);
        charSlotCont.at(j)->Tint(SDL_Color{0,0,0});
    }
    characters.clear();
    alreadyChosen = 0;

    for (auto& trigger : Game::preFightCharChoosers) {
        trigger->GetTransform().scale = 0.8f;
    }
}

void PreFight::StartFight() {
    if(characters.size() == 3) {
        std::cout <<"prepar to start fight " << std::endl;
        delete combat;
        combat = new Fight("combatTest","./assets/backgrounds/combatTest.png",characters);
        //for testing

        for(auto& enemy: enemies) {
            combat->AddEnemy(enemy);
        }

        combat->AddUIElement(Game::combatConsistentUIBoundle);
        combat->AddUIElement(Game::combatAnimProps);
        combat->AddUIConts(Game::combatCh1Cont,Game::combatCh2Cont,Game::combatCh3Cont,Game::combatNATypeChooser,Game::combat_target,Game::combatTurnOrderFIFO);
        combat->AddAnimConts(Game::combatAnimProps);

        ChangeScene(combat);
    }
}

void AdventuresMenu::CheckButtonsOnClick(int mouseX, int mouseY) {
    for(auto& element : nodes) {
        element->HandleClick(mouseX, mouseY);
    }
    for(auto element : uiBoundle) {
        auto button = dynamic_cast<Button*>(element);
        if (button) {
            button->HandleClick(mouseX, mouseY);
        }
    }

    if(tutorialsOverlay)
        tutorialsOverlay->HandleClick(mouseX, mouseY);
}
void AdventuresMenu::CheckButtonsHover(int mouseX, int mouseY) {
    for(auto& element : nodes) {
        element->HandleHover(mouseX, mouseY);
    }
    for(auto element : uiBoundle) {
        auto button = dynamic_cast<Button*>(element);
        if (button) {
            button->HandleHover(mouseX, mouseY);
        }
    }
}
void AdventuresMenu::LoadScene() {
    //get the renderer size for full screen background.
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    SDL_Rect destposition = {0, 0, windowWidth, windowHeight};

    SDL_RenderCopy(renderer, bg, nullptr, &destposition); // Render the background

    for(auto& element: uiBoundle) {
        element->Render();
    }
    for(auto& element: nodes) {
        element->Render();
    }

    if (tutorialsOverlay) tutorialsOverlay->Render();
}
void AdventuresMenu::StartAdventure(int n) {

}
void AdventuresMenu::InitPreFights() {
    for (auto& node : nodes) {
        node->CreatePreFight(preFightTutorials);
    }
}

AdventuresMenu::~AdventuresMenu(){
    SDL_DestroyTexture(bg);
    for (auto it = nodes.begin(); it != nodes.end(); ) {
        delete *it;             // free the memory
        it = nodes.erase(it);    // remove from vector safely
    }
}
void AdventuresMenu::PositionNodes() {
    std::vector<UIElement*> nodesSprite;
    for(auto& node : nodes) {
        nodesSprite.push_back(node->GetIcon());
    }
    EQPosUIElements(SDL_Rect{200,287,1500,1000},nodesSprite,true);
    for (int i = 0; i < nodes.size(); i++) {
        if (i % 2 != 0) {
        nodes.at(i)->GetIcon()->GetTransform().position.y += 515;
        }
        else {
            nodes.at(i)->GetIcon()->GetTransform().position.y += 250;
        }
    }
}
