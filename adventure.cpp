#include "adventure.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "filesAndSaving.h"

FightNode::FightNode(const char* source){
    std::ifstream file(source);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open fight node file: " + std::string(source));
    }

    std::string firstLine;
    std::getline(file, firstLine);

    std::istringstream lineStream(firstLine);

    lineStream >> name;
    attempted = false;
    std::vector<bool> bools(4);
    for (auto && value : bools) {
        bool b;
        lineStream >> std::boolalpha >> b;
        std::cout << "levelstar : " << b << std:: endl;
        value = b;
    }
    attempted = bools[0];
    stars = {bools[3],bools[1],bools[2]};
    std::cout <<"----------------------LEVEL NODE STARS--: " << stars.noDeaths << " " << stars.lessThan5 << " " << stars.lessThan10 << std::endl;
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
    
        
    icon = new Sprite("000","./assets/UI/FightNode000.png",SDL_Rect{0,0,286,273});
    icon->AddState("001", "./assets/UI/FightNode001.png");
    icon->AddState("010", "./assets/UI/FightNode010.png");
    icon->AddState("110", "./assets/UI/FightNode110.png");
    icon->AddState("011", "./assets/UI/FightNode011.png");
    icon->AddState("111", "./assets/UI/FightNode111.png");

    this->UpdateSpriteIconState();
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