//
// Created by user on 01/05/2025.
//
#ifndef ANIMATION_H
#define ANIMATION_H
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

#include "tweeny-3.2.0.h"



enum AnimEasing {
  LIN,IN,OUT
};

class Animation;

#include "UIAndDisplay.h"

class Animation {
  Transform* target;
  Transform afterState;
  std::vector<tweeny::tween<float>> transformTweens;

public:
  Animation(Transform* targetRef,Transform result, int duration,Transform after, auto easingFunc)
    : target(targetRef), afterState(after) {

    transformTweens.push_back(tweeny::from((float)targetRef->position.x).to((float)result.position.x).during(duration).via(easingFunc));
    transformTweens.push_back(tweeny::from((float)targetRef->position.y).to((float)result.position.y).during(duration).via(easingFunc));
    transformTweens.push_back(tweeny::from(targetRef->scale).to(result.scale).during(duration).via(easingFunc));
    transformTweens.push_back(tweeny::from(targetRef->opacity).to(result.opacity).during(duration).via(easingFunc));
    transformTweens.push_back(tweeny::from(targetRef->rotation).to(result.rotation).during(duration).via(easingFunc));
  }

  bool update(float dt) {
    bool allDone = true;

    for (auto& tween : transformTweens) {
      tween.step(dt);
      if (tween.progress() < 1.0f) allDone = false;
    }

    // Apply interpolated values
    target->position.x = transformTweens[0].peek();
    target->position.y = transformTweens[1].peek();
    target->scale      = transformTweens[2].peek();
    target->opacity    = transformTweens[3].peek();
    target->rotation   = transformTweens[4].peek();

    // When all are done, snap to afterState
    if (allDone) {
      *target = afterState;
      return true;
    }

    return false;
  }

};


void CombatUltAnim(UIElement* target, UIElement* overlay);
void CombatSkillAnim(UIElement* target);

#endif //ANIMATION_H
