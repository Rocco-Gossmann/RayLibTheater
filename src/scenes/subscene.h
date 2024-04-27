#ifndef SCENES_SUBSCENE_H
#define SCENES_SUBSCENE_H 1

#include "../lib/RayTheater.hpp"

using namespace Theater;

namespace Scenes {

class SubScene : public Scene {

public:
  void TransitionTo(SubScene *s) {
    _followup = s;
    _hasFollowup = true;
  }

  SubScene *followup() { return _followup; }

  virtual bool OnTick(Play p) { return !_hasFollowup; }
  virtual Scene *OnUnload(Play p) {
    _hasFollowup = false;
    return _followup;
  }

private:
  bool _hasFollowup = false;
  SubScene *_followup = NULL;
};

} // namespace Scenes

#endif // SCENES_SUBSCENE_H
