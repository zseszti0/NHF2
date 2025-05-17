/*
The combat system’s foundation relies on the Mob abstraction, which encapsulates shared behaviors of both player-controlled characters and AI enemies. This approach enforces clean inheritance and enables scalable combat logic.
Overarching Design Considerations:
•	All combatants inherit from Mob, allowing them to maintain:
o	Core battle-related stats,
o	A mechanism for dynamic changes during battle (like buffs),
o	A visual representation for both gameplay and narrative menus,
o	A robust system for stat restoration and level progression.
•	Stat Separation: Combat depends on easily changable current stats (currentStats) derived from persistent base stats (baseStats). This duality makes stat manipulation (buffs, debuffs, damage) clean and reversible.
•	Buff System: A time-bound system for stat modifications that expire and revert properly, supporting deep turn-based mechanics.
•	Smart Visual Design: Every Mob includes a sprite loaded via a name-based asset pathing convention, simplifying visual logic across both enemies and player characters.
•	Save-Compatibility: Characters are loaded and saved using helper functions (see filesAndSaving.h) that persist levels, stats, and progress across sessions.

*/
#ifndef COMBAT_H
#define COMBAT_H

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <SDL2/SDL.h>


#include "UIAndDisplay.h"

class Mob;
class Character;
class Enemy;


enum CardTypes {
    Buster, Arts, Quick
};

struct TypeMultiplier {
    CardTypes type;
    double DMG, ER, SG;

    TypeMultiplier(CardTypes cardType) {
        type = cardType;
        switch (type) {
            case Buster:
                DMG = 1.5;
                ER = 0.15;
                SG = 0.3;
            break;
            case Arts:
                DMG = 1;
                ER = 1;
                SG = 0.1;
            break;
            case Quick:
                DMG = 0.9;
                ER = 0.5;
                SG = 1;
            break;
        }
    }
};

struct Buff {
        std::string type;
        double amount;
        int turnsLeft;
};
struct Dmg {
    double amount ;
    bool isCrit;
    int starsGenerated;

    Dmg() {amount = 0; isCrit = false; starsGenerated = 0;}
    Dmg(double dmg_dealt, bool is_crit, int stars_gend) : amount(dmg_dealt), isCrit(is_crit), starsGenerated(stars_gend) {};
};
struct SkillStruct {
    std::function<void(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself)> effect;
    int cooldown;
    int activeCooldown;
};

/*
Purpose: Serves as the base class for all entities that participate in battle: both Character and Enemy. It provides shared mechanisms for stat handling, damage, buffs, leveling, and visuals.

elper Structs/Enums:
•	struct Buff {
std::string type;
double amount;
int turnsLeft;
};
•	struct Dmg {
double amount ;
bool isCrit;
int starsGenerated;
};

Key Members:
•	std::string name
•	Sprite¬¬¬¬* sprite
•	various doubles baseStats;
•	various doubles currentStats;
•	std::vector<Buff> buffs
•	bool isDead
•	int level

Core Functions:
•	void SetHp(double amount)
Reduces HP; if HP drops to 0 or below, sets isDead = true.
•	void ResetStats()
Resets currentStats to baseStats; called at start of combat or post-buff cleanup.
•	double& GetStat(const std::string& statName)
Provides direct mutable reference to a specific stat in currentStats. Useful for buffs/debuffs.
•	void AddBuff(Buff buff)
Adds a new temporary buff to the Mob.
•	void EraseBuff()
Iterates over buffs and removes them if expired, restoring original stat values.
•	void LevelUp()
Increases level and uses a logarithmic formula to scale base stats accordingly.

Design Considerations
•	The use of GetStat() returning a double& is key to making buff and damage logic clean, avoiding unnecessary duplication or boilerplate.
•	Buffs being managed as a vector allows stacking and turn-based expiration.
•	The sprite is embedded directly in Mob, which simplifies loading visuals using the name as a filepath component.

*/
class Mob {
protected:
    std::string name;
    Sprite* sprite;
    int level;

    double atk,hp,ER,speed;
    double currentAtk, currentHp;

    double energy;
    double ultCost;
    bool isAoe;

    std::vector<Buff> buffs;
    std::vector<UIElement*> buffIcons;

    bool isDead = false;

    double AV;
public:


    Mob(const char* name, double atk, double hp, double ER, double s, double ultCost, int l, bool isAoe = false)
        :name(name), level(l), atk(atk), hp(hp), ER(ER), speed(s),ultCost(ultCost) ,isAoe(isAoe)
    {
        sprite = nullptr;

        currentAtk = atk;
        currentHp = hp;
        energy = 0;

        AV = 10000/speed;
    }

    virtual void ResetStats() = 0;
    void GiveBuff(Buff buff, Mob& target);
    virtual void GetBuff(Buff buff) = 0;
    virtual void EraseBuff(Buff buff) = 0;
    virtual void AddBuffIcon(std::string buff) = 0;
    virtual void EraseBuffIcon() = 0;
    virtual void CountdownTurn();
    virtual void SetHP(double amount);
    void Death();

    double GetSpeed(){return speed;}
    bool GetIsDead(){return isDead;}
    void SetIsDead(bool iD){isDead = iD;}

    double GetAv(){return AV;}
    void SetAv(double av){AV = av;}
    void UpdateAv(){AV = 10000/speed;}

    int GetLevel(){return level;}

    virtual bool IsAoe(){return isAoe;}

    std::string GetName(){return name;}
    Sprite*& GetSprite(){return sprite;}
    void Render();

    virtual Dmg Ult();

    virtual void LevelUp() = 0;

    virtual ~Mob() = default;
};

/*
Purpose: Represents the player’s controllable units in combat. Supports complex mechanics including skills, ultimates, visual variants, leveling, and persistence via file I/O.

Key Members:
•	Various extra doubles added for character specific base nd current stats.
•	SDL_Color color - Unique color for this character, used in visual indicators and UI elements.
•	SkillStruct skill – skill with costumized lambdas and a cooldown machnic.

Helper Structs/Enums:
•	struct SkillSturct {
std::function<void(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself)> effect;
int cooldown;
int activeCooldown;
};
•	std::function<float(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself)> ult;

Core Methods:
•	void CountdownTurn()
Called each turn to reduce cooldown and check buffs.
•	Dmg Normal(…)
Executes one of three attack styles using CardType(Buster, Arts, Quick) enum. Each Normal type has slightly different abilities, defined by the TypeMultipler struct.
•	void Skill(…)
Executes the skill's effect if cooldown is ready.
•	Dmg Ult(…)
Executes the ultimate ability, if energy level is enough.

Design Considerations:
•	Sprites makes it trivial to swap a character’s visual depending on game context (menu, combat, overview, etc.).
•	Skills being cooldown-based encourages planning and cooldown management.
•	Skills and ultimates use std::function, allowing for flexible behavior without inheritance bloat.
•	Color is an efficient visual cue to identify characters during combat and transitions.
•	Characters are loaded from file via LoadCharactersFromFile() and saved via SaveCharactersToFile() (see filesAndSaving.h), preserving progress like levels and cooldown states.

*/
class Character : public Mob {
    double starGen,critDmg,starAbsorbB,starAbsorbA,starAbsorbQ;
    double currentER,currentStarGen,currentSpeed,currentCritDmg,currentStarAbsorbB,currentStarAbsorbA,currentStarAbsorbQ;

    bool isCurrentlyAoe;

    SkillStruct skill;
    std::function<float(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself)> ult;

public:
    const bool skillNeedsTarget;
    const bool ultIsAoe;
    const SDL_Color color;



    Character(const char* n,std::vector<double> numberStats, SDL_Color color = SDL_Color(255,107,220),
        bool isNormalAOE = false, bool snt = false, bool isUltAoe = false, std::function<void(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself)> skillFunc = [](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself){},std::function<float(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself)> ultFunc = [](std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself){return 0.0f;})
            :Mob(n,numberStats.at(0),numberStats.at(1),numberStats.at(2),numberStats.at(4),numberStats.at(9),numberStats.at(10),isNormalAOE) ,
            starGen(numberStats.at(3)), critDmg(numberStats.at(5)), starAbsorbB(numberStats.at(6)), starAbsorbA(numberStats.at(7)), starAbsorbQ(numberStats.at(8)), ult(std::move(ultFunc)),skillNeedsTarget(snt), ultIsAoe(isUltAoe), color(color)
    {
        std::string spritePath = "./assets/characters/" + name;

        sprite = new Sprite(name.c_str(),(spritePath + "/sprite.png").c_str(),SDL_Rect{0,0,244,198});
        sprite->AddState("splash",(spritePath + "/splash.png").c_str());
        sprite->AddState("combat",(spritePath + "/combat_thumbnail.png").c_str());
        sprite->AddState("turnThumb",(spritePath + "/turnThumb.png").c_str());

        currentER = ER;
        currentStarGen = starGen;
        currentSpeed = speed;
        currentCritDmg = critDmg;
        currentStarAbsorbB = starAbsorbB;
        currentStarAbsorbA = starAbsorbA;
        currentStarAbsorbQ = starAbsorbQ;
        isCurrentlyAoe = isNormalAOE;

        skill.effect = std::move(skillFunc);
        skill.cooldown = 0;
        skill.activeCooldown = 0;

        isDead = false;

    }
    void AddSkillAndUlt(SkillStruct newSkill,
        std::function<float(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself)> ultFunc)
    {
        skill.effect = std::move(newSkill.effect);
        skill.cooldown = newSkill.cooldown;
        skill.activeCooldown = 0;
        ult = std::move(ultFunc);
    }


    void ResetStats() override;
    void GetBuff(Buff buff) override;
    void AddBuffIcon(std::string buff) override;
    void EraseBuff(Buff buff) override;
    void EraseBuffIcon() override;
    void ClearBuffs() {
        if (!buffIcons.empty()) {
            for (auto it = buffIcons.begin(); it != buffIcons.end(); ++it) {
                delete *it;
                it = buffIcons.erase(it);
            }
        }
        buffs.clear();
    }
    void CountdownTurn() override;

    Dmg Normal(TypeMultiplier type, Enemy* target, int starsGot);
    void Skill(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself);
    bool CanSkill(){return skill.activeCooldown == 0;}
    int SkillCooldownLeft(){return skill.activeCooldown;}
    Dmg Ult(std::vector<Enemy*>& targets,std::vector<Character*> allies, Character* herself);

    double& GetStat(const char* t);
    bool IsAoe() override {
        return isCurrentlyAoe;
    }
    float ThreatValue() {
        return (atk * (starGen * (starAbsorbB + starAbsorbA + starAbsorbQ )/3)/10 * critDmg)* 0.6 + hp;
    }

    void LevelUp() override;

    std::vector<UIElement*>& GetBuffIcons(){return buffIcons;}

    ~Character() override;
};

/*
Purpose: Represents AI-controlled enemies in combat. Unlike Characters, they are not player-interactive and have hardcoded attack types with minimal external influence.

Key Members:
•	Added def and critRate current and base stats.

Core Functions:
•	void Atk1/2/3(Character¬* target)
These methods execute set attack patterns on the given target.

Design Considerations:
•	Despite being simpler than Character, the inclusion of different attack types adds strategic depth and variety.

*/
class Enemy : public Mob{

    double def,critRate;
    double currentDef, currentCritRate;

    std::function<void(Mob* target)> skill = nullptr;

public:
    Enemy(const char* n, std::vector<double> stats, bool isNormalAOE = false, std::function<void(Mob* target)> skillFunc = nullptr)
        : Mob(n,stats.at(0),stats.at(1),stats.at(2),stats.at(3),stats.at(6),stats.at(7),isNormalAOE)
    {
        def = stats.at(4);
        currentDef = def;
        critRate = stats.at(5);
        currentCritRate = critRate;

        std::string spritePath = "./assets/sprites/" + name + ".png";
        sprite = new Sprite(name.c_str(),spritePath.c_str(),SDL_Rect{0,0,200,196});
        SDL_Rect srcRes;
        SDL_QueryTexture(sprite->GetTexture().get(),nullptr,nullptr,&srcRes.w,&srcRes.h);
        sprite->GetTransform().position.w = srcRes.w;
        sprite->GetTransform().position.h = srcRes.h;
        sprite->GetTransform().scrRect = {0,0,srcRes.w,srcRes.h};

        skill = std::move(skillFunc);
    }

    void ResetStats() override;
    void GetBuff(Buff buff) override;
    void EraseBuff(Buff buff) override;

    void SetHP(double amount) override;

    void AddBuffIcon(std::string buff) override;
    void EraseBuffIcon() override;

    double& GetStat(const char* t);

    void AddSkill(std::function<void(Mob* target)> s) {
        skill = std::move(s);
    }
    Dmg Atk1(Character* target);
    Dmg Atk2(Character* target);
    Dmg Atk3(Character* target);
    Dmg Ult() override;

    void Reset();
    void LevelUp() override;

    ~Enemy() override;

};


#endif
