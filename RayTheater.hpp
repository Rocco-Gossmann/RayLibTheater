#ifndef RAYTHEATER_H
#define RAYTHEATER_H

#include "./internal/macros.h"
#include "./internal/types.h"

#include "./internal/ActorAttributes.h"
#include "./internal/Builder.hpp"
#include "./internal/Play.h"
#include "./internal/Stage.hpp"
#include "./internal/Timer.hpp"

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
// }; // namespace Theater

#endif // RAYTHEATER_H
