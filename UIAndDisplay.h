/// VISUAL HANDLER
///
/// purpose: ----
/// The visualHandler header is responsible for the managing and handling of the different Scene comps of the game.
/// That includes switching seamlessly between scenes, rendering its background and UI elements (like buttons and text).
///
/// classes: ----
/// Scene, UIElement, Button, Sprite

#ifndef VISUALHANDLER_H
#define VISUALHANDLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <utility>
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <functional>
#include <memory>

class UIElement;
class Button;
class Sprite;
class Text;
class Bar;
class SpriteButton;
class TextButton;


struct Transform {
  SDL_Rect position{};
  float scale = 1.0f;
  float opacity = 1.0f;
  float rotation = 0;
  SDL_Rect scrRect = {0,0,0,0};

  Transform(SDL_Rect position, float scale, float opacity, float rotation, SDL_Rect src) {
    this->position = position;
    this->scale = scale;
    this->opacity = opacity;
    this->rotation = rotation;
    scrRect = src;
  }
};
#include "animation.h"

///UI elements isn't an abstract class, as it can be used on it's on (for example rendering just a lonely image to the screen)
class UIElement {
protected:
  std::string name;
  std::shared_ptr<SDL_Texture> texture;


  Animation* animation = nullptr;
  Transform transform = {{0,0,0,0},1.0f,1.0f,0,{0,0,0,0}};
  bool isVisible;
public:
  static float scaleX;
  static float scaleY;
  static SDL_Renderer* renderer;

  UIElement(const char* n = "empty", const char* texturePath = "./assets/UI/empty.jpg", SDL_Rect rect = {0,0,0,0});

  virtual void Render();
  std::shared_ptr<SDL_Texture> GetTexture() { return texture; }

  void SetTexture(std::shared_ptr<SDL_Texture> t) {
    texture = t;
  }
  Transform& GetTransform(){return transform;}
  void SetTransform(Transform t){transform = t;}
  std::string GetName(){return name;}

  bool IsVisible(){return isVisible;}
  void SetVisible(bool v){isVisible = v;}

  void ScaleUp(float scaleFactor, float anchorX = 0.0f, float anchorY = 0.0f);
  void Rotate(int degrees);
  virtual void Tint(SDL_Color tint) {
    SDL_SetTextureColorMod(texture.get(), tint.r, tint.g, tint.b);
  }
  void UnTint() {
    SDL_SetTextureColorMod(texture.get(), 255, 255, 255);
  }

  void AddTweeny(Transform result, int duration, Transform afterState, AnimEasing easing = LIN);
  Animation* GetTweeny(){return animation;}
  void UpdateTweens(float dt);


  virtual ~UIElement();
};
///Trivial usecase. Works with costum callBack functions to suit each buttons need.
class Button : virtual public UIElement {
protected:
  std::function<void()> onClick;

  std::function<void()> onHover = []{};
  std::function<void()> onRelease = []{};

  SDL_Rect clickField{};
  bool isActive = true;

public:
  Button(const char* name, const char* texturePath, const SDL_Rect pos, std::function<void()> onClick = []{}) : UIElement(name, texturePath, pos), onClick(std::move(onClick)), clickField(pos) {};

  void AddOnclick(std::function<void()> oc){onClick = std::move(oc);}
  void AddHoverBehaviour(std::function<void()> hoverFunc, std::function<void()> realaseFunc) {
    onHover = std::move(hoverFunc);
    onRelease = std::move(realaseFunc);
  }
  bool HandleClick(int mouseX, int mouseY);
  bool HandleHover(int mouseX, int mouseY);

  void ChangeClickFiled(SDL_Rect area){clickField =area;}
  void Disable(bool yes){isActive = !yes;}

  void Render() override;

  static void AddBasicScaleUpHoverAnim(Button* target,float anchorX = 0.5f,float anchorY = 0.5f ,float scale = 1.15f);


  ~Button() override;
};
///Sprites with one layer of changing element. Can switch between states, given its name.
class Sprite : virtual public UIElement {
protected:
  std::vector<std::shared_ptr<SDL_Texture>> states;
  std::vector<std::string> stateNames;

  size_t NameToIndex(const char* name);
  std::string currentState;
public:

  Sprite(const char* n, const char* texturePath,SDL_Rect pos) : UIElement(n, texturePath, pos) {
    states.push_back(texture);
    stateNames.push_back(name);
    currentState = name;
  };

  void AddState(const char* newStateID,const char* newStatePath);
  void ChangeState(const char* newState);
  void ChangeState(size_t index);


  std::shared_ptr<SDL_Texture> GetTextureAt(int index) { return states.at(index); }
  std::shared_ptr<SDL_Texture> GetTextureAt(const char* name);


  int GetNumberOfStates(){return states.size();}

  std::string GetCurrentState(){return currentState;}
  int GetCurrentStateIndex(){return NameToIndex(currentState.c_str());}

  ~Sprite() override;
};
///Text yippie
class Text : virtual public UIElement {
protected:
  std::string text;
  SDL_Color color{};
  TTF_Font* font;
  int size;

public:
  Text(const char* n, const char* fPath,SDL_Rect pos,const char* t, SDL_Color c = SDL_Color(255,255,255), int s = 24);

  std::string GetText(){return text;}

  void ChangeText(const char* t);
  virtual void ChangeFont(const char* fontPath);
  void ChangeColor(SDL_Color c);

  void Render() override;

  ~Text() override;
};
class OutlinedText : public Text {
  SDL_Color outlineColor{};
  TTF_Font* outlineFont;
  std::shared_ptr<SDL_Texture> outlineTexture;
  int outlineSize;

public:
  OutlinedText(const char* n, const char* fPath, SDL_Rect pos, const char* t, SDL_Color textC = SDL_Color{255,255,255}, SDL_Color outlineC = SDL_Color{0,0,0}, int s = 24, int outlineS = 1);

  void ChangeFont(const char *fontPath) override;

  void Render() override;
  ~OutlinedText() override;
};

///Bar for hp bars, ult bars, anything that has a max value and visually its.. like.. a bar
class Bar : public UIElement {
  double maxValue = 0;
  double* currentValue = nullptr;

  std::shared_ptr<SDL_Texture> barTexture;
  SDL_Rect barPos{};

  bool dir; //true = horizontal, false = vertical
public:
  Bar(const char* n,const char* bgPath, const char* barPath, SDL_Rect rect, double maxv, double* currentV, bool d = true) : UIElement(n,bgPath,rect){
    SDL_Surface* surface = IMG_Load(barPath);
    if (!surface) {
      std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    } else {
      barTexture = std::shared_ptr<SDL_Texture>(
          SDL_CreateTextureFromSurface(renderer, surface),
          SDL_DestroyTexture
      );
      SDL_FreeSurface(surface);
    }

    barPos = rect;

    maxValue = maxv;
    currentValue = currentV;
    dir = d;
  }
  void SetCur(double* cur){currentValue = cur;}
  void SetMax(double max){maxValue = max;}

  void UpdateBar();
  void Render() override;
  void Tint(SDL_Color tint) override {
    SDL_SetTextureColorMod(barTexture.get(), tint.r, tint.g, tint.b);
  }

  ~Bar() override;
};

class SpriteButton : public Sprite, public Button {
public:
  SpriteButton(const char* n, const char* path,SDL_Rect rect,std::function<void()> func)
      :UIElement(n,path,rect), Sprite(n,path,rect), Button(n,path,rect,std::move(func)) {}

  void Render() override;
  ~SpriteButton() override;
};
class TextButton : public Text, public Button {
  std::shared_ptr<SDL_Texture> textTexture;
  SDL_Rect textRect{};
public:
  TextButton(const char* n, const char* texturePath,SDL_Rect rect,std::function<void()> func,
    const char* fontPath, const char* t = "empty", SDL_Color textC = SDL_Color{255,255,255}, int s = 24)
    : UIElement(n, texturePath, rect), Text(n,fontPath,rect,t), Button(n, texturePath, rect, std::move(func)) {
    size = s;
    font = TTF_OpenFont(fontPath, size);
    color = textC;
    text = t;
  }

  void Render() override;

 ~TextButton() override;
};



void SimulateKeyPress(SDL_Keycode key);
void EQPosUIElements(SDL_Rect area, std::vector<UIElement*>& elements, bool horizontal = true);
void EQPosUIElements(SDL_Rect area, std::vector<Button*>& elements, bool horizontal = true);

#endif
