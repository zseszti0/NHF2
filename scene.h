/*This header defines the Scene base class and its derived subclasses that represent different UI/gameplay screens in the project.
The scene system is inspired by Unity’s scene and LoadScene concept, providing a framework for managing UI bundles, rendering, input handling, and scene switching in a modular and extensible way.

Overarching Design Considerations:
•	Scene manages a collection of UI elements (grouped in UIBundle) and controls rendering, input, and updates per frame.

•	Scenes encapsulate distinct screens or modes such as menus, character overlays, fight preparation, actual fights, and adventure node selection.
•	A shared static SDL renderer is used for all UI rendering, ensuring consistency and efficiency.

•	Scene switching is handled via a global pointer to the active scene; on switching, the new scene’s Init() method sets up required state.

•	Input handling, especially button clicks, used for button onClick callbacks are extensively used for flexibility. However, some scene pointers or UI elements needed inside lambdas are not available at compile time. To workaround this, some buttons use empty lambdas initially. Scenes override CheckButtonsOnClick() to detect clicks manually and trigger the required actions. This approach maintains clean separation of concerns and avoids messy global state or premature captures.

•	Static UI bundles and initialization. Most UI elements (textures, positions) are initialized once in main(). Static bundles hold frequently reused UI components shared across scenes. This avoids repeated loading and setup, improving performance and consistency. Passing UI elements explicitly between classes would be messier, so static bundles act as centralized shared resources.
*/

#ifndef SCENE_H
#define SCENE_H


class Scene;
class ListingScene;
class Fight;
class PreFight;

#include "UIAndDisplay.h"
#include "charactersAndEnemies.h"
#include "adventure.h"
#include "gameEngine.h"

#include <utility>
/*
Purpose: The abstract base class representing a generic scene/screen.

Key members:
•	std::string name;
•	SDL_Texture* bg;
•	std::vector<UIElement*> uiBundle;
•	Scene::static SDL_Renderer* renderer

Core functions:
•	Scene(const std::string& name, std::shared_ptr<SDL_Texture> background)
Constructor initializes scene name and background texture.
•	virtual void Init()
Virtual method called after scene switching to initialize scene-specific data or UI layout. Override in derived classes for custom setup.
•	virtual void LoadScene()
Called every update cycle to render all UI elements in the scene’s bundle. Base implementation renders all elements; override for extra rendering steps.
•	virtual void RenderOne()
Used to render UI elements that must be on top (e.g., overlays).
•	virtual void UpdateTweens(float deltaTime)
Updates all animations/tweening of UI elements based on delta time. Ensures smooth animation updates every frame.
•	virtual void CheckButtonsOnClick/Hover(int mouseX, int mouseY)
Cycles thourgh the ui boundle, and calls each Button’s HandleEvent functions. Overriden in some cases, for more complex button event handling.

Design Considerations:
•	Static Renderer: The use of a shared static renderer ensures all scenes and UI elements render to the same SDL window context.
•	Virtual methods:  Allow each scene to customize rendering and input logic as needed.
•	UIBundles are used to organize elements hierarchically, mirroring Unity’s GameObject hierarchy.

*/
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
  virtual void TutorialsMenuNext();

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

/*
Purpose: Used to display a list of selectable options, primarily designed for the tutorials menu.

Key Members:
•	std::vector<Button*> triggers;
•	std::vector<std::vector<UIElement*>> menus;

Core functions:
•	void Init() override
Custom initialization for ListingScene, resetting or preparing the menu display.
•	void ChangeMenu(int index/ const char* name)
Switches the displayed menu to the one at the index, or the trigger buttons name. Called by the triggers’ lambdas when a button is clicked.

Design Considerations:
•	Enables simple menu selection with a clear separation of trigger buttons and content.
•	Designed for menus with relatively static option sets and content.

*/
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
    for(size_t i = 0; i < newTriggers.size(); i++) {
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
    for(size_t i = 0; i < triggers.size(); i++) {
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

/*
Purpose: Specialized LsitingScene to display character stats and skills in an overview format.

Key Members:
•	Character* currentCharacter;
•	 std::vector<Text*> statTexts;

Core functions:
•	void Init() override
Prepares the overlay by resetting and updating displayed stats.

•	void UpdateCharacterDisplay(Character* newCharacter)
Updates text and UI elements to reflect the selected character’s stats. Instead of creating separate bundles per character, dynamically updates existing UI elements for efficiency.

Design Considerations:
•	Separate class: Inherits ListingScene’s trigger/menu switching functionality but adds character-specific dynamic content.
•	Dynamic text: Avoids excessive UI bundle creation by updating text elements in place.
•	Keeps a reference to the currently selected character for consistent visual updates.

 */
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

/*
Overview
The Fight class is a central component of the game's combat system, inheriting from the Scene class. It encapsulates the turn-based combat mechanics, managing both the visual representation and the underlying logic of battles. The class orchestrates the flow of combat, handling player and enemy actions, updating the UI accordingly, and ensuring a cohesive gaming experience.
Fight : Scene
Key Members:
•	UI Containers
Purpose: These containers group related UI elements, facilitating organized rendering and updates, easier access to frequently used elements, lessening the need to cycle through each ui boundle element
o	std::vector<std::vector<UIElement*>> combatChConts: Container for character-related UI elements.
o	std::vector<UIElement*> combatNATypeCont: Container for normal attack type indicators.
o	std::vector<UIElement*> animProps: Container for animation properties and effects.
o	std::vector<UIElement*> turnOrderCont: Container for turn indicators.
o	UIElement* combatTargetIndicator – visual element for current enemy target

•	Turned based logic elements
Purpose: These elements are used to make it easier to implement the turned based logic, giving feedback of the current battle state to the user.
o	std::vector<Mob*> turnOrderFIFO: A queue representing the order of upcoming actions.
o	Character* currentlyActing: Pointer to the character currently taking action.
o	Enemy* currentlyActingEnemy: Pointer to the enemy currently taking action.
o	CombatState combatState: Helper enum:
enum CombatState {
•	ENEMYACTION,
•	WAITINGFORACTION,
•	WAITINGFORTYPE,
•	WAITINGFORTARGET,
•	OVER,
•	WAITINGFORENEMYANIM
};
Used in main.cpp’s update loop, for a state machine-like input handling.
o	double AV:  A double value tracking the action value, used to calculate action order via speed.

Core methods:
•	Initialization and Setup
o	void Init()
Overrides the base Scene initialization to set up combat-specific elements.
o	void CalculateNext()
Calculates the upcoming actions based on character and enemy speed stats, populating the turnOrderFIFO.
o	void CountNextAction()
Processes the next action in the queue, updating the currentlyActing or currentlyActingEnemy pointers and setting the appropriate combatState.
•	Damage, Character and Enemy action deligating
o	void Normal/Skill/Ult()
Calculates the target of the currentlyActing character, and calls their respective attack pattern functions. Displays the got damage and it’s properties to the screen.
o	void EnemyAtk/EnemyAction()
Calculates the enemies target, and implements the AI logic of enemy actions, while displaying it’s corresponding visuals.
o	void AfterActionCleanup /CheckIfOver/End()
Processes the visuals, logic after an enemy or character action. Checks if the game is over, updating the combat state and visuals accordingly.
Design Considerations
•	UI Containers: Inspired by Unity's empty GameObjects, these containers provide a structured way to manage related UI elements, simplifying updates and rendering.
•	Turn Order Queue: Maintaining a FIFO queue of upcoming actions allows for dynamic turn order calculations based on character stats, adding strategic depth to combat.
•	Combat States: Using an enumeration to represent combat states enables a clear and manageable flow of combat phases, facilitating input handling and state transitions.
•	Visual Feedback: A lot of helper functions dedicated to visual updates enhance player engagement by providing immediate and intuitive feedback on actions and state changes.
•	Input Handling: Centralizing input processing within the Fight class ensures that combat-specific inputs are handled appropriately, maintaining separation of concerns.
________________________________________
Final Notes
The Fight class serves as the backbone of the game's combat system, integrating UI management, turn-based logic, and player interaction into a cohesive module. Its design reflects a balance between functionality and maintainability, drawing inspiration from established game development practices to deliver an engaging combat experience.
*/
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

    FightNodeStars levelStars = {false,false,false};
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

    FightNodeStars GetLevelStars(){return levelStars;}

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
    void SetSkillButtonVisuals();

    //anims
    void UpdateTweens(float dt) override;
    void InitAnimPropsStartingState();
    void TriggerSkillAnim();
    bool AreAnimsOver();


    ~Fight() override {
        SDL_DestroyTexture(bg);

        for (auto it = enemyHps.begin(); it != enemyHps.end(); ) {
                delete *it;             // free the memory
                it = enemyHps.erase(it);    // remove from vector safely
        }

        std::cout << "Fight: " << name << " dtor" <<std::endl;
    }

};

/*
Purpose: Scene for selecting your party members before entering battle.

Key Members:
•	UIBundle charSlotCont — container holding UI elements representing all available characters.
•	std::vector<Character*> selectedCharacters — tracks currently selected party members (max 3).

Core functions:
•	void Init() override
Sets up character slots and selection state.
•	void AddCharacter(Character* newCharacter)
Adds or removes a character from the party by index, enforcing max party size of 3.
•	void StartFight()
Instantiates a new Fight scene (Fight*) with the selected party (of 3) and preloaded enemies (from file) and starts the battle.

Design Considerations:
•	Allows dynamic party composition with simple toggling mechanics.
•	Enforces party size constraints programmatically, adding intuitive visuals.

*/
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

/*
Purpose: Scene displaying a map of fight nodes representing different battle encounters.

Key Members:
•	std::vector<FightNode*> fightNodes — the nodes representing battles, loaded from external data files on startup.
•	Fight nodes contain:
o	Enemy data from file
o	Pointer to their corresponding PreFight scene, due to consistent enemies, battle setup in the same node.
o	Attempt count and star ratings for tracking player progress.
o	Located in adventure.h

Core functions:
•	void Init() override
Loads fight nodes and sets up the UI for node selection.
•	void UpdateLastNode(FightNode* node, int stars, bool attempted)
Updates node data and saves progress using FightNode’s WriteNodeDataToFile().
•	bool CheckButtonsOnClick(int mouseX, int mouseY) override
Manages button clicks to select nodes and start fights, again with manual click handling due to runtime pointer capture limitations.

Design Considerations:
•	Acts as a container and controller for progression through fights.
•	Loads and saves persistent data externally to keep node state consistent across runs.
•	Uses static UI bundles shared across scenes for efficiency.

*/
class AdventuresMenu : public Scene {
  std::vector<FightNode*> nodes;

  SpriteButton* preFightTutorials = nullptr;
  int lastAttempted = 0;
public:
  AdventuresMenu(const char* n, const char* texturePath) : Scene(n,texturePath){};
  void AddNode(FightNode* node) {
    nodes.push_back(node);
  }
  void InitPreFights();
  void AddPreFightTutorials(SpriteButton* overlay){preFightTutorials = overlay;}
  void PositionNodes();
  void TutorialsMenuNext() override;

  void Init() override;

  void CheckButtonsOnClick(int mouseX, int mouseY) override;
  void CheckButtonsHover(int mouseX, int mouseY) override;

  void UpdateLastNode(FightNodeStars starsGot);

  void LoadScene() override;

  void WriteNodeDataToFile();

  ~AdventuresMenu() override;
};

void PostFightAnim(std::vector<UIElement*>& elements, std::vector<Character*> winners, GameMats matsgot,FightNodeStars starsGot);

#endif