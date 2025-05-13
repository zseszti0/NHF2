#ifndef SCENE_H
#define SCENE_H


class Scene;
class ListingScene;
class Fight;
class PreFight;

#include "UIAndDisplay.h"
#include "combat.h"
#include "adventure.h"
#include "gameEngine.h"

#include <utility>


class Scene {
protected:
  std::string name;
  SDL_Texture* bg;
  std::vector<UIElement*> uiBoundle;

  SpriteButton* tutorialsOverlay = nullptr;
  bool tutorialsActive = false;

public:
  static SDL_Renderer* renderer;


  Scene(const char* n, const char* texturePath);
  virtual void Init();
  void AddTutorialsMenu(SpriteButton* overlay){tutorialsOverlay = overlay; tutorialsActive = true;}
  void TutorialsMenuNext();

  void AddUIElement(UIElement* newElement);
  void AddUIElement(std::vector<UIElement*>& newElements);
  std::vector<UIElement*>& GetUIElements(){return uiBoundle;}
  virtual void LoadScene();
  void RenderOne(const char* targetName) {
    for(auto& element: uiBoundle) {
      if(element->GetName() == targetName)
        element->Render();
    }
  }

  virtual void UpdateTweens(float dt) {
    for(auto& element : uiBoundle)
      element->UpdateTweens(dt);
  }

  virtual void CheckButtonsOnClick(int mouseX, int mouseY);
  virtual void CheckButtonsHover(int mouseX, int mouseY) ;

  virtual ~Scene(){
    SDL_DestroyTexture(bg);
    std::cout << "Scene: " << name << " dtor" <<std::endl;
  }
};

class ListingScene : public Scene {
protected:
  std::vector<Button*> triggers;
  std::vector<std::vector<UIElement*>> menus;
  int currentMenu = 0;
public:
  ListingScene(const char* n, const char* texturePath) : Scene(n, texturePath){};
  void AddState(Button* newTrigger, const std::vector<UIElement*>& menu) {
    triggers.push_back(newTrigger);
    menus.push_back(menu);
  }
  void AddStates(const std::vector<Button*> &newTriggers, const std::vector<std::vector<UIElement*>>& newMenus) {
    for(int i = 0; i < newTriggers.size(); i++) {
      AddState(newTriggers[i], newMenus[i]);
    }
  }

  virtual void ChagneMenu(int i) {
    for(auto element : menus.at(currentMenu)) {
      element->SetVisible(false);
    }
    currentMenu = i;
    for(auto element : menus.at(currentMenu)) {
      element->SetVisible(true);
    }
  }
  virtual void ChagneMenu(const char* name) {
    int i = GetIndexByName(name);
    this->ChagneMenu(i);
  }
  int GetIndexByName(const std::string& name) {
    for(int i = 0; i < triggers.size(); i++) {
      if(triggers.at(i)->GetName() == name)
        return i;
    }
    return 0;
  }

  void Reset();

  void CheckButtonsOnClick(int mouseX, int mouseY) override;
  void CheckButtonsHover(int mouseX, int mouseY) override;

  void EQPositionTriggers(SDL_Rect area, bool horizontal = true);

  void LoadScene() override;

  ~ListingScene() override{
    SDL_DestroyTexture(bg);
    std::cout << "Scene: " << name << " dtor" <<std::endl;
  }
};

class CharOverview : public ListingScene {
  Character* activeCharacter = nullptr;
  std::vector<Character*> characters;
  std::vector<Text*> statTexts;

  GameMats& mats;
public:
  CharOverview(const char* n, const char* texturePath,std::vector<Character*> chars, GameMats& materials) : ListingScene(n, texturePath),characters(chars), mats(materials) {
    activeCharacter = characters.at(0);
  }
  void Init() override;
  void AddStatTextsCont(std::vector<Text*> cont) {
    statTexts = std::move(cont);
  }

  void ChagneMenu(int i) override;
  void Level();
  void UpdateMatsText();
  void UpdateLevelText();

  void LoadScene() override;

  ~CharOverview() override{
    SDL_DestroyTexture(bg);
    std::cout << "Scene: " << name << " dtor" <<std::endl;
  }
};

enum CombatState {
    ENEMYACTION,WAITINGFORACTION,WAITINGFORTYPE,WAITINGFORTARGET,OVER,WAITINGFORENEMYANIM
};
class Fight : public Scene {
    std::vector<Character*> characters;

    std::vector<Enemy*> enemies;
    std::vector<Bar*> enemyHps;

    std::vector<Mob*> fieldMobs;

    double AV;
    std::vector<Mob*> turnOrderFIFO;

    std::vector<std::vector<UIElement*>> combatChConts;
    std::vector<UIElement*> combatNATypeCont;
    std::vector<UIElement*> turnOrderCont;
    UIElement* combatTargetIndicator = nullptr;

    std::vector<UIElement*> animProps;

    Character* currentlyActing = nullptr;
    Enemy* currentlyActingEnemy = nullptr;
    CombatState combatState = WAITINGFORACTION;

    int starCount;
    int skillPoints;
    int currentEnemyTarget;

    int levelStars = 0;
public:

    Fight(const char* n, const char* texturePath, std::vector<Character*> chars) : Scene(n,texturePath), AV(0), currentlyActing(nullptr), combatState(WAITINGFORACTION), starCount(0), skillPoints(3), currentEnemyTarget(0) {
      characters = {chars.begin(),chars.begin()+3};
      std::cout <<"char size: " << characters.size() << std::endl;

    }
    std::vector<Character*>& GetCharacters(){return characters;}

    void AddEnemy(Enemy* e);
    void AddUIConts(std::vector<UIElement*>& ch1, std::vector<UIElement*>& ch2, std::vector<UIElement*>& ch3, std::vector<UIElement*>& natypes, UIElement* target,std::vector<UIElement*>& turnO);
    void AddAnimConts(std::vector<UIElement*>& props) {
        animProps = props;
    }

    void LoadScene() override;
    void Init() override {
      ConfigureStartingState();
    }
    void ConfigureStartingState();
    void CountNextAction();
    void CalculateNext();

    Character* GetCurrentlyActing(){return currentlyActing;}

    CombatState GetCombatState(){return combatState;}
    void SetCombatState(CombatState cs){combatState = cs;}

    //skills
    void Ult(int n);
    void Skill(int n);
    void Skill(Character* ch);
    void Normal(TypeMultiplier type);

    void EnemyAction();
    void EnemyAtk(Enemy* enemy);

    void AfterActionCleanup(bool wasMyAction);
    void CheckIfOver();
    void End(bool won);
    GameMats MaterialsEarned();

    void ChangeEnemyTarget(int dir);
    void RecalcEnemyTarget();
    int CalculateStarsGot(TypeMultiplier type);

    //visual thingy bingy's
    void TintGreyUIElement(SDL_Color color, const std::string& name); //name cos i do the looping through ui elements in the function, cleaner tzhat way.
    void SetCritStarText();
    void SetDmgText(Dmg dmg, SDL_Color color);
    void SetEnemyDmgText(Dmg dmg, int x);
    void SetHpVisuals();
    void SetUltCharge();
    void SetNaTypeChooser(bool isVisible);
    void ScaleCharacterCont(bool up, Character* ch);
    void ScaleUpEnemyCont();
    void PositionTarget(int enemyIndex);
    void SetSpVisuals();
    void ChangeTurnOrderCont();
    void SetWhosTurnText(bool mine);
    void SetSkillNeedsTarget(bool isVisible);

    //anims
    void UpdateTweens(float dt) override;
    void InitAnimPropsStartingState();
    void TriggerSkillAnim();
    bool AreAnimsOver();


    ~Fight() override {
        SDL_DestroyTexture(bg);

        for (auto enemy : enemies) {
            //delete enemy;
        }
        for (auto it = enemyHps.begin(); it != enemyHps.end(); ) {
                delete *it;             // free the memory
                it = enemyHps.erase(it);    // remove from vector safely
        }

        std::cout << "Fight: " << name << " dtor" <<std::endl;
    }

};

class PreFight : public Scene {
  std::vector<Character*> characters;
  int alreadyChosen = 0;

  std::vector<Enemy*> enemies;

  std::vector<UIElement*> charSlotCont;

  Fight* combat = nullptr;
public:
  PreFight(const char* n, const char* texturePath) : Scene(n,texturePath){};
  void AddCharCont(std::vector<UIElement*>& cont) {
    charSlotCont = cont;

    charSlotCont.at(3)->SetVisible(false);
    charSlotCont.at(4)->SetVisible(false);
    charSlotCont.at(5)->SetVisible(false);

  }
  void Reset();
  bool AddCharacter(Character* newCharacter);
  void LoadEnemies(Enemy* e);
  void LoadEnemies(std::vector<Enemy*> es);
  void LoadScene() override;

  void CheckButtonsOnClick(int mouseX, int mouseY) override;

  void StartFight();

  ~PreFight() override{
    SDL_DestroyTexture(bg);
    std::cout << "Scene: " << name << " dtor" <<std::endl;
    delete combat;
  }
};

class AdventuresMenu : public Scene {
  std::vector<FightNode*> nodes;

  SpriteButton* preFightTutorials = nullptr;

public:
  AdventuresMenu(const char* n, const char* texturePath) : Scene(n,texturePath){};
  void AddNode(FightNode* node) {
    nodes.push_back(node);
  }
  void InitPreFights();
  void AddPreFightTutorials(SpriteButton* overlay){preFightTutorials = overlay;}
  void PositionNodes();

  void CheckButtonsOnClick(int mouseX, int mouseY) override;
  void CheckButtonsHover(int mouseX, int mouseY) override;

  void LoadScene() override;
  void StartAdventure(int n);

  ~AdventuresMenu() override;
};

void PostFightAnim(std::vector<UIElement*>& elements, std::vector<Character*> winners, GameMats matsgot);

#endif