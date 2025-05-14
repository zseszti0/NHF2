#ifndef ADVENTURE_H
#define ADVENTURE_H
#include <string>
#include <vector>

#include "UIAndDisplay.h"

class FightNode;
struct FightNodeStars {
    bool noDeaths;
    bool lessThan5;
    bool lessThan10;
};
#include "scene.h"

class FightNode {
    std::string name;
    bool attempted;
    FightNodeStars stars;

    Sprite* icon;

    std::vector<Enemy*> enemies;

    PreFight* preFight = nullptr;
public:

    FightNode(const char* source);
    void CreatePreFight(SpriteButton* tutOverlay);

    std::string GetName() {return name;}
    bool GetAttempted() {return attempted;}
    FightNodeStars GetStars() {return stars;}
    Sprite*& GetIcon(){return icon;}
    void Attempted() {
        attempted = true;
        icon->UnTint();
    }
    void UpdateSpriteIconState() {
        std::string stateName = "";
        stateName += stars.lessThan5 ? "1" : "0";
        stateName += stars.lessThan10 ? "1" : "0";
        stateName += stars.noDeaths ? "1" : "0";
        icon->ChangeState(stateName.c_str());
    }
    void UpdateStars(FightNodeStars stars_got) {
        if (!stars.noDeaths) stars.noDeaths = stars_got.noDeaths;
        if (!stars.lessThan5) stars.lessThan5 = stars_got.lessThan5;
        if (!stars.lessThan10) stars.lessThan10 = stars_got.lessThan10;
        attempted = true;
        icon->UnTint();

        this->UpdateSpriteIconState();
    }
    void Start();


    bool HandleClick(int mouseX, int mouseY) {
        if (mouseX > icon->GetTransform().position.x && mouseX < icon->GetTransform().position.x + icon->GetTransform().position.w && mouseY > icon->GetTransform().position.y && mouseY < icon->GetTransform().position.y + icon->GetTransform().position.h) {
            Start();
            return true;
        }
        return false;
    }

    void HandleHover(int mouseX, int mouseY) {

    }
    void Render() {
        icon->Render();
    }

    ~FightNode();
};

#endif //ADVENTURE_H
