/*
The UIAndDisplay.h file defines the foundational UI components of the game. It establishes a hierarchy of classes to represent various UI elements, enabling modularity, reusability, and ease of maintenance. The design draws inspiration from frameworks like Unity, emphasizing a component-based architecture.
*/
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

/*
Purpose: UIElement serves as the base class for all UI components, encapsulating common properties and behaviors such as positioning, visibility, rendering, and animation.

Key Members:
•	std::string name;
•	std::shared_ptr<SDL_Texture> texture;
•	Transform transform;
•	bool visible;
•	Animation* anim;

Core Functions:
•	void Render();
Renders the UI element using its texture and transform properties.
•	void SetTexture(std::shared_ptr<SDL_Texture> tex);
Assigns a new texture to the UI element.
•	void SetVisible(bool isVisible);
Sets the visibility state of the UI element.
•	Transform& GetTransform(int x, int y);
Helps change the transform of the element.
•	void Tint(SDL_Color color);
Applies a color tint to the UI element.
•	void Untint();
Removes any applied color tint.
•	void addTweeny(Animation* animation);
Assigns an animation to the UI element.

Design Considerations:
•	Smart Pointers: Utilizes std::shared_ptr for texture management to ensure proper memory handling and avoid leaks.
•	Transform Structure: Inspired by Unity's Transform component, it encapsulates position, scale, and rotation, promoting a clean and intuitive interface for spatial manipulations.
•	Animation Integration: The anim pointer allows for dynamic animations, enabling smooth transitions and effects.

*/
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

/*
Purpose: Button extends UIElement to represent interactive buttons with event handling capabilities.

Key Members:
•	std::function<void()> onClick;
•	std::function<void()> onRelease;
•	std::function<void()> onHover;
•	bool isActive;
•	SDL_Rect clickField;

Core Functions:
•	bool HandleClick/ HandleHover(int mouseX, int mouseY);
Processes SDL events to determine if the button was clicked, released, or hovered over, invoking the corresponding callbacks. Returns true if cicked/hovered.
•	void AddOnClick/AddHoverBehaviour(std::function<void()> func);
Assigns a functions to be called upon a mouse event.
•	void Disable(bool)
Adjusts the activity of a button.
•	Static void AddBasicScaleUpHoverAnim(…)
Static function, to add a simple scale up hover animation to a Button. Used frequently, hence the preset.

Design Considerations:
•	Lambda Functions: Embraces the use of lambdas for event callbacks, providing flexibility and encapsulation of behavior.
•	Active State Management: The isActive flag allows for enabling or disabling button interactions dynamically.
•	Custom Click Field: The clickField member defines the interactive area, which can differ from the visual representation, offering precise control over user interactions.
*/
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

/*
Purpose: Sprite extends UIElement to manage multiple textures, facilitating state changes and animations.

Key Members:
•	std::vector<std::string> stateNames;
•	std::vector<std::shared_ptr<SDL_Texture>> textures;
•	int currentStateIndex;

Core Functions:
•	void AddState(cosnt char* name, std::shared_ptr<SDL_Texture> tex);
Adds a new state with the associated texture.
•	void ChangeState(cosnt char* name/int index);
Switches to the texture associated with the given state name or index.
•	std::shared_ptr<SDL_Texture> GetTextureAt(cosnt char* name/int index);
Returns the texture at a specific state.

Design Considerations:
•	State Management: Allows for dynamic switching between different visual states, essential for representing animations, status changes or packing different states to a ui element.
•	Texture Organization: By associating names with textures, it simplifies the process of identifying and switching between states.
*/
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

/*
Purpose: Text extends UIElement to render textual content with customizable properties.

Key Members:
•	std::string content;
•	TTF_Font* font;
•	SDL_Color color;
•	int fontSize;

Core Functions:
•	void ChangeText(std::string newText);
Updates the displayed text content.
•	void ChangeFont(TTF_Font* newFont);
Changes the font used for rendering text.
•	void ChangeColor(SDL_Color newColor);
Sets the color of the text.

Design Considerations:
•	Dynamic Text Rendering: Facilitates real-time updates to text content, essential for displaying changing game information.
•	Customization: Offers flexibility in appearance through adjustable fonts, sizes, and colors.
*/
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
/*
Purpose: OutlinedText inherits from Text to render text with an outline, enhancing readability against various backgrounds.

Key Members:
•	SDL_Color outlineColor;
•	int outlineSize;

Design Considerations:
•	Enhanced Visibility: Outlining text improves visibility, especially in visually complex scenes.
•	Separate Class: By creating a distinct class, it avoids cluttering the base Text class with outline-specific logic.
*/
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

/*
Purpose: Bar extends UIElement to represent progress indicators like health or mana bars.

Key Members:
•	double* valuePtr;
•	double maxValue;
•	std::shared_ptr<SDL_Texture> barTexture;
•	bool dir;

Core Functions:
•	void update();
Recalculates the fill level texture based on the current value.
•	void SetCur(double* cur);
Assigns the pointer to the value being tracked.
•	void setMax (double max);
Sets the maximum value charge for the bar.
Design Considerations:

•	Pointer Usage: By referencing a value directly, the bar reflects real-time changes without additional updates.
•	Flexible Orientation: Supports multiple fill directions, accommodating various UI designs.
*/
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

/*
Purpose: Composite class for interactive buttons with multiple texture states.
*/
class SpriteButton : public Sprite, public Button {
public:
  SpriteButton(const char* n, const char* path,SDL_Rect rect,std::function<void()> func)
      :UIElement(n,path,rect), Sprite(n,path,rect), Button(n,path,rect,std::move(func)) {}

  void Render() override;
  ~SpriteButton() override;
};

/*
Purpose: Composite class for interactive buttons with text on top, enabling mass producing similar option buttons (e.g. for tutorials).
*/
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
