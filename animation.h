/*
The animation system handles smooth UI element transitions using Tweeny — a lightweight C++ tweening library. Animations interpolate Transform properties (position, scale, opacity, rotation, etc.) over time with custom easing functions. It integrates deeply with the UI system by directly modifying Transform&, enabling reusable, lightweight, and fluid animations.
Overarching Design Considerations:
•	Pointer-Based Updating: Animations modify the actual UI state in-place through Transform&, eliminating sync complexity.
•	Completion-Tracking: Each animation reports when it's finished (Update(dt) returns true), so UI elements and scenes can know when they're idle.
•	After-State Handling: Supports temporary visual effects by allowing a post-animation "afterTransform" to apply once the main animation completes.
•	Scene-Aware Control: Scenes (like Fight) query whether a UI element is still animating, by checking if its anim pointer is non-null.
*/

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


/*
Purpose: Handles smooth transition of a UI element’s Transform from its current state to a target state over a set duration and easing.

Key Members:
•	Transform* target;
•	Transform afterState; - Optional transform applied immediately after animation completes (for transient effects).
•	std::vector<tweeny::tween<float>> transformTweens;

Core Functions:
•	Animation(Transform* target, const Transform& result, double duration, easing, optional after)
Initializes a new animation on the given Transform*, setting the desired result state, duration and easing.
•	bool Update(float dt)
Advances the animation by dt seconds, interpolates the target Transform fields. Returns true if the animation has finished (including any post-processing via afterState).

Design Considerations:
•	Animation modifies the UI element directly via pointer, making it stateless in usage and ideal for one-off or chained effects.
•	Update() being a bool-returning function makes it trivial for UI systems to manage animation lifecycle.
•	When Update() returns true, the owning UIElement clears its anim pointer — a simple and effective way to know if it's idle or in transition.

*/
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

/*
Animation Preset Functions (in animation.cpp)
Purpose: There are three standalone functions that orchestrate animations for multiple UI elements simultaneously — e.g., sliding in/out panels, skill an ult effects, etc.Provide reusable complex UI animations using multiple Tweeny instances internally.
*/
void CombatUltAnim(UIElement* target, UIElement* overlay);
void CombatSkillAnim(UIElement* target);

#endif //ANIMATION_H
