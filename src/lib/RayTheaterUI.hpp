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
  enum ButtonEvent { BTN_HOVER, BTN_PRESS, BTN_HOLD, BTN_RELEASE, BTN_OUT };

  typedef std::function<void(int, ButtonEvent, Button *)> ButtonEventHandler;

  struct UIStyle {
    Color textColor = WHITE;
    Color backgroundColor = GRAY;
    Vector2 labelOffset = {2, 2};
    float labelFontSize = 10;
    Font font = GetFontDefault();
  };

  Button(int id, float x, float y, float w, float h);

  Button *Label(std::string str);

  Button *Style(UIStyle *);

  Button *OnHover(ButtonEventHandler *h);
  Button *OnPress(ButtonEventHandler *h);
  Button *OnHold(ButtonEventHandler *h);
  Button *OnRelease(ButtonEventHandler *h);
  Button *OnOut(ButtonEventHandler *h);

  Button *OnEvent(ButtonEventHandler *h);

private:
  static UIStyle defaultStyle;

  UIStyle *_style;
  bool _showBG = true;
  bool _showLabel = true;

  enum ButtonState { STATE_IDLE, STATE_ACTIVATE, STATE_HELD };
  ButtonState _state;

  int _id;
  Rectangle _drawRect;
  std::string _label;
  Vector2 _labelPosition;

  ButtonEventHandler *_hoverhandler = NULL;
  ButtonEventHandler *_presshandler = NULL;
  ButtonEventHandler *_holdhandler = NULL;
  ButtonEventHandler *_releasehandler = NULL;
  ButtonEventHandler *_outhandler = NULL;

  // Helpers
  //==============================================================================
  void rerender();

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
// BM: Button - Implementation
//==============================================================================

inline Button::UIStyle Button::defaultStyle = {};

inline Button::Button(int id, float x, float y, float w, float h)
    : Actor(), Visible(this), Ticking(this), _drawRect({x, y, w, h}), _id(id),
      _label(std::to_string(id)), _state(STATE_IDLE), _style(&defaultStyle) {
  rerender();
}

inline void Button::rerender() {
  this->_showBG = (_style->backgroundColor.a + _style->backgroundColor.r +
                   _style->backgroundColor.g + _style->backgroundColor.b) > 0;

  this->_showLabel = (_style->textColor.a + _style->textColor.r +
                      _style->textColor.g + _style->textColor.b) > 0 &&
                     this->_label.size() > 0;

  this->_labelPosition.x = this->_drawRect.x + this->_style->labelOffset.x;
  this->_labelPosition.y = this->_drawRect.y + this->_style->labelOffset.y;
}

inline Rectangle Button::getRect() { return _drawRect; }

inline void Button::OnDraw(Play p) {
  if (_showBG)
    DrawRectangleRec(_drawRect, _style->backgroundColor);

  if (_showLabel)
    DrawTextPro(_style->font, _label.c_str(), this->_labelPosition, {0, 0}, 0,
                _style->labelFontSize, 1, _style->textColor);
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
      } else {
        if (_releasehandler != NULL) {
          (*_releasehandler)(_id, BTN_RELEASE, this);
        }
        _state = STATE_IDLE;
      }
    }

  } else {

    switch (_state) {
    case STATE_ACTIVATE:
    case STATE_HELD:
      if (_outhandler != NULL) {
        (*_outhandler)(_id, BTN_OUT, this);
      }
      _state = STATE_IDLE;
    default:
      break;
    }
  }

  return true;
}

inline void Button::OnStageEnter(Play p) { p.stage->MakeActorVisible(this); }
inline void Button::OnStageLeave(Play p) { p.stage->MakeActorInvisible(this); }

//==============================================================================
// BM: Button - Implementation - Setters
//==============================================================================
inline Button *Button::OnHover(ButtonEventHandler *h) {
  _hoverhandler = h;
  return this;
}
inline Button *Button::OnPress(ButtonEventHandler *h) {
  _presshandler = h;
  return this;
}
inline Button *Button::OnHold(ButtonEventHandler *h) {
  _holdhandler = h;
  return this;
}
inline Button *Button::OnRelease(ButtonEventHandler *h) {
  _releasehandler = h;
  return this;
}
inline Button *Button::OnOut(ButtonEventHandler *h) {
  _outhandler = h;
  return this;
}

inline Button *Button::OnEvent(ButtonEventHandler *h) {
  _hoverhandler = h;
  _presshandler = h;
  _holdhandler = h;
  _releasehandler = h;
  _outhandler = h;
  return this;
}

inline Button *Button::Label(std::string str) {
  _label = str;
  this->rerender();
  return this;
}

inline Button *Button::Style(Button::UIStyle *s) {
  this->_style = s;
  this->rerender();
  return this;
}

} // namespace UI
} // namespace Theater

#endif // RayTheaterUI
