#include "raylib.h"
#include <string>
#ifndef RayTheaterUI
#define RayTheaterUI 1

#include "RayTheater.hpp"

namespace Theater {
namespace UI {

//==============================================================================
// BM: Button - Class
//==============================================================================

class Button : public Actor,
               public ColliderRect,
               public Visible,
               public Ticking {

public:
  enum ButtonEvent { BTN_HOVER, BTN_PRESS, BTN_HOLD, BTN_RELEASE };
  enum ButtonState { STATE_IDLE, STATE_ACTIVATE, STATE_HELD };

  typedef std::function<void(int, ButtonEvent, Button *)> ButtonEventHandler;

  Button(int id, float x, float y, float w, float h);

  Button *OnHover(ButtonEventHandler *h) {
    _hoverhandler = h;
    return this;
  }
  Button *OnPress(ButtonEventHandler *h) {
    _presshandler = h;
    return this;
  }
  Button *OnHold(ButtonEventHandler *h) {
    _holdhandler = h;
    return this;
  }
  Button *OnRelease(ButtonEventHandler *h) {
    _releasehandler = h;
    return this;
  }

  Button *Label(std::string str) {
    _label = str;
    return this;
  }

private:
  ButtonState _state;

  int _id;
  Rectangle _drawRect;
  std::string _label;
  Vector2 _labelOffset;
  float _labelSize;

  Font _font;

  Color _drawFGColor;
  Color _drawBGColor;

  ButtonEventHandler *_hoverhandler = NULL;
  ButtonEventHandler *_presshandler = NULL;
  ButtonEventHandler *_holdhandler = NULL;
  ButtonEventHandler *_releasehandler = NULL;

  // Helpers
  //==============================================================================
  void backToIdle();

  // Interfaces
  //==============================================================================
public:
  // Implement - ColliderRect
  //------------------------------------------------------------------------------
  Rectangle getRect() override;

  // Implement - Visible
  //------------------------------------------------------------------------------
  void OnDraw(Play) override;

  // Implement - Tickable
  //------------------------------------------------------------------------------
  bool OnTick(Play) override;

  // Implement - Actor
  //------------------------------------------------------------------------------
  void OnStageEnter(Play) override;
  void OnStageLeave(Play) override;
};

//==============================================================================
// BM: Button-Implementation
//==============================================================================

inline Button::Button(int id, float x, float y, float w, float h)
    : Actor(), Visible(this), Ticking(this), _drawRect({x, y, w, h}), _id(id),
      _font(GetFontDefault()), _label(std::to_string(id)), _drawFGColor(WHITE),
      _state(STATE_IDLE), _drawBGColor(GRAY), _labelOffset({x, y}),
      _labelSize(8) {}

inline Rectangle Button::getRect() { return _drawRect; }

inline void Button::OnDraw(Play p) {
  DrawRectangleRec(_drawRect, _drawBGColor);
  DrawTextPro(_font, _label.c_str(), _labelOffset, {0, 0}, 0, _labelSize, 1,
              _drawFGColor);
}

inline bool Button::OnTick(Play p) {

  if (containsPoint(p.mouseX, p.mouseY)) {

    switch (_state) {
    case STATE_IDLE:
      if (p.mouseHeld || p.mouseDown) {
        if (_presshandler != NULL) {
          (*_presshandler)(_id, BTN_PRESS, this);
        }
        _state = STATE_ACTIVATE;

      } else if (_hoverhandler != NULL) {
        (*_hoverhandler)(_id, BTN_HOVER, this);
      }
      break;

    case STATE_HELD:
    case STATE_ACTIVATE:
      if (p.mouseHeld || p.mouseDown) {
        if (_holdhandler != NULL) {
          (*_holdhandler)(_id, BTN_HOLD, this);
        }
        _state = STATE_HELD;
      } else
        backToIdle();
    }

  } else {

    switch (_state) {
    case STATE_ACTIVATE:
    case STATE_HELD:
      backToIdle();
    default:
      break;
    }
  }

  return true;
}

inline void Button::backToIdle() {
  if (_releasehandler) {
    (*_releasehandler)(_id, BTN_RELEASE, this);
  }
  _state = STATE_IDLE;
}

inline void Button::OnStageEnter(Play p) { p.stage->MakeActorVisible(this); }
inline void Button::OnStageLeave(Play p) { p.stage->MakeActorInvisible(this); }

} // namespace UI
} // namespace Theater

#endif // RayTheaterUI
