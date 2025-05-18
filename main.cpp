///Consider reading the documentation before use. Some functions and classes are documented here too
///But most logic and design philosophies are explained thoroughly there.
///:3 happy playing!!
///
///
///

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <filesystem>
#include <optional>
#include "tweeny-3.2.0.h"

//MY HEADERS
#include "UIAndDisplay.h"
#include "charactersAndEnemies.h"
#include "scene.h"
#include "filesAndSaving.h"
#include "gameEngine.h"

int main(int argc, char* argv[]) {


    // 1.) Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! Error massage: " << SDL_GetError() << "\n";
        return -1;
    }


    // 2.) Create a Fullscreen Window
    SDL_Window* window = SDL_CreateWindow(
        "ddddddddddddd",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1920, 1080,
    SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (!window) {
        std::cerr << "Window could not be created! Error massage: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }
    //SDL_MaximizeWindow(window);

    // 3.) Create Renderer

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! Error massage: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    Scene::renderer = renderer; //set it as the renderer for the visual classes
    UIElement::renderer = Scene::renderer;
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    UIElement::scaleX = static_cast<float>(windowWidth)/ 1920;
    UIElement::scaleY = static_cast<float>(windowHeight)/ 1080;

    //4.) Init ttf
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
    }

    ///INIT THE DIFFERENT GAME SCENES
    ///
    ///
    Game* player = LoadProfileFromFile("./savFiles/profile.txt");


    ///----------CHARACTERS INIT-------------------
    ///
    ///
    std::vector<Character*> characters = LoadCharactersFromFile("./savFiles/characters.txt");
    std::cout << "characters created" <<std:: endl;
    characters.at(0)->AddSkillAndUlt( {
        [](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {

std::cout << "EPHYXAME SKILLED :3" <<std::endl << std::endl;
herself->GetBuff({"atk",herself->GetStat("atk")*0.2,4});
herself->GetBuff({"critDmg",0.3,4});
herself->GetBuff({"energy",20,0});

}
    ,2,0},[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself){

        std::cout << "EPHYXAME ULTED :3" <<std::endl << std::endl;
        return 6.0f;
    });
    characters.at(1)->AddSkillAndUlt( {
        [](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {
std::cout << "VELYCTISKA SKILLED :3" <<std::endl << std::endl;

herself->GetBuff({"starAbsorbA",0.5,4});
herself->GetBuff({"energy",35,0});

herself->GetBuff({"hp",-1000,0});

for(auto& ally : allies) {
ally->GetBuff({"energy",15,0});
}
}
    ,5,0},[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {

        std::cout << "VALYTISKA ULTED :3" <<std::endl << std::endl;
        herself->GetBuff({"hp",-3000,0});

        herself->GetBuff({"atk",herself->GetStat("atk")*0.4,2});
        return 12.0f;
    });
    characters.at(2)->AddSkillAndUlt( {
        [](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {
herself->GetBuff({"starGen",4,4});
herself->GetBuff({"critDmg",1,4});

}
    ,2,0},[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {

        herself->GetBuff({"starAbsorbQ",0.5,4});
        herself->GetBuff({"critDmg",1,2});

        for(auto& enemy:targets) {
            enemy->GetBuff({"def",enemy->GetStat("basedef")*0.15,1});
        }
        return 3.5f;
    });
    characters.at(3)->AddSkillAndUlt( {
        [](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {
std::cout << "CALLEOPE SKILLED :3" <<std::endl << std::endl;

herself->GetBuff({"atk",herself->GetStat("atk")*0.3,4});
herself->GetBuff({"critDmg",0.3,4});
            for(auto& ally : allies) {
                ally->GetBuff({"ER",15,2});
            }
            herself->GetBuff({"speed",30,6});

}
    ,5,0},[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {

        std::cout << "EPHYXAME ULTED :3" <<std::endl << std::endl;
        return 5.0f;
    });
    characters.at(4)->AddSkillAndUlt({[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {
        std::cout << "BUSTER SUPP SKILLED :3" <<std::endl << std::endl;

        herself->GetBuff({"critDmg",1,2});
        herself->GetBuff({"starAbsorbB",0.5,4});
        herself->GetBuff({"speed",20,4});
    },3,0},[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {

        std::cout << "EPHYXAME ULTED :3" <<std::endl << std::endl;
        for(auto& ally : allies) {
            ally->GetBuff({"atk",ally->GetStat("atk")*0.2,2});
            ally->GetBuff({"hp",1000,4});
            ally->GetBuff({"starGen",3,4});
        }
        return 0.0f;
    });
    characters.at(5)->AddSkillAndUlt({[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {
        std::cout << "EMA SKILLED :3" <<std::endl << std::endl;

        herself->GetBuff({"atk",herself->GetStat("atk")*0.2,6});
        herself->GetBuff({"critDmg",0.2,6});
        herself->GetBuff({"starAbsorbA",herself->GetStat("starAbsorbA")*0.3,6});

        for(auto& ally : allies) {
            ally->GetBuff({"ER",ally->GetStat("ER")*0.3,4});
            ally->GetBuff({"energy",30,0});
        }
    },4,0},[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {

        std::cout << "EPHYXAME ULTED :3" <<std::endl << std::endl;
        herself->GetBuff({"atk",herself->GetStat("atk")*0.2,3});

        return 4.0f;
    });
    characters.at(6)->AddSkillAndUlt({[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {
        herself->GetBuff({"starAbsorbQ",0.8,4});
        herself->GetBuff({"energy",60,0});
        herself->GetBuff({"starGen",5,4});
    },5,0},[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {

    std::cout << "EPHYXAME ULTED :3" <<std::endl << std::endl;
        for(auto& ally : allies) {
            ally->GetBuff({"ER",ally->GetStat("ER")*0.3,4});
            ally->GetBuff({"energy",20,0});
        }
        for(auto& enemy: targets) {
            enemy->GetBuff({"def",enemy->GetStat("basedef")*0.4,1});
        }

        return 0.0f;
    });
    characters.at(7)->AddSkillAndUlt( {
        [](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {
std::cout << "HEAL1 SKILLED :3" <<std::endl << std::endl;

herself->SetHP(5000);
herself->GetBuff({"energy",10,0});

}
    ,2,0},[](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {

    std::cout << "EPHYXAME ULTED :3" <<std::endl << std::endl;
        for(auto& ally : allies) {
            ally->SetHP(2000);
            ally->GetBuff({"energy",20,0});
        }
        return 0.0f;
    });
    Game::characters = characters;

    ///----------COMBAT SCENE INIT-------------------
    ///
    //ANIMATION TEXTURES---------------------------
    UIElement combat_ch1SkillAnim("ch1SkillAnim","./assets/UI/empty.jpg",SDL_Rect{153,800,709,1417});
    combat_ch1SkillAnim.GetTransform().scale = 0.4f;
    combat_ch1SkillAnim.GetTransform().opacity = 0.0f;
    UIElement combat_ch2SkillAnim("ch2SkillAnim","./assets/UI/empty.jpg",SDL_Rect{550,800,709,1417});
    combat_ch2SkillAnim.GetTransform().scale = 0.4f;
    combat_ch2SkillAnim.GetTransform().opacity = 0.0f;
    UIElement combat_ch3SkillAnim("ch3SkillAnim","./assets/UI/empty.jpg",SDL_Rect{947,800,709,1417});
    combat_ch3SkillAnim.GetTransform().scale = 0.4f;
    combat_ch3SkillAnim.GetTransform().opacity = 0.0f;

    UIElement combat_ch1UltAnim("ch1SUltAnim","./assets/UI/empty.jpg",SDL_Rect{510,240,3000,3000});
    combat_ch1UltAnim.GetTransform().scale = 0.3f;
    combat_ch1UltAnim.GetTransform().opacity = 0.0f;
    UIElement combat_ch2UltAnim("ch2UltAnim","./assets/UI/empty.jpg",SDL_Rect{510,240,3000,3000});
    combat_ch2UltAnim.GetTransform().scale = 0.3f;
    combat_ch2UltAnim.GetTransform().opacity = 0.0f;
    UIElement combat_ch3UltAnim("ch3UltAnim","./assets/UI/empty.jpg",SDL_Rect{510,240,3000,3000});
    combat_ch3UltAnim.GetTransform().scale = 0.3f;
    combat_ch3UltAnim.GetTransform().opacity = 0.0f;
    UIElement combat_ultOverlay("ultOverlay","./assets/combat/combatUltOverlay.png",SDL_Rect{0,0,1920,1080});
    combat_ultOverlay.GetTransform().opacity = 0.0f;

    std::vector<UIElement*> combatAnimProps ={&combat_ch1SkillAnim,&combat_ch2SkillAnim,&combat_ch3SkillAnim,&combat_ch1UltAnim,&combat_ch2UltAnim,&combat_ch3UltAnim,&combat_ultOverlay};

    //CONSISTENT UI BACKGROUND
    Button combat_NA("NA","./assets/combat/NA.png",SDL_Rect{1432,829,149,134},[] {SimulateKeyPress(SDLK_q);});
    Button::AddBasicScaleUpHoverAnim(&combat_NA);
    TextButton combat_skill("skill","./assets/combat/skill.png",SDL_Rect{1579,743,127,120}, []{SimulateKeyPress(SDLK_e);},
        "./assets/fonts/combat_DMG.otf","",SDL_Color{255,255,255},70);
    Button::AddBasicScaleUpHoverAnim(&combat_skill);

    Button combat_Buster("BusterNA","./assets/combat/buster.png",SDL_Rect{1291,720,104,47}, []{SimulateKeyPress(SDLK_1);});
    Button::AddBasicScaleUpHoverAnim(&combat_Buster);
    Button combat_Arts("ArtsNA","./assets/combat/arts.png",SDL_Rect{1291,785,104,47}, []{SimulateKeyPress(SDLK_2);});
    Button::AddBasicScaleUpHoverAnim(&combat_Arts);
    Button combat_Quick("QucikNA","./assets/combat/quick.png",SDL_Rect{1291,851,104,47}, []{SimulateKeyPress(SDLK_3);});
    Button::AddBasicScaleUpHoverAnim(&combat_Quick);

    UIElement combat_baqBg("baqBg","./assets/combat/baqBg.png",SDL_Rect{1245,691,194,249});
    UIElement combat_CritChanceB("BusterCritChance","./assets/combat/critChance.png",SDL_Rect{1372,700,52,35});
    OutlinedText combat_CritChanceTextB("BusterCritChanceText","./assets/fonts/combat_DMG.otf",SDL_Rect{1387,713},"0%",SDL_Color{255,181,0},SDL_Color{0,0,0},20,2);

    UIElement combat_CritChanceA("ArtsCritChance","./assets/combat/critChance.png",SDL_Rect{1372,768,52,35});
    OutlinedText combat_CritChanceTextA("ArtsCritChanceText","./assets/fonts/combat_DMG.otf",SDL_Rect{1387,781},"0%",SDL_Color{255,181,0},SDL_Color{0,0,0},20,2);

    UIElement combat_CritChanceQ("QuickCritChance","./assets/combat/critChance.png",SDL_Rect{1372,831,52,35});
    OutlinedText combat_CritChanceTextQ("QuickCritChanceText","./assets/fonts/combat_DMG.otf",SDL_Rect{1387,844},"0%",SDL_Color{255,181,0},SDL_Color{0,0,0},20,2);

    UIElement combat_spriteCont1L1("spriteCont1Layer1","./assets/combat/SpriteLayer1.png",SDL_Rect{78,840,361,223});
    UIElement combat_spriteCont1L2("spriteCont1Layer2","./assets/combat/SpriteLayer3.png",SDL_Rect{78,840,361,223});
    UIElement combat_spriteCont2L1("spriteCont2Layer1","./assets/combat/SpriteLayer1.png",SDL_Rect{475,840,361,223});
    UIElement combat_spriteCont2L2("spriteCont2Layer2","./assets/combat/SpriteLayer3.png",SDL_Rect{475,840,361,223});
    UIElement combat_spriteCont3L1("spriteCont3Layer1","./assets/combat/SpriteLayer1.png",SDL_Rect{872,840,361,223});
                                                UIElement combat_spriteCont3L2("spriteCont3Layer2","./assets/combat/SpriteLayer3.png",SDL_Rect{872,840,361,223});
    Bar combat_ult1("ult1","./assets/combat/ultBarBg.png","./assets/combat/ultBar.png",SDL_Rect{330,913,77,68},0,nullptr,false);
    Bar combat_ult2("ult2","./assets/combat/ultBarBg.png","./assets/combat/ultBar.png",SDL_Rect{727,913,77,68},0,nullptr,false);
    Bar combat_ult3("ult3","./assets/combat/ultBarBg.png","./assets/combat/ultBar.png",SDL_Rect{1124,913,77,68},0,nullptr,false);



    UIElement combat_spBg("spBg","./assets/combat/skillPontBg.png",SDL_Rect{1588,734,286,274});
    Sprite combat_sp("sp0","./assets/combat/skillPont0.png",SDL_Rect{1588,734,286,274});
    combat_sp.AddState("sp1","./assets/combat/skillPont1.png");
    combat_sp.AddState("sp2","./assets/combat/skillPont2.png");
    combat_sp.AddState("sp3","./assets/combat/skillPont3.png");

    combat_sp.ChangeState(3);

    Bar combat_hpBar1("hp1","./assets/combat/hpBarBg.png","./assets/combat/hpBar.png",SDL_Rect{111,1004,204,27},0,nullptr);
    Bar combat_hpBar2("hp2","./assets/combat/hpBarBg.png","./assets/combat/hpBar.png",SDL_Rect{508,1004,204,27},0,nullptr);
    Bar combat_hpBar3("hp3","./assets/combat/hpBarBg.png","./assets/combat/hpBar.png",SDL_Rect{905,1004,204,27},0,nullptr);

    UIElement combat_critStars("critStars","./assets/combat/critStars.png",SDL_Rect{1363,969,157,60});
    Text combat_critStarNumber("critStarNumber","./assets/fonts/combat_main.ttf",SDL_Rect{1520,988,157,60},"0",SDL_Color(220,220,220),40);
    UIElement combat_target("target","./assets/combat/target.png",SDL_Rect{826,420,116,116});

    OutlinedText combat_DmgNumber("DmgNumber","./assets/fonts/combat_DMG.otf",SDL_Rect{1343,145},"0",SDL_Color(255,255,255),SDL_Color{39, 39, 46},120,4);
    combat_DmgNumber.Rotate(20);
    OutlinedText combat_EnemyDmgNumber("enemyDmgNumber","./assets/fonts/combat_DMG.otf",SDL_Rect{0,720},"0",SDL_Color{115, 16, 16},SDL_Color{48, 4, 4},90,3);

    OutlinedText combat_skillNeedsTarget("skillNeedsTarget","./assets/fonts/combat_main.ttf",SDL_Rect{300,650},"SKILL NEEDS TARGET",SDL_Color{215, 96, 219},SDL_Color{20,20,20},70,1);

    OutlinedText combat_whosTurn("whosTurn","./assets/fonts/combat_main.ttf",SDL_Rect{140,105},"0",SDL_Color{115, 16, 16},SDL_Color{20,20,20},50,1);
    UIElement combat_next0("next0","./assets/UI/empty.jpg", SDL_Rect{560,105,78,51});
    combat_next0.ScaleUp(1.6,0,0);
    UIElement combat_next1("next1","./assets/UI/empty.jpg", SDL_Rect{713,105,78,51});
    UIElement combat_next2("next2","./assets/UI/empty.jpg", SDL_Rect{821,105,78,51});
    UIElement combat_next3("next3","./assets/UI/empty.jpg", SDL_Rect{929,105,78,51});
    UIElement combat_next4("next4","./assets/UI/empty.jpg", SDL_Rect{1037,105,78,51});
    UIElement combat_turnOrderBg("turnOrderBg","./assets/combat/turnOrder.png", SDL_Rect{554,86,567,113});

    UIElement combat_battleOver("battleOver","./assets/combat/battleOver.png",SDL_Rect{331,500,1393,224});

    std::vector<UIElement*> combatTurnOrderFIFO = {&combat_next0,&combat_next1,&combat_next2,&combat_next3,&combat_next4};

    std::vector<UIElement *> combatConsistentUIBoundle = {
        &combat_baqBg, &combat_NA, &combat_skill, &combat_Buster, &combat_Arts, &combat_Quick,
        &combat_CritChanceB, &combat_CritChanceA, &combat_CritChanceQ, &combat_CritChanceTextB, &combat_CritChanceTextA,
        &combat_CritChanceTextQ,
        &combat_spriteCont1L2, &combat_spriteCont1L1, &combat_spriteCont2L2, &combat_spriteCont2L1,
        &combat_spriteCont3L2, &combat_spriteCont3L1,
        &combat_ult1, &combat_ult2, &combat_ult3, &combat_spBg, &combat_sp,
        &combat_hpBar1, &combat_hpBar2, &combat_hpBar3, &combat_critStars, &combat_critStarNumber, &combat_target,
        &combat_DmgNumber, &combat_EnemyDmgNumber, &combat_skillNeedsTarget,
        &combat_next0, &combat_next1, &combat_next2, &combat_next3, &combat_next4, &combat_whosTurn, &combat_turnOrderBg,
        &combat_battleOver
    };

    std::vector<UIElement*> combatCh1Cont = {&combat_spriteCont1L1,&combat_spriteCont1L2,&combat_ult1,&combat_hpBar1};
    std::vector<UIElement*> combatCh2Cont = {&combat_spriteCont2L1,&combat_spriteCont2L2,&combat_ult2,&combat_hpBar2};
    std::vector<UIElement*> combatCh3Cont = {&combat_spriteCont3L1,&combat_spriteCont3L2,&combat_ult3,&combat_hpBar3};

    std::vector<UIElement*> combatNATypeChooser = {&combat_baqBg,&combat_Buster,&combat_Arts,&combat_Quick,
        &combat_CritChanceB,&combat_CritChanceA,&combat_CritChanceQ,
        &combat_CritChanceTextB,&combat_CritChanceTextA,&combat_CritChanceTextQ};


    auto Buster = TypeMultiplier(CardTypes::Buster);
    auto Arts = TypeMultiplier(CardTypes::Arts);
    auto Quick = TypeMultiplier(CardTypes::Quick);


    Game::combat_target = &combat_target;
    Game::combatAnimProps = combatAnimProps;
    Game::combatConsistentUIBoundle = combatConsistentUIBoundle;
    Game::combatCh1Cont = combatCh1Cont;
    Game::combatCh2Cont = combatCh2Cont;
    Game::combatCh3Cont = combatCh3Cont;
    Game::combatNATypeChooser = combatNATypeChooser;
    Game::combatTurnOrderFIFO = combatTurnOrderFIFO;


    //CONSISTENT SCENES
    //for testing



    ///----------TURORIALS SCENE INIT-------------------
    ///
    //nee the main menu before, cos the button functions c:
    Scene mainMenu("mainMenu","./assets/backgrounds/mainMenu.png");


    ListingScene tutorials("tutorials","./assets/backgrounds/tutorials.png");
    Button closeMenu("close","./assets/UI/closeTutorials.png",SDL_Rect{1785,86,118,87},[&mainMenu]{ChangeScene(&mainMenu);});
    tutorials.AddUIElement(&closeMenu);
    Button::AddBasicScaleUpHoverAnim(&closeMenu);

    TextButton tutorials_op1("op1","./assets/UI/tutorialsButtonBg.png",SDL_Rect{322,0,340,50},
        [&tutorials] {
                tutorials.ChagneMenu("op1");
            },"./assets/fonts/combat_main.ttf","Action Order",SDL_Color{161,215,231});

    TextButton tutorials_op2("op2","./assets/UI/tutorialsButtonBg.png",SDL_Rect{322,0,340,50},
    [&tutorials] {
            tutorials.ChagneMenu("op2");
        },"./assets/fonts/combat_main.ttf","Character actions: NA",SDL_Color{161,215,231});

    TextButton tutorials_op3("op3","./assets/UI/tutorialsButtonBg.png",SDL_Rect{322,0,340,50},
    [&tutorials] {
            tutorials.ChagneMenu("op3");
        },"./assets/fonts/combat_main.ttf","Critical damage",SDL_Color{161,215,231});

    TextButton tutorials_op4("op4","./assets/UI/tutorialsButtonBg.png",SDL_Rect{322,0,340,50},
    [&tutorials] {
            tutorials.ChagneMenu("op4");
        },"./assets/fonts/combat_main.ttf","Energy- Ult",SDL_Color{161,215,231});

    TextButton tutorials_op5("op5","./assets/UI/tutorialsButtonBg.png",SDL_Rect{322,0,340,50},
    [&tutorials] {
            tutorials.ChagneMenu("op5");
        },"./assets/fonts/combat_main.ttf","Character actions: Skill",SDL_Color{161,215,231});

    TextButton tutorials_op6("op6","./assets/UI/tutorialsButtonBg.png",SDL_Rect{322,0,340,50},
    [&tutorials] {
            tutorials.ChagneMenu("op6");
        },"./assets/fonts/combat_main.ttf","Enemy action",SDL_Color{161,215,231});

    TextButton tutorials_op7("op7","./assets/UI/tutorialsButtonBg.png",SDL_Rect{322,0,340,50},
    [&tutorials] {
            tutorials.ChagneMenu("op7");
        },"./assets/fonts/combat_main.ttf","Bonus: Strategy",SDL_Color{161,215,231});

    std::vector<Button*> tutorials_triggers = {&tutorials_op1,&tutorials_op2,&tutorials_op3,&tutorials_op4,&tutorials_op5,&tutorials_op6,&tutorials_op7};

    UIElement tutoials_op1Text("tutoials_op1Text","./assets/UI/tutorials/op1.png",SDL_Rect{490,110,1372,890});
    UIElement tutoials_op2Text("tutoials_op2Text","./assets/UI/tutorials/op2.png",SDL_Rect{490,110,1372,890});
    UIElement tutoials_op3Text("tutoials_op3Text","./assets/UI/tutorials/op3.png",SDL_Rect{490,110,1372,890});
    UIElement tutoials_op4Text("tutoials_op4Text","./assets/UI/tutorials/op4.png",SDL_Rect{490,110,1372,890});
    UIElement tutoials_op5Text("tutoials_op5Text","./assets/UI/tutorials/op5.png",SDL_Rect{490,110,1372,890});
    UIElement tutoials_op6Text("tutoials_op6Text","./assets/UI/tutorials/op6.png",SDL_Rect{490,110,1372,890});
    UIElement tutoials_op7Text("tutoials_op7Text","./assets/UI/tutorials/op7.png",SDL_Rect{490,110,1372,890});




    std::vector<std::vector<UIElement*>> tutorials_menus = {{&tutoials_op1Text},{&tutoials_op2Text},{&tutoials_op3Text},{&tutoials_op4Text},{&tutoials_op5Text},{&tutoials_op6Text},{&tutoials_op7Text}};

    tutorials.AddStates(tutorials_triggers,tutorials_menus);
    tutorials.EQPositionTriggers(SDL_Rect{171,252,518,741},false);

    for(auto& button : tutorials_triggers) {
        Button::AddBasicScaleUpHoverAnim(button,0.9f,0.5f,1.2f);
    }


    ///----------CHAROVERVIEW SCENE INIT-------------------
    ///
    CharOverview charOverview("preBattle","./assets/backgrounds/charOverview.png",characters,player->GetMats());

    //triggers / menus
    std::vector<Button*> charOverview_ops;
    for(size_t i = 0; i < characters.size(); i++) {
        std::string name = characters.at(i)->GetName();
        std::string path = "./assets/charOverview/" + characters.at(i)->GetName() + ".png";
        charOverview_ops.push_back(new Button(name.c_str(),path.c_str(),SDL_Rect{0,753,258,254}));
        charOverview_ops.at(i)->AddOnclick(
            [i, &charOverview]{charOverview.ChagneMenu(i);});
    }


    //other charOverview UI elements

    Text charOverview_nameText("name","./assets/fonts/starlight.ttf",SDL_Rect{114,94},"empty",SDL_Color{222,222,222},166);
    Text charOverview_level("level","./assets/fonts/combat_DMG.otf",SDL_Rect{160,40},"Lv. 1",SDL_Color{222,222,222},50);

    UIElement charOverview_nameFrame("nameFrame","./assets/charOverview/nameFrame.png",SDL_Rect{15,79,850,347});
    Text charOverview_Atk("baseatk","./assets/fonts/combat_main.ttf",SDL_Rect{115,275},"ATK:");
    Text charOverview_Hp("maxhp","./assets/fonts/combat_main.ttf",SDL_Rect{115,307},"HP:");
    Text charOverview_Cd("critDmg","./assets/fonts/combat_main.ttf",SDL_Rect{115,339},"CD:");
    Text charOverview_Spd("speed","./assets/fonts/combat_main.ttf",SDL_Rect{115,371},"SPD:");
    Text charOverview_Sg("starGen","./assets/fonts/combat_main.ttf",SDL_Rect{300,275},"SG:");
    Text charOverview_Sa("starAbsorbB","./assets/fonts/combat_main.ttf",SDL_Rect{300,307},"SA:");
    Text charOverview_Er("ER","./assets/fonts/combat_main.ttf",SDL_Rect{300,339},"ER:");
    Text charOverview_Uc("ultCost","./assets/fonts/combat_main.ttf",SDL_Rect{300,371},"Ult cost:");

    std::vector<Text*> charOverview_statTexts = {&charOverview_nameText,&charOverview_Atk, &charOverview_Hp, &charOverview_Cd, &charOverview_Spd,&charOverview_Sg, &charOverview_Sa, &charOverview_Er, &charOverview_Uc};


    UIElement charOverview_matTier1("matTier1","./assets/UI/matTier1.png",SDL_Rect{415,434,211,398});
    charOverview_matTier1.GetTransform().scale = 0.1f;
    Text charOverview_matTier1Text("matTier1Text","./assets/fonts/combat_main.ttf",SDL_Rect{445,434,211,398},std::to_string(player->GetMats().tier1).c_str());

    UIElement charOverview_matTier2("matTier2","./assets/UI/matTier2.png",SDL_Rect{520,434,211,398});
    charOverview_matTier2.GetTransform().scale = 0.1f;
    Text charOverview_matTier2Text("matTier2Text","./assets/fonts/combat_main.ttf",SDL_Rect{550,434,211,398},std::to_string(player->GetMats().tier2).c_str());

    UIElement charOverview_matTier3("matTier3","./assets/UI/matTier3.png",SDL_Rect{620,434,211,398});
    charOverview_matTier3.GetTransform().scale = 0.1f;
    Text charOverview_matTier3Text("matTier3Text","./assets/fonts/combat_main.ttf",SDL_Rect{650,434,211,398},std::to_string(player->GetMats().tier3).c_str());


    TextButton charOverview_LvlUp("lvlUp","./assets/UI/tutorialsButtonBg.png",SDL_Rect{67,434,340,50},[&charOverview]{charOverview.Level();},"./assets/fonts/combat_main.ttf","Ascend");
    Button::AddBasicScaleUpHoverAnim(&charOverview_LvlUp);

    UIElement charOverview_KitDesc("kitDesc","./assets/charOverview/kitDesc.png",SDL_Rect{1483,270,413,383});

    std::vector<std::vector<UIElement*>> charOverview_menus;
    for (auto & character : characters) {
        std::string path = "./assets/charOverview/" + character->GetName() + "story.png";
        auto* story = new UIElement("story",path.c_str(),SDL_Rect{79,527,498,182});

        path = "./assets/charOverview/" + character->GetName() + "skill.png";
        auto* kit = new Sprite("kit",path.c_str(),SDL_Rect{1523,429,318,153});

        Button* skillButton = new Button("skill","./assets/charOverview/skill.png",SDL_Rect{1479,162,195,184},[kit] {
            kit->ChangeState("base");
        });
        Button* ultButton = new Button("ult","./assets/charOverview/ult.png",SDL_Rect{1703,104,194,188},[kit] {
            kit->ChangeState("ult");
        });

        path = "./assets/charOverview/" + character->GetName() + "ult.png";
        kit->AddState("ult",path.c_str());
        std::vector<UIElement*> temp = {kit,story,skillButton,ultButton};
        charOverview_menus.push_back(temp);
    }

    charOverview.AddStates(charOverview_ops,charOverview_menus);
    charOverview.AddStatTextsCont(charOverview_statTexts);
    charOverview.EQPositionTriggers(SDL_Rect{9,753,1900,318},true);
    for (auto& button : charOverview_ops) {
        Button::AddBasicScaleUpHoverAnim(button,0.5f,1.0f,1.15f);
    }

    charOverview.AddUIElement(&charOverview_KitDesc);
    charOverview.AddUIElement(&charOverview_nameFrame);
    charOverview.AddUIElement(&charOverview_level);
    charOverview.AddUIElement(&charOverview_LvlUp);
    charOverview.AddUIElement(&charOverview_matTier1);
    charOverview.AddUIElement(&charOverview_matTier2);
    charOverview.AddUIElement(&charOverview_matTier3);
    charOverview.AddUIElement(&charOverview_matTier1Text);
    charOverview.AddUIElement(&charOverview_matTier2Text);
    charOverview.AddUIElement(&charOverview_matTier3Text);

    Button closeMenuCharOverview("close","./assets/UI/closeTutorials.png",SDL_Rect{1785,10,118,87},[&mainMenu]{ChangeScene(&mainMenu);});

    charOverview.AddUIElement(&closeMenuCharOverview);


    ///----------PREBATTLE SCENE INIT-------------------
    ///
    UIElement char1SlotBg("slot1Bg","./assets/UI/preFightSlot1Bg.png",SDL_Rect{325,210,465,652});
    UIElement char2SlotBg("slot2Bg","./assets/UI/preFightSlot1Bg.png",SDL_Rect{740,210,465,652});
    UIElement char3SlotBg("slot3Bg","./assets/UI/preFightSlot1Bg.png",SDL_Rect{1152,210,465,652});

    UIElement char1Slot("slot1","./assets/UI/empty.jpg",SDL_Rect{200,190,709,1417});
    //char1Slot.transform.scrRect = SDL_Rect{125,0,465,652};
    UIElement char2Slot("slot2","./assets/UI/empty.jpg",SDL_Rect{615,190,709,1417});
    //char2Slot.transform.scrRect = SDL_Rect{125,0,465,652};
    UIElement char3Slot("slot3","./assets/UI/empty.jpg",SDL_Rect{1027,190,709,1417});
    //char3Slot.transform.scrRect = SDL_Rect{125,0,465,652};



    UIElement preFight_charChooserBg("charChooserBg","./assets/UI/characterChooserBg.png",SDL_Rect{0,497,1920,583});

    std::vector<Button*> prefight_charOps;
    for(auto & character : characters) {
        std::string name = character->GetName() + "Option";
        std::string path = "./assets/charOverview/" + character->GetName() + ".png";
        prefight_charOps.push_back(new Button(name.c_str(),path.c_str(),SDL_Rect{0,753,258,254}));
    }
    EQPosUIElements(SDL_Rect{9,753,1900,318},prefight_charOps,true);
    std::vector<UIElement*> preFightSlots = {&char1SlotBg,&char2SlotBg,&char3SlotBg,&char1Slot,&char2Slot,&char3Slot};
    std::vector<UIElement*> preFightCharChoosers = {&preFight_charChooserBg};
    for(auto& op : prefight_charOps)
        preFightCharChoosers.push_back(op);


    for(auto& element:preFightCharChoosers) {
        element->SetVisible(false);
        element->GetTransform().scale = 0.8f;
    }


    Button preFight_Overlay("overlay","./assets/backgrounds/preBattle.png",SDL_Rect{0,0,1920,1080},
        [preFightCharChoosers,&preFight_Overlay] {
            if(preFightCharChoosers.at(0)->IsVisible()) {
                for(auto& element:preFightCharChoosers) {
                    element->SetVisible(false);
                }
            preFight_Overlay.Disable(true);
            }
        });
    preFight_Overlay.ChangeClickFiled(SDL_Rect{0,0,1920,605});
    preFight_Overlay.Disable(true);

    Button char1SlotButton("slot1Button","./assets/UI/preFightInvButton.png",SDL_Rect{325,210,465,652},
        [preFightCharChoosers,&preFight_Overlay] {
            if(!preFightCharChoosers.at(0)->IsVisible()) {
                for(auto& element:preFightCharChoosers) {
                    element->SetVisible(true);
                }
            preFight_Overlay.Disable(false);
            }
        });
    Button char2SlotButton("slot2Button","./assets/UI/preFightInvButton.png",SDL_Rect{740,210,465,652},
        [preFightCharChoosers,&preFight_Overlay] {
            if(!preFightCharChoosers.at(0)->IsVisible()) {
                for(auto& element:preFightCharChoosers) {
                    element->SetVisible(true);
                }
            preFight_Overlay.Disable(false);
            }
        });
    Button char3SlotButton("Button","./assets/UI/preFightInvButton.png",SDL_Rect{1152,210,465,652},
        [preFightCharChoosers,&preFight_Overlay] {
            if(!preFightCharChoosers.at(0)->IsVisible()) {
                for(auto& element:preFightCharChoosers) {
                    element->SetVisible(true);
                }
            preFight_Overlay.Disable(false);
            }
        });

    Button startCombat("startCombat","./assets/UI/startCombatButton.png",SDL_Rect{729,49,494,73});
    Button::AddBasicScaleUpHoverAnim(&startCombat);

    Game::preFightConsistentUIBoundle = {&preFight_Overlay,&char1SlotButton,&char2SlotButton,&char3SlotButton,&startCombat,&closeMenu};
    Game::preFightCharChoosers = preFightCharChoosers;
    Game::preFightSlots = preFightSlots;


    ///----------ADVENTURE MENU INIT-------------------
    ///
    AdventuresMenu adventures("adventureMenu","./assets/backgrounds/adventure.png");

    // Directory with fight node data
    const std::string folderPath = "./assets/nodeData/fight";

    // Iterate through the `fight` folder and process `.txt` files
    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            try {
                // Create and add FightNode to AdventuresMenu
                auto* fightNode = new FightNode(entry.path().string().c_str());
                adventures.AddNode(fightNode);
            } catch (const std::exception& e) {
                std::cerr << "Error creating FightNode from file: " << entry.path() << " - " << e.what() << '\n';
            }
        }
    }
    adventures.PositionNodes();
    adventures.AddUIElement(&closeMenu);

    ///----------POSTFIGHT MENU INIT-------------------
    ///
    Scene postFight("posFight","./assets/backgrounds/afterBattle.png");
    UIElement postFight_charAnim1("char1","./assets/UI/empty.jpg",SDL_Rect{725,147,709,1417});
    postFight_charAnim1.ScaleUp(0.8,1,0.5);
    UIElement postFight_charAnim2("char2","./assets/UI/empty.jpg",SDL_Rect{1206,112,709,1417});
    postFight_charAnim2.ScaleUp(0.8,1,0.5);
    UIElement postFight_charAnim3("char3","./assets/UI/empty.jpg",SDL_Rect{972,-75,709,1417});
    postFight_charAnim3.ScaleUp(0.8,1,0.5);

    UIElement postFight_materialTier1("tier1Mat","./assets/UI/matTier1.png",SDL_Rect{311,475,211,398});
    postFight_materialTier1.GetTransform().scale = 0.3f;
    UIElement postFight_materialTier2("tier2Mat","./assets/UI/matTier2.png",SDL_Rect{517,489,211,398});
    postFight_materialTier2.GetTransform().scale = 0.2f;
    UIElement postFight_materialTier3("tier3Mat","./assets/UI/matTier3.png",SDL_Rect{690,497,211,398});
    postFight_materialTier3.GetTransform().scale = 0.15f;

    Text postFight_tier1Gained("tier1Gained","./assets/fonts/combat_main.ttf",SDL_Rect{371,500,211,398},"100",SDL_Color{255,255,255},50);
    Text postFight_tier2Gained("tier2Gained","./assets/fonts/combat_main.ttf",SDL_Rect{577,500,211,398},"100",SDL_Color{255,255,255},50);
    Text postFight_tier3Gained("tier3Gained","./assets/fonts/combat_main.ttf",SDL_Rect{740,500,211,398},"100",SDL_Color{255,255,255},50);

    Sprite postFight_nodeStar1("lessthan5","./assets/UI/postBattleStarEmpty.png",SDL_Rect{404,625,90,87});
    postFight_nodeStar1.AddState("got","./assets/UI/postBattleStarGot.png");
    Sprite postFight_nodeStar2("lessthan10","./assets/UI/postBattleStarEmpty.png",SDL_Rect{539,625,90,87});
    postFight_nodeStar2.AddState("got","./assets/UI/postBattleStarGot.png");
    Sprite postFight_nodeStar3("noDeaths","./assets/UI/postBattleStarEmpty.png",SDL_Rect{674,625,90,87});
    postFight_nodeStar3.AddState("got","./assets/UI/postBattleStarGot.png");

    TextButton postFight_continue("continue","./assets/UI/tutorialsButtonBg.png",SDL_Rect{256,756,340,50},
        [&mainMenu] {
                ChangeScene(&mainMenu);
            },"./assets/fonts/combat_main.ttf","Continue",SDL_Color{161,215,231});    ///----------MAIN MENU INIT-------------------
    Button::AddBasicScaleUpHoverAnim(&postFight_continue);

    postFight.AddUIElement(&postFight_charAnim3);
    postFight.AddUIElement(&postFight_charAnim2);
    postFight.AddUIElement(&postFight_charAnim1);
    postFight.AddUIElement(&postFight_materialTier1);
    postFight.AddUIElement(&postFight_materialTier2);
    postFight.AddUIElement(&postFight_materialTier3);
    postFight.AddUIElement(&postFight_tier1Gained);
    postFight.AddUIElement(&postFight_tier2Gained);
    postFight.AddUIElement(&postFight_tier3Gained);
    postFight.AddUIElement(&postFight_continue);
    postFight.AddUIElement(&postFight_nodeStar1);
    postFight.AddUIElement(&postFight_nodeStar2);
    postFight.AddUIElement(&postFight_nodeStar3);

    ///----------MAIN MENU INIT-------------------
    ///

    SpriteButton startFightButton("startFightButton","./assets/UI/screens.png",SDL_Rect{1178,0,744,873},[&adventures]{ChangeScene(&adventures);});
    startFightButton.AddState("hover","./assets/UI/screensHover.png");
    startFightButton.AddHoverBehaviour([&startFightButton]{startFightButton.ChangeState("hover");},[&startFightButton]{startFightButton.ChangeState("startFightButton");});

    SpriteButton charactersScreenButton("charactersScreenButton","./assets/UI/drawer.png",SDL_Rect{208,164,490,383},[&charOverview]{ChangeScene(&charOverview); charOverview.Reset();});
    charactersScreenButton.AddState("hover","./assets/UI/drawerHover.png");
    charactersScreenButton.AddHoverBehaviour([&charactersScreenButton]{charactersScreenButton.ChangeState("hover");},[&charactersScreenButton]{charactersScreenButton.ChangeState("charactersScreenButton");});

    SpriteButton tutorialsButton("tutorialsButton","./assets/UI/computer.png",SDL_Rect{105,563,690,349},[&tutorials]{ChangeScene(&tutorials); tutorials.Reset();});
    tutorialsButton.AddState("hover","./assets/UI/computerHover.png");
    tutorialsButton.AddHoverBehaviour([&tutorialsButton]{tutorialsButton.ChangeState("hover");},[&tutorialsButton]{tutorialsButton.ChangeState("tutorialsButton");});


    Button quit("quit","./assets/UI/quit.png",SDL_Rect{67,60,60,59},[]{SimulateKeyPress(SDLK_ESCAPE);});
    Button::AddBasicScaleUpHoverAnim(&quit);

    mainMenu.AddUIElement(&startFightButton);
    mainMenu.AddUIElement(&charactersScreenButton);
    mainMenu.AddUIElement(&tutorialsButton);
    mainMenu.AddUIElement(&quit);

    SpriteButton* MMTutorial = nullptr;
    if (player->GetTutorialNeed("MM")) {
        MMTutorial = new SpriteButton("MMTutorial","./assets/tutOverlay/MM1.png",SDL_Rect{0,0,1920,1080},[&mainMenu]{mainMenu.TutorialsMenuNext();});
        MMTutorial->AddState("MM2","./assets/tutOverlay/MM2.png");
        MMTutorial->AddState("MM3","./assets/tutOverlay/MM3.png");
        MMTutorial->AddState("MM4","./assets/tutOverlay/MM4.png");
        MMTutorial->SetVisible(false);
        mainMenu.AddTutorialsMenu(MMTutorial);
    }

    SpriteButton* COTutorial = nullptr;
    if (player->GetTutorialNeed("CO")) {
        COTutorial = new SpriteButton("COTutorial","./assets/tutOverlay/CO1.png",SDL_Rect{0,0,1920,1080},[&charOverview]{charOverview.TutorialsMenuNext();});
        COTutorial->AddState("CO2","./assets/tutOverlay/CO2.png");
        COTutorial->AddState("CO3","./assets/tutOverlay/CO3.png");
        COTutorial->AddState("CO4","./assets/tutOverlay/CO4.png");
        COTutorial->AddState("CO5","./assets/tutOverlay/CO5.png");
        COTutorial->AddState("CO6","./assets/tutOverlay/CO6.png");
        COTutorial->SetVisible(false);
        charOverview.AddTutorialsMenu(COTutorial);
    }

    SpriteButton* ADTutorial = nullptr;
    if (player->GetTutorialNeed("AD")) {
        ADTutorial = new SpriteButton("ADTutorial","./assets/tutOverlay/AD1.png",SDL_Rect{0,0,1920,1080},[&adventures]{adventures.TutorialsMenuNext();});
        ADTutorial->AddState("AD2","./assets/tutOverlay/AD2.png");
        ADTutorial->AddState("AD3","./assets/tutOverlay/AD3.png");
        ADTutorial->AddState("AD4","./assets/tutOverlay/AD4.png");
        ADTutorial->SetVisible(false);
        adventures.AddTutorialsMenu(ADTutorial);
    }

    SpriteButton* PFTutorial = nullptr;
    if (player->GetTutorialNeed("PF")) {
        PFTutorial = new SpriteButton("PFTutorial","./assets/tutOverlay/PF1.png",SDL_Rect{0,0,1920,1080},[]{});
        PFTutorial->AddState("PF2","./assets/tutOverlay/PF2.png");
        PFTutorial->AddState("PF3","./assets/tutOverlay/PF3.png");
        PFTutorial->AddState("PF4","./assets/tutOverlay/PF4.png");
        PFTutorial->SetVisible(false);
        adventures.AddPreFightTutorials(PFTutorial);
    }
    adventures.InitPreFights();

    ///----------STARTING SCENE INIT-------------------
    ///
    Scene startingScreen("startingScreen","./assets/backgrounds/startingScreen.jpg");
    Button startGame("startGame","./assets/UI/startGameButton.png",SDL_Rect{1000,940,273,35},[&mainMenu]{ChangeScene(&mainMenu);});




    startingScreen.AddUIElement(&startGame);
    {
        /// UPDATE
        ///
        bool running = true;
        Uint32 lastTime = SDL_GetTicks(); // at the beginning

        ChangeScene(&startingScreen);
        bool isStartingScreen = true;

        SDL_RenderClear(renderer);
        Game::currentScene->LoadScene();
        SDL_RenderPresent(renderer);

        while (running && isStartingScreen) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                else if(event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN) {
                    ChangeScene(&mainMenu);

                    isStartingScreen = false;
                }

            }

            //(16); // Small delay to prevent CPU overuse (~60 FPS)
        }
 ///Loop is event type handling to avoid unnecesary rendering.
        ///(Sience it's turn based with minimal animations, not much will happen unless the user clicked or typed a kew)
        ///
        while (running) {
            SDL_Event event;
            bool wasEvent = SDL_PollEvent(&event);
            if (wasEvent) {
                if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                ///USER INTERACTIONS
                ///
                if(event.type == SDL_MOUSEMOTION) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    Game::currentScene->CheckButtonsHover(mouseX,mouseY);
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    // Get mouse position
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    Game::currentScene->CheckButtonsOnClick(mouseX,mouseY);
                }
            }
            auto fight = dynamic_cast<Fight*>(Game::currentScene);
            if(fight) {
                if(fight->AreAnimsOver()){
                    if (fight->GetCombatState() == OVER) {
                        ChangeScene(&postFight);
                        PostFightAnim(postFight.GetUIElements(),fight->GetCharacters(),fight->MaterialsEarned(),fight->GetLevelStars());
                        player->UpdateMats(fight->MaterialsEarned());
                        adventures.UpdateLastNode(fight->GetLevelStars());
                    }
                    if(fight->GetCombatState() == WAITINGFORENEMYANIM) {
                        fight->CountNextAction();
                    }
                    else if(fight->GetCombatState() == ENEMYACTION) {
                        fight->EnemyAction();
                    }
                    else if(wasEvent){
                        if(event.type == SDL_KEYDOWN) {
                            switch (fight->GetCombatState()) {
                                case WAITINGFORTARGET: {
                                    switch (event.key.keysym.sym) {
                                        case SDLK_1:
                                            fight->Skill(1);
                                        break;
                                        case SDLK_2:
                                            fight->Skill(2);
                                        break;
                                        case SDLK_3:
                                            fight->Skill(3);
                                        break;

                                        default: break;
                                    }
                                    break;
                                }
                                case WAITINGFORTYPE: {
                                    switch (event.key.keysym.sym) {
                                        case SDLK_1:
                                            fight->Normal(Buster);
                                        break;
                                        case SDLK_2:
                                            fight->Normal(Arts);
                                        break;
                                        case SDLK_3:
                                            fight->Normal(Quick);
                                        break;

                                        case SDLK_x:
                                            fight->SetCombatState(WAITINGFORACTION);
                                        fight->SetNaTypeChooser(false);
                                        break;

                                        default: break;
                                    }
                                    break;
                                }
                                case WAITINGFORACTION: {
                                    switch (event.key.keysym.sym) {
                                        case SDLK_e: {
                                            if (!fight->GetCurrentlyActing()->CanSkill())
                                                break;
                                            fight->SetNaTypeChooser(false);
                                            if(fight->GetCurrentlyActing()->skillNeedsTarget) {
                                                std::cout << "Skill needs target" << std::endl;
                                                fight->SetSkillNeedsTarget(true);
                                                fight->SetCombatState(WAITINGFORTARGET);
                                            }
                                            else {
                                                fight->Skill(fight->GetCurrentlyActing());
                                            }
                                            break;
                                        }
                                        case SDLK_q: {
                                            fight->SetCombatState(WAITINGFORTYPE);
                                            fight->SetNaTypeChooser(true);
                                            std::cout << "waiting for type" << std::endl;

                                            break;
                                        }
                                        case SDLK_1:
                                            fight->Ult(1);
                                        break;

                                        case SDLK_2:
                                            fight->Ult(2);
                                        break;
                                        case SDLK_3:
                                            fight->Ult(3);
                                        break;
                                        case SDLK_a:
                                            fight->ChangeEnemyTarget(-1);
                                        break;
                                        case SDLK_d:
                                            fight->ChangeEnemyTarget(1);
                                        break;
                                        default: break;
                                    }
                                    break;
                                }
                                default: break;
                            }
                        }
                    }
                }
            }
            //Update render when needed.

            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f; // convert ms to seconds
            lastTime = currentTime;


            Game::currentScene->UpdateTweens(deltaTime);

            SDL_RenderClear(renderer);
            Game::currentScene->LoadScene();
            SDL_RenderPresent(renderer);

            //SDL_Delay(16); // Small delay to prevent CPU overuse (~60 FPS)
        }
    }
    UpdateProfileData(player);
    adventures.WriteNodeDataToFile();

    for (auto it = characters.begin(); it != characters.end(); ) {
        delete *it;
        it = characters.erase(it);
    }
    for (auto it = prefight_charOps.begin(); it != prefight_charOps.end(); ) {
        delete *it;
        it = prefight_charOps.erase(it);
    }
    for (auto it = charOverview_ops.begin(); it != charOverview_ops.end(); ) {
        delete *it;
        it = charOverview_ops.erase(it);
    }
    for (auto& menu:charOverview_menus) {
        for (auto it = menu.begin(); it != menu.end(); ) {
            delete *it;
            it = menu.erase(it);
        }
    }
    delete MMTutorial;
    delete COTutorial;
    delete ADTutorial;
    delete PFTutorial;

    delete player;
    //EXIT
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
