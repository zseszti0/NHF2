#include "UIAndDisplay.h"
#include "animation.h"
#include "scene.h"

#include <cmath>
#include <memory>

SDL_Renderer* UIElement::renderer = nullptr;  // Static member initialization
float UIElement::scaleX;
float UIElement::scaleY;

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
        texture = std::shared_ptr<SDL_Texture>(
            SDL_CreateTextureFromSurface(renderer, surface),
            SDL_DestroyTexture  // Custom deleter
        );
        SDL_FreeSurface(surface);
    }

    std::cout << "UIelement: " << name << " created" <<std::endl;
}

void UIElement::Render() {
    if(isVisible) {
        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture.get(), alpha);
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);


        SDL_Rect scaledPos = {transform.position.x,transform.position.y,(int)(transform.position.w * transform.scale),(int)(transform.position.h * transform.scale)};

        SDL_Rect fullscreentexture =
            {(int)(scaledPos.x * scaleX),
            (int)(scaledPos.y * scaleY),
            (int)(scaledPos.w * scaleX),
            (int)(scaledPos.h * scaleY)
        };

        SDL_RenderCopyEx(renderer,texture.get(),&transform.scrRect,&fullscreentexture,transform.rotation,nullptr,SDL_FLIP_NONE);
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
void Button::Render() {
    if(isVisible) {

        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture.get(), alpha);
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);

        SDL_Rect scaledPos = {transform.position.x,transform.position.y,(int)(transform.position.w * transform.scale),(int)(transform.position.h * transform.scale)};

        SDL_Rect fullscreentexture =
            {(int)(scaledPos.x * scaleX),
            (int)(scaledPos.y * scaleY),
            (int)(scaledPos.w * scaleX),
            (int)(scaledPos.h * scaleY)
        };

        clickField = fullscreentexture;

        SDL_RenderCopyEx(renderer,texture.get(),&transform.scrRect,&fullscreentexture,transform.rotation,nullptr,SDL_FLIP_NONE);
    }
}

size_t Sprite::NameToIndex(const char *name) {
    int index = 0;
    for (size_t i = 0; i < states.size(); i++) {
        if (stateNames[i] == name) {
            index = i;
            break;
        }
    }
    return index;
}

void Sprite::AddState(const char* newStateID, const char* newStatePath) {
    SDL_Surface* surface = IMG_Load(newStatePath);
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    } else {
        auto newState = std::shared_ptr<SDL_Texture>(
            SDL_CreateTextureFromSurface(renderer, surface),
            SDL_DestroyTexture
        );
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

    currentState = newState;
    texture = states[index];
}
void Sprite::ChangeState(size_t index) {
    if (index < 0 || index >= states.size())
        return;
    currentState = stateNames[index];
    texture = states[index];
}

std::shared_ptr<SDL_Texture> Sprite::GetTextureAt(const char *name) {
    if(strcmp(name,"base") == 0) {
        return states[0];
    }
    size_t index = this->NameToIndex(name);
    return states[index];
}



void SpriteButton::Render() {
    if(isVisible) {

        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture.get(), alpha);
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);

        SDL_Rect scaledPos = {transform.position.x,transform.position.y,(int)(transform.position.w * transform.scale),(int)(transform.position.h * transform.scale)};

        SDL_Rect fullscreentexture =
            {(int)(scaledPos.x * scaleX),
            (int)(scaledPos.y * scaleY),
            (int)(scaledPos.w * scaleX),
            (int)(scaledPos.h * scaleY)
        };

        clickField = fullscreentexture;

        SDL_RenderCopyEx(renderer,texture.get(),&transform.scrRect,&fullscreentexture,transform.rotation,nullptr,SDL_FLIP_NONE);
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
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        texture = std::shared_ptr<SDL_Texture>(
            SDL_CreateTextureFromSurface(renderer, textSurface),
            SDL_DestroyTexture
        );
        SDL_FreeSurface(textSurface);

        //need this cos we dont know the length and height of the text.
        SDL_QueryTexture(texture.get(), nullptr, nullptr, &transform.position.w, &transform.position.h);

        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture.get(), alpha);
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);

        SDL_Rect scaledPos = {transform.position.x,transform.position.y,(int)(transform.position.w * transform.scale),(int)(transform.position.h * transform.scale)};

        SDL_Rect fullscreentexture =
            {(int)(scaledPos.x * scaleX),
            (int)(scaledPos.y * scaleY),
            (int)(scaledPos.w * scaleX),
            (int)(scaledPos.h * scaleY)
        };

        SDL_RenderCopyEx(renderer,texture.get(),nullptr,&fullscreentexture,transform.rotation,nullptr,SDL_FLIP_NONE);
    }
}

void Text::ChangeColor(SDL_Color c) {
    color = c;
}
void Text::ChangeFont(const char* fontPath) {
    TTF_CloseFont(font);
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
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        texture = std::shared_ptr<SDL_Texture>(
            SDL_CreateTextureFromSurface(renderer, textSurface),
            SDL_DestroyTexture
        );
        SDL_FreeSurface(textSurface);


        //need this cos we dont know the length and height of the text.
        SDL_QueryTexture(texture.get(), nullptr, nullptr, &transform.position.w, &transform.position.h);



        SDL_Surface* outlineSurface = TTF_RenderText_Blended(outlineFont, text.c_str(), outlineColor);
        outlineTexture = std::shared_ptr<SDL_Texture>(
            SDL_CreateTextureFromSurface(renderer, outlineSurface),
            SDL_DestroyTexture
        );
        SDL_FreeSurface(outlineSurface);
        

        SDL_Rect destRect = {
            transform.position.x,
            transform.position.y,
            transform.position.w,
            transform.position.h
        };

        // 5️⃣ Render outline then text
        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int

        SDL_SetTextureAlphaMod(outlineTexture.get(), alpha);
        SDL_SetTextureBlendMode(outlineTexture.get(), SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture.get(), alpha);
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);


        SDL_Rect scaledPos = {transform.position.x,transform.position.y,(int)(transform.position.w * transform.scale),(int)(transform.position.h * transform.scale)};

        SDL_Rect fullscreentexture =
            {(int)(scaledPos.x * scaleX),
            (int)(scaledPos.y * scaleY),
            (int)(scaledPos.w * scaleX),
            (int)(scaledPos.h * scaleY)
        };

        SDL_Rect scaledPosOutine = {destRect.x,destRect.y,(int)(destRect.w * transform.scale),(int)(destRect.h * transform.scale)};

        SDL_Rect fullscreentextureOutline =
            {(int)(scaledPosOutine.x * scaleX),
            (int)(scaledPosOutine.y * scaleY),
            (int)(scaledPosOutine.w * scaleX),
            (int)(scaledPosOutine.h * scaleY)
        };

        SDL_RenderCopyEx(renderer,outlineTexture.get(),nullptr,&fullscreentextureOutline,transform.rotation,nullptr,SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, texture.get(), nullptr, &fullscreentexture, transform.rotation, nullptr, SDL_FLIP_NONE);

    }
}

void OutlinedText::ChangeFont(const char *fontPath) {
    TTF_CloseFont(font);
    font = TTF_OpenFont(fontPath, size);

    TTF_CloseFont(outlineFont);
    outlineFont = TTF_OpenFont(fontPath, size);
    TTF_SetFontOutline(outlineFont, outlineSize);
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
        SDL_SetTextureAlphaMod(texture.get(), alpha);
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);

        SDL_Rect scaledPos = {transform.position.x,transform.position.y,(int)(transform.position.w * transform.scale),(int)(transform.position.h * transform.scale)};

        SDL_Rect fullscreentexture =
            {(int)(scaledPos.x * scaleX),
            (int)(scaledPos.y * scaleY),
            (int)(scaledPos.w * scaleX),
            (int)(scaledPos.h * scaleY)
        };

        SDL_RenderCopyEx(renderer,texture.get(),nullptr,&fullscreentexture,transform.rotation,nullptr,SDL_FLIP_NONE);

        SDL_SetTextureAlphaMod(barTexture.get(), alpha);
        SDL_SetTextureBlendMode(barTexture.get(), SDL_BLENDMODE_BLEND);

        SDL_Rect scaledPosBar = {barPos.x,barPos.y,(int)(barPos.w * transform.scale),(int)(barPos.h * transform.scale)};

        SDL_Rect fullscreentextureBar =
            {(int)(scaledPosBar.x * scaleX),
            (int)(scaledPosBar.y * scaleY),
            (int)(scaledPosBar.w * scaleX),
            (int)(scaledPosBar.h * scaleY)
        };

        SDL_RenderCopyEx(renderer,barTexture.get(),&transform.scrRect,&fullscreentextureBar,transform.rotation,nullptr,SDL_FLIP_NONE);
    }
}

void TextButton::Render() {
    if(isVisible) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        textTexture = std::shared_ptr<SDL_Texture>(
            SDL_CreateTextureFromSurface(renderer, textSurface),
            SDL_DestroyTexture
        );
        SDL_FreeSurface(textSurface);

        //need this cos we dont know the length and height of the text.
        SDL_QueryTexture(textTexture.get(), nullptr, nullptr, &textRect.w, &textRect.h);
        textRect.x = transform.position.x + (transform.position.w - textRect.w) / 2;
        textRect.y = transform.position.y + (transform.position.h - textRect.h) / 2;

        transform.opacity = std::clamp(transform.opacity, 0.0f, 1.0f); // ensure it's in bounds
        int alpha = static_cast<int>(transform.opacity * 255.0f + 0.5f); // round to nearest int
        SDL_SetTextureAlphaMod(texture.get(), alpha);
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(textTexture.get(), alpha);
        SDL_SetTextureBlendMode(textTexture.get(), SDL_BLENDMODE_BLEND);

        SDL_Rect scaledPos = {transform.position.x,transform.position.y,(int)(transform.position.w * transform.scale),(int)(transform.position.h * transform.scale)};

        SDL_Rect fullscreentexture =
            {(int)(scaledPos.x * scaleX),
            (int)(scaledPos.y * scaleY),
            (int)(scaledPos.w * scaleX),
            (int)(scaledPos.h * scaleY)
        };

        SDL_Rect scaledPosText = {textRect.x,textRect.y,(int)(textRect.w * transform.scale),(int)(textRect.h * transform.scale)};

        SDL_Rect fullscreentextureText =
            {(int)(scaledPosText.x * scaleX),
            (int)(scaledPosText.y * scaleY),
            (int)(scaledPosText.w * scaleX),
            (int)(scaledPosText.h * scaleY)
        };

        clickField = fullscreentexture;

        SDL_RenderCopyEx(renderer, texture.get(), nullptr, &fullscreentexture, transform.rotation, nullptr, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, textTexture.get(), nullptr, &fullscreentextureText, transform.rotation, nullptr, SDL_FLIP_NONE);

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


UIElement::~UIElement() {
    delete animation;
    std::cout << "UIElement dtor" << std::endl;

}
Button::~Button() {
    std::cout << "Button dtor" << std::endl;

}
Sprite::~Sprite() {
    std::cout << "Sprite dtor" << std::endl;

}
Text::~Text() {
    std::cout << "Text dtor" << std::endl;
    TTF_CloseFont(font);
}
OutlinedText::~OutlinedText() {
    std::cout << "Outlined text dtor" << std::endl;
    TTF_CloseFont(outlineFont);
}
Bar::~Bar() {
    std::cout << "Bar dtor" << std::endl;

}
SpriteButton::~SpriteButton() {
    std::cout << "SpriteButton dtor" << std::endl;

}
TextButton::~TextButton() {
    std::cout << "SpriteButton dtor" << std::endl;
}

