#include "charactersAndEnemies.h"
#include "scene.h"

#include <utility>
#include <random>
#include <cmath>

///MOBS-----------------------
///
void Mob::GiveBuff(Buff buff, Mob& target) {
    target.GetBuff(std::move(buff));
}
void Mob::SetHP(double amount) {
    currentHp = currentHp + amount > hp ? hp : currentHp + amount;

    if(currentHp <= 0) {
        std::cout << "Bro died: " << name << std::endl;
        this->Death();
    }
}
void Mob::CountdownTurn() {
    int i = 0;
    for (auto it = buffs.begin(); it != buffs.end(); ) { //using it-s cause if multiple element gets deleted, it doesnt mess up the for loop.
        if (--(it->turnsLeft) == 0) { // Remove else if expired
            this->EraseBuff(*it);
            it = buffs.erase(it); // Erase returns the next valid iterator

            delete buffIcons[i];
            buffIcons.erase(buffIcons.begin()+i);
        } else
            ++it; // Only increment else if not erasing

        i++;
    }
}
void Mob::Render() {
    sprite->Render();
    for(auto buff : buffIcons) {
        buff->Render();
    }
}

//Skills
Dmg Mob::Ult() {
    std::cout << "ult" << std::endl;
    return {0,false,0};
}

void Mob::Death() {
    sprite->Tint(SDL_Color(92, 92, 92));
    isDead = true;
    energy = 0;

    for (auto it = buffIcons.begin(); it != buffIcons.end(); ) {
        delete *it;             // free the memory
        it = buffIcons.erase(it);    // remove from vector safely
    }
    buffIcons.clear();
    buffs.clear();
}



///CHARACTERS-----------------------
///

double& Character::GetStat(const char *t) {
    std::string type(t);
    if(type == "atk"){
        return currentAtk;
    }
    if(type == "baseatk"){
        return atk;
    }
    if(type == "hp"){
        return currentHp;
    }
    if(type == "maxhp"){
        return hp;
    }
    if(type == "ER"){
        return currentER;
    }
    if(type == "speed"){
        return speed;
    }
    if(type == "starGen"){
        return currentStarGen;
    }
    if(type == "critDmg"){
        return currentCritDmg;
    }
    if(type == "starAbsorbB"){
        return currentStarAbsorbB;
    }
    if(type == "starAbsorbA"){
        return currentStarAbsorbA;
    }
    if(type == "starAbsorbQ"){
        return currentStarAbsorbQ;
    }
    if(type == "energy") {
        return energy;
    }
    if(type == "ultCost") {
        return ultCost;
    }
    if(type == "AV") {
        return AV;
    }
    return atk;
}


void Character::ResetStats(){
    currentAtk = atk;
    currentHp = hp;
    energy = 0;

    currentER = ER;
    currentStarGen = starGen;
    currentSpeed = speed;
    currentCritDmg = critDmg;
    currentStarAbsorbB = starAbsorbB;
    currentStarAbsorbA = starAbsorbA;
    currentStarAbsorbQ = starAbsorbQ;
    isCurrentlyAoe = isAoe;

    skill.activeCooldown = 0;

    isDead = false;

    if (buffIcons.empty())  return;
    for (auto it = buffIcons.begin(); it != buffIcons.end(); ) {
        delete *it;
        it = buffIcons.erase(it);
    }
    buffIcons.clear();
    buffs.clear();
}
void Character::GetBuff(Buff buff) {
    if(buff.type == "atk"){
        currentAtk += buff.amount;
    }
    else if(buff.type == "hp"){
        currentHp += buff.amount;
    }
    else if(buff.type == "ER"){
        currentER += buff.amount;
    }
    else if(buff.type == "speed"){
        currentSpeed += buff.amount;
    }
    else if(buff.type == "starGen"){
        currentStarGen += buff.amount;
    }
    else if(buff.type == "critDmg"){
        currentCritDmg += buff.amount;
    }
    else if(buff.type == "starAbsorbB"){
        double temp = currentStarAbsorbB;
        currentStarAbsorbB = currentStarAbsorbB + buff.amount >= 1 ? 1 : currentStarAbsorbB + buff.amount;
        buff.amount = currentStarAbsorbB - temp;
    }
    else if(buff.type == "starAbsorbA"){
        double temp = currentStarAbsorbA;
        currentStarAbsorbA = currentStarAbsorbA + buff.amount >= 1 ? 1 : currentStarAbsorbA + buff.amount;
        buff.amount = currentStarAbsorbA - temp;
    }
    else if(buff.type == "starAbsorbQ"){
        double temp = currentStarAbsorbQ;
        currentStarAbsorbQ = currentStarAbsorbQ + buff.amount >= 1 ? 1 : currentStarAbsorbQ + buff.amount;
        buff.amount = currentStarAbsorbQ - temp;
    }
    else if(buff.type == "aoe"){
        isCurrentlyAoe = buff.amount == 1;
    }
    else if(buff.type == "energy") {
        energy = energy + buff.amount >= ultCost ? ultCost : energy + buff.amount;
    }
    else {
        std::cout << "Invalid buff type" << std::endl;
        return;
    }

    std::cout << "!!BUFF GOT for: "<< name << " type: " << buff.type << " amount: " << buff.amount << std::endl;

    if(buff.type != "aoe" && buff.type != "energy") {
        buffs.push_back(buff); //only add the buff else if its valid;
        AddBuffIcon(buff.type);
    }
}
void Character::EraseBuff(Buff buff)  {
    if(buff.type == "atk"){
        currentAtk -= buff.amount;
    }
    else if(buff.type == "hp"){
        currentHp -= buff.amount;
    }
    else if(buff.type == "ER"){
        currentER -= buff.amount;
    }
    else if(buff.type == "speed"){
        currentSpeed -= buff.amount;
    }
    else if(buff.type == "starGen"){
        currentStarGen -= buff.amount;
    }
    else if(buff.type == "critDmg"){
        currentCritDmg -= buff.amount;
    }
    else if(buff.type == "starAbsorbB"){
        currentStarAbsorbB -= buff.amount;
    }
    else if(buff.type == "starAbsorbA"){
        currentStarAbsorbA -= buff.amount;
    }
    else if(buff.type == "starAbsorbQ"){
        currentStarAbsorbQ -= buff.amount;
    }
    else if(buff.type == "aoe"){
        for (const auto& aoebuff: buffs) {
            if (aoebuff.type == "aoe") return;
        }
        isCurrentlyAoe = false;
    }
}
void Character::AddBuffIcon(std::string buff) {
    std::string path = "./assets/combat/" + buff + "Buff.png";
    int x = sprite->GetTransform().position.x+165 + (buffIcons.size())*24;
    auto* buffIcon = new UIElement(buff.c_str(), path.c_str(),SDL_Rect{x,835,20,19});
    buffIcons.push_back(buffIcon);
}
void Character::EraseBuffIcon() {

}
void Character::CountdownTurn() {
    if (skill.activeCooldown  != 0) skill.activeCooldown--;

    int i = 0;
    for (auto it = buffs.begin(); it != buffs.end(); ) { //using it-s cause if multiple element gets deleted, it doesnt mess up the for loop.
        if (--(it->turnsLeft) == 0) { // Remove else if expired
            this->EraseBuff(*it);
            it = buffs.erase(it); // Erase returns the next valid iterator

            delete buffIcons[i];
            buffIcons.erase(buffIcons.begin()+i);
        } else {
            ++it; // Only increment else if not erasing
            i++;
        }
    }
}


//Skills
Dmg Character::Normal(TypeMultiplier type, Enemy* target, int starsGot) {
    ///IS crit??---------------------------------------
    ///

    // Random number generator
    static std::random_device rd;   // Non-deterministic random seed
    static std::mt19937 gen(rd());  // Mersenne Twister RNG
    std::uniform_real_distribution<double> dist(0.0, 1.0); // Range [0, 1]
    double roll = dist(gen);  // Get a random number between 0 and 1

    bool isCrit = roll < starsGot * 0.10; // else if roll is less than critChance(=stars got * 5%), it's a crit


    ///Stats got + dmg calc-------------------------------
    ///

    //lets do +/- 10% dmg variation for some randomness in low/high dmg rolls, cos why not
    // Random number generator
    static std::random_device rd2;   // Non-deterministic random seed
    static std::mt19937 gen2(rd2());  // Mersenne Twister RNG
    std::uniform_real_distribution<double> dist2(0.9, 1.1); // Range [90%-110%]
    double roll2 = dist2(gen2);  // Get the random +/- 10%

    double dmgDealt = currentAtk * type.DMG * roll2;
    dmgDealt = isCrit ? dmgDealt *  critDmg: dmgDealt;
    double energyGot = type.ER * currentER;
    int starsGenerated = round(type.SG * currentStarGen);

    energy = energyGot + energy >= ultCost ? ultCost : energyGot + energy;

    target->SetHP(-dmgDealt);
    std::cout << type.type << " type normal, isCrit: " << isCrit << " final dmg: " << dmgDealt << std::endl;
    std::cout << "energy got: " <<energyGot << std::endl;
    std::cout << "stars got: " <<starsGenerated << std::endl;
    std::cout <<std::endl;

    return Dmg{dmgDealt,isCrit,starsGenerated};
}
void Character::Skill(std::vector<Enemy*> targets,std::vector<Character*> allies, Character* herself) {
    if (skill.activeCooldown  == 0) {
        skill.effect(targets,allies,herself);
        skill.activeCooldown = skill.cooldown;
        std::cout << "Character skill hihi" << std::endl;
    }
}
Dmg Character::Ult(std::vector<Enemy*>& targets,std::vector<Character*> allies, Character* herself) {
    float multiplier = ult(targets,allies,herself);
    Dmg damageDealt = {currentAtk * multiplier,false,0};

    for(auto& enemy :targets) {
        enemy->SetHP(-damageDealt.amount);
    }

    energy = 0;
    return damageDealt;
}

void Character::LevelUp() {
    if(level < 10) {
        if(level != 1) {
            double stat = atk;
            double base = stat/log(tgamma(level+1));

            stat = base*log(tgamma(level+2));
            atk = (int)stat;

            stat = hp;
            base = stat/log(tgamma(level+1));

            stat = base*log(tgamma(level+2));
            hp = (int)stat;

            level++;
        } else {
            double stat = atk * log(2);
            atk = (int)stat;

            stat = hp * log(2);
            hp = (int)stat;

            level++;
        }
    }
}

Character::~Character() {
    if (!buffIcons.empty()) {
        for (auto it = buffIcons.begin(); it != buffIcons.end(); ) {
            delete *it;             // free the memory
            it = buffIcons.erase(it);    // remove from vector safely
        }
    }
    delete sprite;
}




///ENEMIES-----------------------
///
void Enemy::ResetStats(){
    currentAtk = atk;
    currentHp = hp;
    energy = 0;

    currentDef = def;
    currentCritRate = critRate;
}
void Enemy::GetBuff(Buff buff){
    if(buff.type == "atk"){
        currentAtk += buff.amount;
    }
    else if(buff.type == "hp"){
        currentHp += buff.amount;
    }
    else if(buff.type == "def"){
        currentDef += buff.amount;
    }
    else if(buff.type == "critRate"){
        currentCritRate += buff.amount;
    }
    else
        std::cout << "Invalid buff type" << std::endl;
    buffs.push_back(buff); //only add the buff else if its valid;
}
void Enemy::EraseBuff(Buff buff) {
    if(buff.type == "atk"){
        currentAtk -= buff.amount;
    }
    else if(buff.type == "hp"){
        currentHp -= buff.amount;
    }
    else if(buff.type == "def"){
        currentDef -= buff.amount;
    }
    else if(buff.type == "critRate"){
        currentCritRate -= buff.amount;
    }
    else
        std::cout << "Invalid buff type" << std::endl;
}

double& Enemy::GetStat(const char *t) {
    std::string type(t);
    if(type == "atk"){
        return currentAtk;
    }
    if(type == "baseatk"){
        return atk;
    }
    if(type == "hp"){
        return currentHp;
    }
    if(type == "maxhp"){
        return hp;
    }
    if(type == "def"){
        return currentDef;
    }
    if(type == "basedef"){
        return def;
    }
    if(type == "critRate"){
        return currentCritRate;
    }
    if(type == "speed"){
        return speed;
    }
    if(type == "energy") {
        return energy;
    }
    if(type == "ultCost") {
        return ultCost;
    }
    if(type == "AV") {
        return AV;
    }
    return atk;
}


//Skills
Dmg Enemy::Atk1(Character* target) {
    static std::random_device rd;   // Non-deterministic random seed
    static std::mt19937 gen(rd());  // Mersenne Twister RNG
    std::uniform_real_distribution<double> dist(0.7, 1.2); //
    double atkMulti = dist(gen);

    double dmg = atk * atkMulti;

    static std::random_device rd2;  // Non-deterministic random seed
    static std::mt19937 gen2(rd2());  // Mersenne Twister RNG
    std::uniform_real_distribution<double> dist2(0.0, 1.0);
    double roll = dist2(gen2);
    bool isCrit = roll < critRate;

    dmg = isCrit ? dmg * 1.5 : dmg;

    target->SetHP(-dmg);
    return Dmg{dmg,isCrit,1};
}
Dmg Enemy::Atk2(Character* target) {
    static std::random_device rd;   // Non-deterministic random seed
    static std::mt19937 gen(rd());  // Mersenne Twister RNG
    std::uniform_real_distribution<double> dist(1, 1.3); //
    double atkMulti = dist(gen);

    double dmg = atk * atkMulti;

    static std::random_device rd2;  // Non-deterministic random seed
    static std::mt19937 gen2(rd2());  // Mersenne Twister RNG
    std::uniform_real_distribution<double> dist2(0.0, 1.0);
    double roll = dist2(gen2);
    bool isCrit = roll < critRate;

    dmg = isCrit ? dmg * 1.7 : dmg;

    target->SetHP(-dmg);
    return Dmg{dmg,isCrit,1};
}
Dmg Enemy::Atk3(Character* target) {
    if(skill != nullptr) {
        skill(target);
        return Dmg{0,false,0};
    }
    return Atk1(target);
}
Dmg Enemy::Ult() {
return {0,false,0};
}

void Enemy::Reset() {
    this->ResetStats();
    sprite->UnTint();
    isDead = false;
}
void Enemy::SetHP(double amount) {
    amount = (1-def)*amount;
    currentHp = currentHp + amount > hp ? hp : currentHp + amount;

    if(currentHp <= 0) {
        std::cout << "Bro died: " << name << std::endl;
        this->Death();
    }
}

void Enemy::LevelUp() {
    if(level != 1) {
        double stat = atk;
        double base = stat/log(log(tgamma(level+1)));
        std::cout << "base: " << base << std::endl;

        stat = base*log(log(tgamma(level+2)));
        atk = stat;

        stat = hp;
        base = stat/log(tgamma(level+1));

        stat = base*log(tgamma(level+2));
        hp = stat;


        speed += log((level+1)*1.5);
        level++;

    } else {
        double stat = atk * log(log(2));
        atk = stat;

        stat = hp * log(2);
        hp = stat;

        level++;
    }
    std::cout << "ENEMY STAT level: " << level <<" atk hp def speed: " << atk << " " << hp << " " << def <<" " << speed << std::endl;
}
void Enemy::AddBuffIcon(std::string buff) {

}
void Enemy::EraseBuffIcon() {

}
Enemy::~Enemy() {
    delete sprite;
}







