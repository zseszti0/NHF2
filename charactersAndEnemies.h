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
