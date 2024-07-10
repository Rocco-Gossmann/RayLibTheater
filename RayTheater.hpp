#ifndef RAYTHEATER_H
#define RAYTHEATER_H

#include "./internal/ActorAttributes.h"
#include "./internal/Builder.hpp"
#include "./internal/Play.h"
#include "./internal/Stage.hpp"
#include "./internal/macros.h"
#include "./internal/types.h"

// #include "./internal/RayTheaterBuilder.hpp"
// #include "./internal/RayTheaterPlay.hpp"
// #include "./internal/RayTheaterScene.hpp"
// #include "./internal/RayTheaterStage.hpp"
// #include "./internal/RayTheaterTimer.hpp"
// #include <climits>
// #include <cmath>
// #include <cstdio>
// #include <cstdlib>
// #include <functional>
// #include <iostream>
// #include <ostream>
// #include <unordered_set>
// #include <vector>
// // BM: ActorComponent - Transform2D - Class
// //==============================================================================
// class Transform2D : ActorComponent {
//   friend Stage;
//
// public:
//   Transform2D(Actor *a)
//       : ActorComponent(a, TRANSFORMABLE), _loc({0.0f, 0.0f}),
//         loc({0.0f, 0.0f}){};
//
//   /** @brief privides the Actors location, at the star of the cycle
//    * @return the actors location
//    */
//   Vector2 getLoc() { return Vector2(loc); }
//
//   /** @brief Requests the Actor to change its location at the beginning
//    * of the next Cycle
//    * @param l the new Location, the Actor should move to
//    */
//   void setLoc(Vector2 l) {
//     this->_loc.x = l.x;
//     this->_loc.y = l.y;
//   }
//
// private:
//   // Values, that the Actor is currently at
//   Vector2 loc;
//
//   // Values, that the Actor should be next frame
//   Vector2 _loc;
//
//   void FlipTransform2DStates() {
//     loc.x = _loc.x;
//     loc.y = _loc.y;
//   }
// };
//
// // BM: ActorComponent - Ticking - Class
// //==============================================================================
// class Ticking : ActorComponent {
//   friend Stage;
//
// public:
//   Ticking(Actor *ac) : ActorComponent(ac, TICKING) {}
//
// private:
//   virtual void OnTick(Play) = 0;
// };
//
// // BM: ActorComponent - Visible - Class
// //==============================================================================
// class Visible : ActorComponent {
//   friend Stage;
//
// private:
//   virtual void OnDraw(Play) = 0;
//
// public:
//   Visible(Actor *ac) : ActorComponent(ac, VISIBLE) {}
//
//   /**
//    * @brief Sets what elements are drawn above which other
//    * actors with higher numbers will be drawn above elements with lower
//    numbers
//    * Should 2 elements have the same number, there draw order
//    * depends on the order, in which they were made visible
//    *
//    * @param layer
//    */
//   void SetRenderLayer(int layer) { this->_zindex = layer; }
//
// private:
//   int _renderListIndex = -1;
//   int _zindex = 0;
// };
//
// // BM: Scene - Class
// //=============================================================================
// class Scene {
//   friend Stage;
//
// public:
//   virtual void OnStart(Play) {}
//   virtual void OnUpdate(Play) {}
//   virtual void OnStageDraw(Play) {}
//   virtual void OnWindowDraw(Play) {}
//   virtual void OnEnd(Play) {}
//
//   void TransitionTo(Scene *s) {
//     _followup = s;
//     _transitionRequested = true;
//   }
//
//   Scene *followup() { return _followup; }
//
// private:
//   bool _transitionRequested = false;
//   Scene *_followup = NULL;
//
//   bool Tick(Play p) {
//     OnUpdate(p);
//     return !_transitionRequested;
//   }
//
//   Scene *Unload(Play p) {
//     OnEnd(p);
//     _transitionRequested = false;
//     return _followup;
//   }
// };
//
// }; // namespace Theater

#endif // RAYTHEATER_H
