#ifndef ADVENTURE_H
#define ADVENTURE_H
#include <string>
#include <vector>

#include "UIAndDisplay.h"

class FightNode;
#include "scene.h"

class FightNode {
    std::string name;
    bool attempted;
    int stars;

    Sprite* icon;

    std::vector<Enemy*> enemies;

    PreFight* preFight = nullptr;
public:

    FightNode(const char* source);
    void CreatePreFight(SpriteButton* tutOverlay);

    std::string GetName() {return name;}
    bool GetAttempted() {return attempted;}
    int GetStars() {return stars;}
    Sprite*& GetIcon(){return icon;}
    void Attempted() {
        attempted = true;
        icon->UnTint();
    }
    void ChangeStars(int s) {stars = s;}

    void Start();


    void HandleClick(int mouseX, int mouseY) {
        if (mouseX > icon->GetTransform().position.x && mouseX < icon->GetTransform().position.x + icon->GetTransform().position.w && mouseY > icon->GetTransform().position.y && mouseY < icon->GetTransform().position.y + icon->GetTransform().position.h) {
            Start();
        }
    }
    void HandleHover(int mouseX, int mouseY) {

    }
    void Render() {
        icon->Render();
    }

    ~FightNode();

};

#endif //ADVENTURE_H
