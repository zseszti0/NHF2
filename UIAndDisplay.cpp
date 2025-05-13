#include "UIAndDisplay.h"
#include "animation.h"
#include "scene.h"

#include <cmath>
#include <memory>

SDL_Renderer* UIElement::renderer = nullptr;  // Static member initialization



///UIElements
///
UIElement::UIElement(const char* n, const char* texturePath, SDL_Rect rect) {
    name = n;
    transform.position = rect;
    isVisible = true;
    transform.rotation = 0;
    transform.scale = 1.0f;
    transform.scrRect = {0,0,rect.w,rect.h};

    SDL_Surface* surface = IMG_Load(texturePath);
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    std::cout << "UIelement: " << name << " created" <<std::endl;
}

void UIElement::Render() {
    if(isVisible) {
        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        SDL_Rect scaledPos = {transform.position.x,transform.position.y,(int)(transform.position.w * transform.scale),(int)(transform.position.h * transform.scale)};
        SDL_RenderCopyEx(renderer,texture,&transform.scrRect,&scaledPos,transform.rotation,nullptr,SDL_FLIP_NONE);
    }
}
void UIElement::ScaleUp(float scaleFactor, float anchorX, float anchorY) {
    int newWidth = static_cast<int>(transform.position.w * scaleFactor);
    int newHeight = static_cast<int>(transform.position.h * scaleFactor);

    int deltaW = newWidth - transform.position.w;
    int deltaH = newHeight - transform.position.h;

    transform.position.x -= static_cast<int>(deltaW * anchorX);
    transform.position.y -= static_cast<int>(deltaH * anchorY);

    transform.position.w = newWidth;
    transform.position.h = newHeight;
    std::cout << "scaled up:  " << name << std::endl;

}
void UIElement::Rotate(int degrees) {
    transform.rotation = degrees;
}

void UIElement::AddTweeny(Transform result, int duration, Transform afterState, AnimEasing easing) {
    if(animation == nullptr) {
        switch (easing) {
            case LIN:
                animation = new Animation(&transform,result,duration,afterState,tweeny::easing::linear);
            break;
            case OUT:
                animation = new Animation(&transform,result,duration,afterState,tweeny::easing::quadraticOut);
            break;
            case IN:
                animation = new Animation(&transform,result,duration,afterState,tweeny::easing::quadraticIn);
            break;
        }
    }
}


void UIElement::UpdateTweens(float dt) {
    if(animation != nullptr) {
    std::cout <<"------------update: " <<name <<std::endl;
        if(animation->update(dt)) {
            std::cout <<"------------finished anim: " <<name <<std::endl;
            delete animation;
            animation = nullptr;
        }
    }
}



bool Button::HandleClick(int mouseX, int mouseY) {
    if(isActive && isVisible){
        if (mouseX > clickField.x && mouseX < clickField.x + clickField.w && mouseY > clickField.y && mouseY < clickField.y + clickField.h) {
            onClick();  // Call the function when clicked
            return true;
        }
    }
    return false;
}
bool Button::HandleHover(int mouseX, int mouseY) {
    if(isActive && isVisible){
        if (mouseX > clickField.x && mouseX < clickField.x + clickField.w && mouseY > clickField.y && mouseY < clickField.y + clickField.h) {
            onHover();
            return true;
        }
        else {
            onRelease();
        }
    }
            return false;
}
void Button::AddBasicScaleUpHoverAnim(Button *target, float anchorX, float anchorY, float scale) {
    std::cout << "basic anim 'scale up' added to: " << target->GetName() << std::endl;
    //count up the posis.
    SDL_Rect ogPos = target->transform.position;
    target->ScaleUp(scale, anchorX, anchorY);
    SDL_Rect scaledUpPos = target->transform.position;

    target->transform.position = ogPos;

    target->AddHoverBehaviour([target, scaledUpPos] {
        target->transform.position = scaledUpPos;
    },
    [target, ogPos] {
        target->transform.position = ogPos;
    });
}


size_t Sprite::NameToIndex(const char *name) {
    size_t index = -1;
    for (size_t i = 0; i < states.size(); i++) {
        if (stateNames[i] == name) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        std::cerr << "No sprite state found as: " << name << std::endl;
    }
    return index;
}

void Sprite::AddState(const char* newStateID,const char* newStatePath) {
    //Load the texture of the element.
    SDL_Surface* surface = IMG_Load(newStatePath);
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    } else {
        SDL_Texture* newState = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        states.push_back(newState);
        stateNames.emplace_back(newStateID);
    }
}
void Sprite::ChangeState(const char* newState) {
    if(strcmp(newState,"base") == 0) {
        currentState = "base";
        texture = states[0];
        return;
    }
    size_t index = this->NameToIndex(newState);

    if (index < 0 || index >= states.size())
        return;
    if (index != -1) {
        currentState = newState;
        texture = states[index];
    }
}
void Sprite::ChangeState(int index) {
    if (index < 0 || index >= states.size())
        return;
    currentState = stateNames[index];
    texture = states[index];
}
SDL_Texture* Sprite::GetTextureAt(const char *name) {
    if(strcmp(name,"base") == 0) {
        return states[0];
    }
    size_t index = this->NameToIndex(name);
    if (index != -1) {
        return states[index];
    }
    return nullptr;
}



void SpriteButton::Render() {
    if(isVisible) {

        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        SDL_RenderCopyEx(renderer,texture,&transform.scrRect,&transform.position,transform.rotation,nullptr,SDL_FLIP_NONE);
    }
}


Text::Text(const char* n, const char* fPath, SDL_Rect pos, const char* t, SDL_Color c, int s) {
    name = n;
    transform.position = pos;
    transform.scrRect = {0,0,pos.w,pos.h};

    size  = s;
    font = TTF_OpenFont(fPath, size);
    color = c;
    text = t;

}
void Text::Render() {
    if(isVisible) {
        //needs this cos the text,size color etc. might have changed!!
        SDL_Surface* textSurface = TTF_RenderText_Blended(font,text.c_str(),color);
        SDL_DestroyTexture(texture);
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface); // We don't need the surface anymore

        //need this cos we dont know the length and height of the text.
        SDL_QueryTexture(texture, nullptr, nullptr, &transform.position.w, &transform.position.h);

        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        SDL_RenderCopyEx(renderer,texture,nullptr,&transform.position,transform.rotation,nullptr,SDL_FLIP_NONE);
    }
}

void Text::ChangeColor(SDL_Color c) {
    color = c;
}
void Text::ChangeFont(const char* fontPath) {
    font = TTF_OpenFont(fontPath, size);
}
void Text::ChangeText(const char* t) {
    text = t;
}

OutlinedText::OutlinedText(const char *n, const char *fPath, SDL_Rect pos, const char *t, SDL_Color textC, SDL_Color outlineC, int s, int outlineS): Text(n,fPath,pos,t,textC,s) {
    outlineColor = outlineC;
    outlineSize = outlineS;

    outlineFont = TTF_OpenFont(fPath, size);
    TTF_SetFontOutline(outlineFont, outlineSize);
}

void OutlinedText::Render() {
    if(isVisible) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(font,text.c_str(),color);
        SDL_DestroyTexture(texture);
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface); // We don't need the surface anymore

        //need this cos we dont know the length and height of the text.
        SDL_QueryTexture(texture, nullptr, nullptr, &transform.position.w, &transform.position.h);


        SDL_Surface* outlineSurface = TTF_RenderText_Blended(outlineFont,text.c_str(),outlineColor);
        SDL_DestroyTexture(outlineTexture);
        outlineTexture = SDL_CreateTextureFromSurface(renderer, outlineSurface);
        SDL_FreeSurface(outlineSurface); // We don't need the surface anymore

        SDL_Rect destRect = {
            transform.position.x,
            transform.position.y,
            transform.position.w,
            transform.position.h
        };

        // 5️⃣ Render outline then text
        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int

        SDL_SetTextureAlphaMod(outlineTexture, alpha);
        SDL_SetTextureBlendMode(outlineTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        SDL_RenderCopyEx(renderer,outlineTexture,nullptr,&destRect,transform.rotation,nullptr,SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, texture, nullptr, &transform.position, transform.rotation, nullptr, SDL_FLIP_NONE);

    }
}

void OutlinedText::ChangeFont(const char *fontPath) {
    font = TTF_OpenFont(fontPath, size);
    outlineFont = TTF_OpenFont(fontPath, size);
}


void Bar::UpdateBar() {
    barPos = transform.position;
    if(dir) {
        int newWidth = (*currentValue/maxValue) * transform.position.w;
        transform.scrRect = SDL_Rect{
            transform.position.w-newWidth,
            0,
            newWidth,
            transform.position.h
        };
        barPos.w = newWidth;
        barPos.x = transform.position.x + transform.position.w - newWidth;
    }
    else {
        int newHeight = (*currentValue/maxValue) * transform.position.h;
        transform.scrRect = SDL_Rect{
            0,
            transform.position.h-newHeight,
            transform.position.w,
            newHeight
        };
        barPos.h = newHeight;
        barPos.y = transform.position.y + transform.position.h - newHeight;
    }
}
void Bar::Render() {
    if(isVisible) {
        if (dir) {
            barPos.y = transform.position.y;
            barPos.h = transform.position.h;
        }
        else {
            barPos.x = transform.position.x;
            barPos.w = transform.position.w;
        }

        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_RenderCopyEx(renderer,texture,nullptr,&transform.position,transform.rotation,nullptr,SDL_FLIP_NONE);

        SDL_SetTextureAlphaMod(barTexture, alpha);
        SDL_SetTextureBlendMode(barTexture, SDL_BLENDMODE_BLEND);
        SDL_RenderCopyEx(renderer,barTexture,&transform.scrRect,&barPos,transform.rotation,nullptr,SDL_FLIP_NONE);
    }
}

void TextButton::Render() {
    if(isVisible) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(font,text.c_str(),color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface); // We don't need the surface anymore

        //need this cos we dont know the length and height of the text.
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textRect.w, &textRect.h);
        textRect.x = transform.position.x + (transform.position.w - textRect.w) / 2;
        textRect.y = transform.position.y + (transform.position.h - textRect.h) / 2;

        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(textTexture, alpha);
        SDL_SetTextureBlendMode(textTexture, SDL_BLENDMODE_BLEND);

        SDL_RenderCopyEx(renderer, texture, nullptr, &transform.position, transform.rotation, nullptr, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, textTexture, nullptr, &textRect, transform.rotation, nullptr, SDL_FLIP_NONE);

    }
}



void SimulateKeyPress(SDL_Keycode key) {
    SDL_Event event;

    // Simulate key down
    event.type = SDL_KEYDOWN;
    event.key.type = SDL_KEYDOWN;
    event.key.keysym.sym = key;
    event.key.state = SDL_PRESSED;
    SDL_PushEvent(&event);

    // Simulate key up (optional)
    event.type = SDL_KEYUP;
    event.key.type = SDL_KEYUP;
    event.key.keysym.sym = key;
    event.key.state = SDL_RELEASED;
    SDL_PushEvent(&event);
}
void EQPosUIElements(SDL_Rect area, std::vector<UIElement *> &elements, bool horizontal) {
    if(horizontal) {
        int totalWidth = 0;
        for(auto element : elements) {
            totalWidth += element->GetTransform().position.w;
        }
        double margin = ((double)area.w - (double)totalWidth)/elements.size();


        int xSoFar = area.x;
        for(auto& element : elements) {
            element->GetTransform().position.x = xSoFar;
            int x = element->GetTransform().position.x;
            xSoFar += element->GetTransform().position.w + margin;
        }
    }
    else {
        int totalHeight = 0;
        for(auto element : elements) {
            totalHeight += element->GetTransform().position.h;
        }
        double margin = ((double)area.h - (double)totalHeight)/elements.size();

        int ySoFar = area.y;
        for(auto & element : elements) {
            element->GetTransform().position.y = ySoFar;
            ySoFar += element->GetTransform().position.h + margin;
        }
    }
    for(auto& element: elements) {
        auto trigger = dynamic_cast<Button*>(element);
        if (trigger)
            trigger->ChangeClickFiled(trigger->GetTransform().position);
    }
}
void EQPosUIElements(SDL_Rect area, std::vector<Button*> &elements, bool horizontal) {
    if(horizontal) {
        int totalWidth = 0;
        for(auto element : elements) {
            totalWidth += element->GetTransform().position.w;
        }
        double margin = ((double)area.w - (double)totalWidth)/elements.size();


        int xSoFar = area.x;
        for(auto& element : elements) {
            element->GetTransform().position.x = xSoFar;
            int x = element->GetTransform().position.x;
            xSoFar += element->GetTransform().position.w + margin;
        }
    }
    else {
        int totalHeight = 0;
        for(auto element : elements) {
            totalHeight += element->GetTransform().position.h;
        }
        double margin = ((double)area.h - (double)totalHeight)/elements.size();

        int ySoFar = area.y;
        for(auto & element : elements) {
            element->GetTransform().position.y = ySoFar;
            ySoFar += element->GetTransform().position.h + margin;
        }
    }
    for(auto& element: elements) {
        element->ChangeClickFiled(element->GetTransform().position);
    }
}

