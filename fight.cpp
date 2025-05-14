#include "UIAndDisplay.h"
#include "scene.h"
#include "charactersAndEnemies.h"

#include <cmath>
#include <string>
#include <random>
#include <algorithm>


//FIGHT SCENE
void Fight::LoadScene() {
    //get the renderer size for full screen background.
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    SDL_Rect destposition = {0, 0, windowWidth, windowHeight};

    SDL_RenderCopy(renderer, bg, nullptr, &destposition); // Render the background

    //color the spriteBoxes
    this->TintGreyUIElement(characters.at(0)->color,"spriteCont1Layer2");
    this->TintGreyUIElement(characters.at(1)->color,"spriteCont2Layer2");
    this->TintGreyUIElement(characters.at(2)->color,"spriteCont3Layer2");
    this->TintGreyUIElement(characters.at(0)->color,"ult1");
    this->TintGreyUIElement(characters.at(1)->color,"ult2");
    this->TintGreyUIElement(characters.at(2)->color,"ult3");

    this->RenderOne("spriteCont1Layer2");
    this->RenderOne("spriteCont2Layer2");
    this->RenderOne("spriteCont3Layer2");

    for(auto& enemy: enemies) {
        enemy->Render();
    }
    for(auto& ch : characters) {
        ch->Render();
    }


    for(auto& element: uiBoundle) {
        if(element->GetName() != "spriteCont1Layer2"
            && element->GetName() != "spriteCont2Layer2"
            && element->GetName() != "spriteCont3Layer2")
            element->Render();
    }

}
void Fight::UpdateTweens(float dt) {
    for(auto& element : uiBoundle)
        element->UpdateTweens(dt);
    for(auto& element : fieldMobs)
        element->GetSprite()->UpdateTweens(dt);
}

void Fight::AddEnemy(Enemy* e) {
    enemies.push_back(e);

    Bar* enemyHp = new Bar("enemyHp","./assets/combat/enemyHpBg.png","./assets/combat/enemyHp.png",SDL_Rect{0,0},e->GetStat("maxhp"),&e->GetStat("hp"));
    enemyHps.push_back(enemyHp);
    uiBoundle.push_back(enemyHp);

    fieldMobs.push_back(e);
}
void Fight::AddUIConts(std::vector<UIElement *>& ch1, std::vector<UIElement *>& ch2, std::vector<UIElement *>& ch3, std::vector<UIElement *>& natypes, UIElement* target,std::vector<UIElement*>& turnO) {
    combatChConts = {ch1,ch2,ch3};
    std::cout <<"done1" << std::endl;

    for(int i = 0; i < 3; i++) {
        combatChConts.at(i).push_back(characters.at(i)->GetSprite());
        dynamic_cast<Bar*>(combatChConts.at(i).at(2))->SetCur(&characters.at(i)->GetStat("energy"));
        dynamic_cast<Bar*>(combatChConts.at(i).at(2))->SetMax(characters.at(i)->GetStat("ultCost"));
        dynamic_cast<Bar*>(combatChConts.at(i).at(3))->SetCur(&characters.at(i)->GetStat("hp"));
        dynamic_cast<Bar*>(combatChConts.at(i).at(3))->SetMax(characters.at(i)->GetStat("maxhp"));
    std::cout <<"done" << std::endl;
    }
    combatNATypeCont = natypes;
    turnOrderCont = turnO;

    combatTargetIndicator = target;
    this->PositionTarget(0);
}

void Fight::ConfigureStartingState(){
    starCount = 0;
    skillPoints = 3;
    AV = 0;

    for(auto element: uiBoundle) {
        if(element->GetName() == "enemyDmgNumber") {
            element->GetTransform().opacity = 0.0f;
        }
    }


    fieldMobs.clear();
    int xpos = 143;
    for(auto& ch : characters) {
        ch->GetSprite()->ChangeState("combat");
        ch->GetSprite()->GetTransform().position = SDL_Rect{xpos,845,244,198};
        xpos+=397;
        ch->GetSprite()->GetTransform().scrRect = SDL_Rect{0,0,244,198};

        ch->UpdateAv();
        ch->ResetStats();
        fieldMobs.push_back(ch);
        ch->GetSprite()->SetVisible(true);
        ch->GetSprite()->UnTint();
    }

    std::vector<UIElement*> enemySprites;

    int totalWidth = 0;
    for(auto& enemy:enemies) {
        fieldMobs.push_back(enemy);
        enemy->UpdateAv();
        enemySprites.push_back(enemy->GetSprite());
        enemy->GetSprite()->GetTransform().position.y = 400;
        totalWidth += enemy->GetSprite()->GetTransform().position.w + 20;
        enemy->Reset();
    }
    totalWidth = totalWidth >= 1920 ? 1920 : totalWidth;
    totalWidth = totalWidth < 1000 ? 1000 : totalWidth;
    EQPosUIElements(SDL_Rect{(1920 - totalWidth)/2,400,totalWidth,301},enemySprites);
    for (size_t i = 0; i < enemies.size(); i++) {
        SDL_Rect barPos = enemies.at(i)->GetSprite()->GetTransform().position;
        int x = barPos.x + barPos.w/2 - 230/2;
        barPos.x = x;
        barPos.y -= 40;
        barPos.w = 230;
        barPos.h = 15;
        enemyHps.at(i)->GetTransform().position = barPos;
        enemyHps.at(i)->UpdateBar();
    }

    this->InitAnimPropsStartingState();

    currentlyActing = nullptr;
    turnOrderFIFO.clear();
    for(int i = 0; i < 5; i++)
        this->CalculateNext();

    std::cout << "calc Next after" << std::endl;
    this->CountNextAction();
    std::cout << "count Next after" << std::endl;
    this->SetSpVisuals();
    std::cout << "spvisual Next after" << std::endl;
    this->SetNaTypeChooser(false);
    std::cout << "natypechooser Next after" << std::endl;
    this->SetSkillNeedsTarget(false);
    std::cout << "skill needs visuals Next after" << std::endl;
    this->ChangeEnemyTarget(0);
    std::cout << "change target Next after" << std::endl;

}


void Fight::InitAnimPropsStartingState() {
    for(int i = 0; i < 3; i++) {
        std::shared_ptr<SDL_Texture> skillText = characters.at(i)->GetSprite()->GetTextureAt("base");
        animProps.at(i)->SetTexture(skillText);

        std::shared_ptr<SDL_Texture> ultText = characters.at(i)->GetSprite()->GetTextureAt("splash");
        animProps.at(3+i)->SetTexture(ultText);
    }
    for (auto& element : uiBoundle) {
        if (element->GetName() == "battleOver") {
            element->SetVisible(false);
        }
    }
}

void Fight::CountNextAction() {
    if(currentlyActing != nullptr)this->ScaleCharacterCont(false,currentlyActing);
    this->SetNaTypeChooser(false);
    std::cout << "inside count next natypechooser Next after" << std::endl;

    Mob* actingTemp = turnOrderFIFO.at(0);
    ChangeTurnOrderCont();
    turnOrderFIFO.erase(turnOrderFIFO.begin());
    while(turnOrderFIFO.size() <= 5) {
        CalculateNext();
    }

    std::cout << "next action: " <<actingTemp->GetName() <<" AV: " << 10000/(actingTemp->GetSpeed()) <<std::endl <<std::flush;

    auto actor = dynamic_cast<Character*>(actingTemp);
    if(actor) {
        currentlyActing = actor;

        currentlyActing->CountdownTurn();
        this->SetSkillButtonVisuals();
        this->TintGreyUIElement(actor->color,"NA");

        this->ScaleCharacterCont(true,currentlyActing);
        this->SetWhosTurnText(true);

        combatState = WAITINGFORACTION;
        std::cout << "CURRENTLY ACTING: " << currentlyActing->GetName() << std::endl;

    }
    else {
        combatState = ENEMYACTION;
        currentlyActingEnemy = dynamic_cast<Enemy*>(actingTemp);
    }

}
void Fight::CalculateNext() {
    double min = fieldMobs.at(0)->GetAv();
    for(auto& mob: fieldMobs) {
        if(mob->GetAv() < min)
            min = mob->GetAv();
    }

    for(auto& mob: fieldMobs) {
        if(mob->GetAv() == min) {
            turnOrderFIFO.push_back(mob);
            double newAV = mob->GetAv() + 10000/mob->GetSpeed();
            mob->SetAv(newAV);
            std::cout <<"pushed next: " << mob->GetName() << "with: " << newAV << std::endl;
        }
    }

    AV = min;
}

void Fight::EnemyAction() {

    this->TintGreyUIElement(SDL_Color{92, 92, 92},"skill");
    this->TintGreyUIElement(SDL_Color{92, 92, 92},"NA");

    this->SetWhosTurnText(false);
    std::cout << "---------ENEMIES TURN------------------------------------" << std::endl;


    SDL_RenderClear(renderer);
    this->LoadScene();
    SDL_RenderPresent(renderer);

    EnemyAtk(currentlyActingEnemy);
    ScaleUpEnemyCont();

    if (combatState != OVER)
        combatState = WAITINGFORENEMYANIM;
}


//ATKS AND STATS
/////////////
void Fight::Ult(int n) {
    auto* character = characters.at(n-1);
    if(character->GetIsDead()) return;
    if(character->GetStat("energy") >= character->GetStat("ultCost")) {


        std::vector<Character*> allies;
        for(auto& ch : characters) {
            if(!ch->GetIsDead()) allies.push_back(ch);
        }

        std::vector<Enemy*> targets;
        Dmg dmgDealt = {0,false,0};
        if(character->ultIsAoe) {
            for(auto& enemy : enemies) {
                if(!enemy->GetIsDead()) targets.push_back(enemy);
            }
            dmgDealt = character->Ult(targets,allies,character);
            dmgDealt.amount *= 3;
        }
        else {
            targets.push_back(enemies.at(currentEnemyTarget));
            dmgDealt = character->Ult(targets,allies,character);
        }

        if(dmgDealt.amount != 0)this->SetDmgText(dmgDealt,character->color);
        CombatUltAnim(animProps.at(2+n),animProps.at(6));
        this->AfterActionCleanup(true);
    }
}
void Fight::Skill(int n) {
    if(skillPoints <= 0) {
        std::cout << "not enough skillpoint" << std::endl;
        this->SetSkillNeedsTarget(false);
        combatState = WAITINGFORACTION;
        return;
    }
    if(characters.at(n-1)->GetIsDead()) {
        combatState = WAITINGFORTARGET;
        return;
    }


    skillPoints--;

    std::vector<Character*> allies;
    for(auto& ch : characters) {
        if(!ch->GetIsDead()) allies.push_back(ch);
    }

    std::vector<Enemy*> targets;
    for(auto& enemy : enemies) {
        if(!enemy->GetIsDead()) targets.push_back(enemy);
    }
    currentlyActing->Skill(targets,allies,characters.at(n-1));

    this->TriggerSkillAnim();
    this->SetSkillNeedsTarget(false);
    this->AfterActionCleanup(true);

    if(combatState != OVER)
        CountNextAction();

}
void Fight::Skill(Character* target) {
    if(skillPoints <= 0) {
        std::cout << "not enough skillpoint" << std::endl;
        this->SetSkillNeedsTarget(false);
        combatState = WAITINGFORACTION;
        return;
    }
    if(target->GetIsDead()) {
        combatState = WAITINGFORTARGET;
        return;
    }

    skillPoints--;
    std::vector<Character*> allies;
    for(auto& ch : characters) {
        if(!ch->GetIsDead()) allies.push_back(ch);
    }
    std::vector<Enemy*> targets;
    for(auto& enemy : enemies) {
        if(!enemy->GetIsDead()) targets.push_back(enemy);
    }
    currentlyActing->Skill(targets,allies,target);

    this->TriggerSkillAnim();
    this->SetSkillNeedsTarget(false);
    this->AfterActionCleanup(true);

    if(combatState != OVER)
        CountNextAction();
}
void Fight::Normal(TypeMultiplier type) {
    skillPoints = skillPoints + 1 >= 3 ? 3 : skillPoints + 1;

    //TEMP, NOT COUNTING AOE
    int starsToGive = this->CalculateStarsGot(type);
    starCount -= starsToGive;
    std::cout <<"Normal!! gave: " <<starsToGive <<" stars." << std::endl;

    Dmg dmgDealt;
    if(currentlyActing->IsAoe()) {
        for(auto enemy : enemies) {
            Dmg temp = currentlyActing->Normal(type,enemy,starsToGive);
            dmgDealt.amount += temp.amount;
            dmgDealt.isCrit = temp.isCrit ? true : dmgDealt.isCrit;
            dmgDealt.starsGenerated = temp.starsGenerated;
        }

    }
    else {
        dmgDealt = currentlyActing->Normal(type,enemies.at(currentEnemyTarget),starsToGive);
    }
    starCount += dmgDealt.starsGenerated;

    this->SetDmgText(dmgDealt,currentlyActing->color);
    this->AfterActionCleanup(true);

    if(combatState != OVER)
        CountNextAction();
}

void Fight::EnemyAtk(Enemy* enemy) {
    //preparations
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 3);

    //what skill the enemy does
    int atkPattern = dist(gen); // Will be 1, 2, or 3

    std::vector<Character*> potentialTargets;
    for(auto& ch : characters) {
        if(!ch->GetIsDead()) potentialTargets.push_back(ch);
    }
    Dmg dmgDealt;

    std::vector<Character*> targets;
    if (enemy->IsAoe()) {
        targets = potentialTargets;
    }
    else {
        //target using threat value (very cool)
        float totalThreat = 0;
        for(auto& target : potentialTargets) {
            totalThreat += target->ThreatValue();
        }

        static std::random_device rd2;   // Non-deterministic random seed
        static std::mt19937 gen2(rd2());  // Mersenne Twister RNG
        std::uniform_real_distribution<double> dist2(0.0, totalThreat); // Range [0, totalThreat]
        double roll = dist2(gen2);

        float sum = 0.0f;
        Character* target = potentialTargets.at(0);

        for(auto& tg : potentialTargets) {
            sum += tg->ThreatValue();
            if(roll <= sum) {
                target = tg;
                break;
            }
        }
        targets.push_back(target);
    }
    for (auto& target : targets) {
        switch(atkPattern) {
            case 1: {
                dmgDealt = enemy->Atk1(target);
                break;
            }
            case 2: {
                dmgDealt = enemy->Atk2(target);
                break;
            }
            case 3: {
                dmgDealt = enemy->Atk3(target);
                break;
            }
            default: dmgDealt = enemy->Atk1(target);
        }
        target->GetBuff({"energy",10,1});
        if(dmgDealt.starsGenerated)
            this->SetEnemyDmgText(dmgDealt,target->GetSprite()->GetTransform().position.x + 50);
    }
    //visuals, after
    std::cout << ":00 enemy dealt: " << dmgDealt.amount << " with type: " << atkPattern << std::endl << std::endl;

    this->AfterActionCleanup(false);
}

void Fight::AfterActionCleanup(bool wasMyAction) {
    if(wasMyAction) {
        this->SetSpVisuals();
        this->SetNaTypeChooser(false);
        this->SetCritStarText();

        for(auto enemyHp : enemyHps) {
            enemyHp->UpdateBar();
        }
    }
    else {
        this->SetHpVisuals();
    }
    this->SetUltCharge();

    bool someoneDied = false;
    for (auto it = fieldMobs.begin(); it != fieldMobs.end(); ) {
        if((*it)->GetIsDead()) {
            it = fieldMobs.erase(it);
            someoneDied = true;
        }
        else {
            it++;
        }
    }

    if(someoneDied) {
        for (auto it = turnOrderFIFO.begin(); it != turnOrderFIFO.end(); ) {
            if((*it)->GetIsDead()) {
                it = turnOrderFIFO.erase(it);
            }
            else {
                it++;
            }
        }

        while(turnOrderFIFO.size() <= 5) {
            CalculateNext();
        }
        CheckIfOver();
        if (combatState != OVER)
            if(wasMyAction) this->RecalcEnemyTarget();
    }
}

void Fight::CheckIfOver() {
    bool allEnemiesDead = true;
    for(auto& enemy : enemies) {
        if(!enemy->GetIsDead()) allEnemiesDead = false;
    }

    bool allAlliesDead = true;
    for(auto& ch : characters) {
        if(!ch->GetIsDead()) allAlliesDead = false;
    }

    if(allEnemiesDead || allAlliesDead) {
        combatState = OVER;
        this->End(allEnemiesDead);
    }
}
void Fight::End(bool win) {
    if (win) {
        levelStars.noDeaths = true;
        for(auto& ch : characters) {
            if(ch->GetIsDead()) levelStars.noDeaths = false;
        }
        if (AV < 2000) levelStars.lessThan10 = true;
        if (AV < 5000) levelStars.lessThan5 = true;
        std::cout << "!!!!!!!!!!!level stars: " << levelStars.lessThan5 << levelStars.lessThan10 << levelStars.noDeaths
        <<std::endl;
    }

    for (auto& element : uiBoundle) {
        if (element->GetName() == "battleOver") {
            element->SetVisible(true);
            if (!win) element->Tint(SDL_Color{135, 16, 16});
            else element->UnTint();
                CombatUltAnim(element,animProps.at(6));
        }
    }
}
GameMats Fight::MaterialsEarned() {
    GameMats mats = {0,0,0};
    bool allAlliesDead = true;
    for(auto& ch : characters) {
        if(!ch->GetIsDead()) allAlliesDead = false;
    }
    if (allAlliesDead) return mats;
    int numOfStars = 0;
    numOfStars += levelStars.lessThan5 ? 1 : 0;
    numOfStars += levelStars.lessThan10 ? 1 : 0;
    numOfStars += levelStars.noDeaths ? 1 : 0;
    switch (numOfStars) {
        case 3: {
            mats = {20,100,200};
            break;
        }
        case 2: {
            mats = {10,80,100};
            break;
        }
        case 1: {
            mats = {5,50,50};
            break;
        }
        default: {
            mats = {2,25,30};
            break;
        }
    }
    return mats;
}



void Fight::ChangeEnemyTarget(int dir) {
    do {
        currentEnemyTarget += dir;
        if(currentEnemyTarget == -1) currentEnemyTarget = enemies.size() - 1;
        else if(currentEnemyTarget >= (int)enemies.size()) currentEnemyTarget = 0;
    } while (enemies.at(currentEnemyTarget)->GetIsDead());

    this->PositionTarget(currentEnemyTarget);
}
void Fight::RecalcEnemyTarget() {
    this->ChangeEnemyTarget(1);
}

int Fight::CalculateStarsGot(TypeMultiplier type) {
    switch(type.type) {
        case Buster: {
            return currentlyActing->GetStat("starAbsorbB") *starCount;
        }
        case Arts: {
            return currentlyActing->GetStat("starAbsorbA") *starCount;
        }
        case Quick: {
            return currentlyActing->GetStat("starAbsorbQ") *starCount;
        }
    }
    return 0;
}



//VISUAL TINHGY BINGY'S
void Fight::TintGreyUIElement(SDL_Color color, const std::string& name) { //not final
    for(auto& element : uiBoundle) {
        if(element->GetName() == name)
            element->Tint(color);
    }
}
void Fight::SetCritStarText() {
    for(auto element: uiBoundle) {
        if(element->GetName() == "critStarNumber") {
            auto label = dynamic_cast<Text*>(element);
            label->ChangeText(std::to_string(starCount).c_str());
        }
    }
}
void Fight::SetDmgText(Dmg dmg, SDL_Color color) {
    for(auto& element: uiBoundle) {
        if(element->GetName() == "DmgNumber") {
            auto label = dynamic_cast<Text*>(element);
            std::string number = dmg.isCrit ?  std::to_string((int)dmg.amount) + "!!" : std::to_string((int)dmg.amount);
            label->ChangeText(number.c_str());
            label->ChangeColor(color);
        }
    }
}
void Fight::SetEnemyDmgText(Dmg dmg, int x) {
    for(auto& element: uiBoundle) {
        if(element->GetName() == "enemyDmgNumber") {
            auto label = dynamic_cast<Text*>(element);
            std::string number = dmg.isCrit ?  std::to_string((int)dmg.amount) + "!!" : std::to_string((int)dmg.amount);
            label->ChangeText(number.c_str());
            label->GetTransform().position.x = x;

            Transform t = label->GetTransform();
            t.position.x += 50;
            t.position.y -= 30;
            t.opacity = 1.0f;

            element->AddTweeny(t,3000,label->GetTransform(),OUT);
        }
    }
}
void Fight::SetHpVisuals() {
    for(auto& cont : combatChConts) {
        dynamic_cast<Bar*>(cont.at(3))->UpdateBar();
    }
}
void Fight::SetUltCharge() {
    for(auto& cont : combatChConts) {
        dynamic_cast<Bar*>(cont.at(2))->UpdateBar();
    }
}
void Fight::SetNaTypeChooser(bool isVisible) {
    for(auto element : combatNATypeCont) {
        element->SetVisible(isVisible);
    }
    if(isVisible) {
        int bCritChance = 10 * this->CalculateStarsGot(TypeMultiplier(Buster));
        int aCritChance = 10 * this->CalculateStarsGot(TypeMultiplier(Arts));
        int qCritChance = 10 * this->CalculateStarsGot(TypeMultiplier(Quick));
        for(auto element : combatNATypeCont) {
            if(element->GetName() == "BusterCritChanceText") {
                Text* text = dynamic_cast<Text*>(element);
                text->ChangeText((std::to_string(bCritChance) + "%").c_str());
            }
            else if(element->GetName() == "ArtsCritChanceText") {
                Text* text = dynamic_cast<Text*>(element);
                text->ChangeText((std::to_string(aCritChance) + "%").c_str());
            }
            else if(element->GetName() == "QuickCritChanceText") {
                Text* text = dynamic_cast<Text*>(element);
                text->ChangeText((std::to_string(qCritChance) + "%").c_str());
            }
        }
    }
}
void Fight::ScaleCharacterCont(bool up, Character* ch) {
    if(up) {
        int n = 0;
        int xpos = 39;
        for(n = 0; n < 3; n++) {
            if(ch->GetName() == characters.at(n)->GetName()) {
                break;
            }
            xpos += 400;
        }
        combatChConts.at(n).at(0)->GetTransform().position = {xpos,761,441,281};
        combatChConts.at(n).at(1)->GetTransform().position = {xpos,761,441,281};
        combatChConts.at(n).at(2)->GetTransform().position = {xpos+309,853,93,82};
        combatChConts.at(n).at(3)->GetTransform().position = {xpos+36,976,249,33};
        combatChConts.at(n).at(4)->GetTransform().position = {xpos+78,770,298,241};

        std::vector<UIElement*> buffIcons = characters.at(n)->GetBuffIcons();
        for(size_t i = 0; i < buffIcons.size(); i++) {
            int x = characters.at(n)->GetSprite()->GetTransform().position.x+200 + i*29;
            buffIcons.at(i)->GetTransform().position = SDL_Rect{x,762,25,23};
        }
    }
    else {
        int n = 0;
        int xpos = 78;
        for(n = 0; n < 3; n++) {
            if(ch->GetName() == characters.at(n)->GetName()) {
                break;
            }
            xpos += 397;
        }
        combatChConts.at(n).at(0)->GetTransform().position = {xpos,840,361,223};
        combatChConts.at(n).at(1)->GetTransform().position = {xpos,840,361,223};
        combatChConts.at(n).at(2)->GetTransform().position = {xpos+252,913,77,68};
        combatChConts.at(n).at(3)->GetTransform().position = {xpos+33,1004,204,27};
        combatChConts.at(n).at(4)->GetTransform().position = {xpos+65,845,244,198};

        std::vector<UIElement*> buffIcons = characters.at(n)->GetBuffIcons();
        for(size_t i = 0; i < buffIcons.size(); i++) {
            int x = characters.at(n)->GetSprite()->GetTransform().position.x+165 + i*24;
            buffIcons.at(i)->GetTransform().position = SDL_Rect{x,835,20,19};
        }
    }
    this->SetHpVisuals();
    this->SetUltCharge();
}
void Fight::ScaleUpEnemyCont() {
    //TEMP
    Transform t1 = currentlyActingEnemy->GetSprite()->GetTransform();
    t1.scale = 1.0f;
    currentlyActingEnemy->GetSprite()->GetTransform().scale = 1.25f;
    Transform t2 = currentlyActingEnemy->GetSprite()->GetTransform();
    currentlyActingEnemy->GetSprite()->AddTweeny(t2,2000,t1,LIN);
}
void Fight::PositionTarget(int enemyIndex) {
    int x = enemies.at(enemyIndex)->GetSprite()->GetTransform().position.x + enemies.at(enemyIndex)->GetSprite()->GetTransform().position.w/2 - combatTargetIndicator->GetTransform().position.w/2;
    int y = enemies.at(enemyIndex)->GetSprite()->GetTransform().position.y + enemies.at(enemyIndex)->GetSprite()->GetTransform().position.h/2 - combatTargetIndicator->GetTransform().position.h/2;

    combatTargetIndicator->GetTransform().position.x = x;
    combatTargetIndicator->GetTransform().position.y = y;
}
void Fight::SetSpVisuals() {
    for(auto& element : uiBoundle) {
        if(element->GetName() == "sp0") {
            dynamic_cast<Sprite*>(element)->ChangeState(skillPoints);

            std::cout << "skilpoint spent, currently av.: "<< skillPoints <<std::endl;
        }
    }
}
void Fight::SetSkillNeedsTarget(bool isVisible) {
    for(auto& element : uiBoundle) {
        if(element->GetName() == "skillNeedsTarget") {
            element->SetVisible(isVisible);
            if (currentlyActing) {
                dynamic_cast<Text*>(element)->ChangeColor(currentlyActing->color);

                for (auto& ch : characters) {
                    if (!ch->GetIsDead() && ch->GetName() != currentlyActing->GetName()) {
                        this->ScaleCharacterCont(isVisible,ch);
                    }
                }
            }
            std::cout << "skill needs target vbisible" <<std::endl;
        }
    }
}
void Fight::SetSkillButtonVisuals() {
    for (auto& element : uiBoundle) {
        if (element->GetName() == "skill") {
            auto label = dynamic_cast<Text*>(element);
            if (currentlyActing->CanSkill()) {
                label->ChangeText("");
                label->Tint(currentlyActing->color);
            }
            else {
                label->ChangeText(std::to_string(currentlyActing->SkillCooldownLeft()).c_str());
                label->UnTint();
                label->ChangeColor(currentlyActing->color);
            }
        }
    }
}

void Fight::ChangeTurnOrderCont() {
    for(int i = 0; i < 5; i++) {
        std::shared_ptr<SDL_Texture> thumb = turnOrderFIFO.at(i)->GetSprite()->GetTextureAt("turnThumb");
        turnOrderCont.at(i)->SetTexture(thumb);
    }
}
void Fight::SetWhosTurnText(bool mine) {
    for(auto element : uiBoundle) {
        if(element->GetName() == "whosTurn") {
            auto label = dynamic_cast<Text*>(element);
            if(mine) {
                label->ChangeText("YOUR TURN");
                label->ChangeColor(SDL_Color{224, 72, 161});
            }
            else {
                label->ChangeText("ENEMY'S TURN");
                label->ChangeColor(SDL_Color{115, 16, 16});
            }
            std::cout <<"turn label set" <<std::endl;
        }
    }
}

void Fight::TriggerSkillAnim() {
    int n = 0;
    for(int i = 0; i < 3; i++) {
        if(currentlyActing->GetName() == characters.at(i)->GetName()){ n = i;}
    }
    CombatSkillAnim(animProps.at(n));
}
bool Fight::AreAnimsOver() {
    bool ret = true;
    for(auto& element : uiBoundle) {
        if(element->GetTweeny() != nullptr) ret = false;
    }
    for(auto& mob : fieldMobs) {
        if(mob->GetSprite()->GetTweeny() != nullptr) ret = false;
    }
    return ret;
}



