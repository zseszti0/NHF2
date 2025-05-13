#include "adventure.h"

#include <fstream>
#include <sstream>

#include "filesAndSaving.h"

FightNode::FightNode(const char* source){
    std::ifstream file(source);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open fight node file: " + std::string(source));
    }

    std::string firstLine;
    std::getline(file, firstLine);

    std::istringstream lineStream(firstLine);

    attempted = false;
    stars = 0,
    lineStream >> name >> stars >> std::boolalpha >> attempted;
    

    // Load the monsters into a vector of strings
    std::string line;
    std::string monster;
    int levelNeeded;
    while (std::getline(file, line)) {
        std::istringstream mobStream(line);
        mobStream >> monster >> levelNeeded;

        std::cout <<"LOADGIN ENEMIES-----------" << monster << std::endl;
        std::string path = "./assets/enemiesData/" + monster + ".txt";
        enemies.push_back(LoadEnemyFromFile(path.c_str()));
        path = "./assets/sprites/" + monster + "_turnThumb.png";
        enemies.back()->GetSprite()->AddState("turnThumb",path.c_str());
        for (int i = 1; i < levelNeeded; ++i) {
            enemies.back()->LevelUp();
        }
    }
    
        
    icon = new Sprite("0","./assets/UI/FightNode0.png",SDL_Rect{0,0,286,273});
    icon->AddState("1", "./assets/UI/FightNode1.png");
    icon->AddState("2", "./assets/UI/FightNode2.png");
    icon->AddState("3", "./assets/UI/FightNode3.png");
    icon->ChangeState(stars);
    if (!attempted) icon->Tint(SDL_Color{65, 65, 65});
}
void FightNode::CreatePreFight(SpriteButton* tutOverlay) {
    preFight = new PreFight(name.c_str(), "./assets/backgrounds/black.png");
    std::cout << "1" << std::endl;

    preFight->AddCharCont(Game::preFightSlots);
    std::cout << "2.0" << std::endl <<std::flush;
    preFight->AddUIElement(Game::preFightConsistentUIBoundle);
    std::cout << "2.1" << std::endl <<std::flush;
    preFight->AddUIElement(Game::preFightCharChoosers);
    std::cout << "2.2" << std::endl <<std::flush;
    std::cout << "3" << std::endl;
    preFight->LoadEnemies(enemies);

    if (tutOverlay) {
        std::cout << "!!!!!!!tut overlay" << std::endl;
        preFight->AddTutorialsMenu(tutOverlay);
    }
}


void FightNode::Start() {
    preFight->Reset();
    ChangeScene(preFight);
}

FightNode::~FightNode() {
    delete preFight;
    delete icon;
    for (auto it = enemies.begin(); it != enemies.end(); ) {
        delete *it;             // free the memory
        it = enemies.erase(it);    // remove from vector safely
    }
}