#include "raylib.h"
#include <string>
#include <type_traits>
#ifndef RayTheaterUI
#define RayTheaterUI 1

#include "RayTheater.hpp"

namespace Theater {
namespace UI {

// BM: Label - Class
//==============================================================================
class Label : public Actor, public Visible {
public:
  struct UIStyle {
    Color textColor = WHITE;
    float fontSize = 10;
    Font font = GetFontDefault();
  };

  Label();
  Label(std::string);

  Label *Text(std::string);
  Label *Style(UIStyle *);
  Label *Position(float x, float y);

private:
  Vector2 _pos;
  UIStyle *_style;
  std::string _text;

public:
  // Implement - Visible
  //------------------------------------------------------------------------------
  void OnDraw(Play) override;
};

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
    Font font = GetFontDefault();
    float fontSize = 10;

    Color backgroundColor = GRAY;
    Vector2 labelOffset = {2, 2};
    float cornorRadius = .25;

    bool roundTL = true;
    bool roundTR = true;
    bool roundBL = true;
    bool roundBR = true;
  };

  Button(int id, float x, float y, float w, float h);
  ~Button();

  Button *Label(std::string str);

  Button *Style(UIStyle *);

  Button *OnHover(ButtonEventHandler *h);
  Button *OnPress(ButtonEventHandler *h);
  Button *OnHold(ButtonEventHandler *h);
  Button *OnRelease(ButtonEventHandler *h);
  Button *OnOut(ButtonEventHandler *h);

  Button *OnEvent(ButtonEventHandler *h);

private:
  UIStyle *_style;
  bool _onStage = false;

  enum ButtonState { STATE_IDLE, STATE_ACTIVATE, STATE_HELD };
  ButtonState _state;

  int _id;
  Rectangle _drawRect;
  std::string _label;
  Vector2 _labelPosition;

  Rectangle _srcRect;
  Vector2 _textOrigin;

  ButtonEventHandler *_hoverhandler = NULL;
  ButtonEventHandler *_presshandler = NULL;
  ButtonEventHandler *_holdhandler = NULL;
  ButtonEventHandler *_releasehandler = NULL;
  ButtonEventHandler *_outhandler = NULL;

  RenderTexture2D _texture;

  // Helpers
  //----------------------------------------------------------------------------
  void rerender();

  // Interfaces
  //----------------------------------------------------------------------------
public:
  // Implement - ColliderRect
  //----------------------------------------------------------------------------
  Rectangle getRect() override;

  // Implement - Visible
  //----------------------------------------------------------------------------
  void OnDraw(Play) override;

  // Implement - Tickable
  //----------------------------------------------------------------------------
  void OnTick(Play) override;

  // Implement - Actor
  //----------------------------------------------------------------------------
  void OnStageEnter(Play) override;
  void OnStageLeave(Play) override;
};

// BM: Button - Implementation
//==============================================================================
static Button::UIStyle defaultButtonStyle = {};

inline Button::Button(int id, float x, float y, float w, float h)
    : Actor(), Visible(this), Ticking(this), _drawRect({x, y, w, h}), _id(id),
      _label(std::to_string(id)), _state(STATE_IDLE),
      _style(&defaultButtonStyle), _srcRect({0, 0, w, -h}),
      _textOrigin({0, 0}) {}

inline Button::~Button() { UnloadRenderTexture(_texture); }

inline void Button::rerender() {
  if (!_onStage)
    return;

  this->_labelPosition.x = this->_drawRect.x + this->_style->labelOffset.x;
  this->_labelPosition.y = this->_drawRect.y + this->_style->labelOffset.y;

  BeginTextureMode(_texture);
  ClearBackground(BLANK);

  float rad = fmin(this->_drawRect.width, this->_drawRect.height) *
              (this->_style->cornorRadius * 0.5);
  float rad2 = rad * 2;

  if ((_style->backgroundColor.a + _style->backgroundColor.r +
       _style->backgroundColor.g + _style->backgroundColor.b) > 0) {

    if (rad > 0) {
      if (_style->roundTL)
        DrawCircle(rad, rad, rad, this->_style->backgroundColor);
      else
        DrawRectangle(0, 0, rad2, rad2, this->_style->backgroundColor);

      if (_style->roundTR)
        DrawCircle(this->_drawRect.width - rad, rad, rad,
                   this->_style->backgroundColor);
      else
        DrawRectangle(0, this->_drawRect.width - rad2, rad2, rad2,
                      this->_style->backgroundColor);

      if (_style->roundBL)
        DrawCircle(rad, this->_drawRect.height - rad, rad,
                   this->_style->backgroundColor);
      else
        DrawRectangle(0, this->_drawRect.height - rad2, rad2, rad2,
                      this->_style->backgroundColor);

      if (_style->roundBR)
        DrawCircle(this->_drawRect.width - rad, this->_drawRect.height - rad,
                   rad, this->_style->backgroundColor);

      else
        DrawRectangle(this->_drawRect.width - rad2,
                      this->_drawRect.height - rad2, rad2, rad2,
                      this->_style->backgroundColor);

      DrawRectangle(rad, 0, this->_drawRect.width - rad2,
                    this->_drawRect.height, this->_style->backgroundColor);
    }

    DrawRectangle(0, rad, this->_drawRect.width, this->_drawRect.height - rad2,
                  this->_style->backgroundColor);
  }

  if ((_style->textColor.a + _style->textColor.r + _style->textColor.g +
       _style->textColor.b) > 0 &&
      this->_label.size() > 0)
    DrawTextPro(_style->font, _label.c_str(), this->_style->labelOffset, {0, 0},
                0, _style->fontSize, 1, _style->textColor);

  EndTextureMode();
}

inline Rectangle Button::getRect() { return _drawRect; }

inline void Button::OnDraw(Play p) {
  DrawTexturePro(_texture.texture, _srcRect, _drawRect, _textOrigin, 0, WHITE);
}

inline void Button::OnTick(Play p) {

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
}

inline void Button::OnStageEnter(Play p) {
  _texture = LoadRenderTexture(_drawRect.width, _drawRect.height);
  _onStage = true;
  rerender();
  p.stage->MakeActorVisible(this);
}
inline void Button::OnStageLeave(Play p) {
  p.stage->MakeActorInvisible(this);
  _onStage = false;
  UnloadRenderTexture(_texture);
}

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

//==============================================================================
// BM: Label - Implementation
//==============================================================================
static Label::UIStyle defaultLabelStyle = {};

inline Label::Label()
    : Actor(), Visible(this), _style(&defaultLabelStyle), _pos({0, 0}),
      _text(" - ") {}
inline Label::Label(std::string txt) : Label() { _text = txt; };

inline Label *Label::Text(std::string s) {
  _text = s;
  return this;
}
inline Label *Label::Position(float x, float y) {
  this->_pos.x = x;
  this->_pos.y = y;
  return this;
}
inline Label *Label::Style(UIStyle *s) {
  if (s == NULL)
    _style = &defaultLabelStyle;
  else
    _style = s;

  return this;
};

inline void Label::OnDraw(Play p) {
  DrawTextPro(_style->font, _text.c_str(), _pos, {0, 0}, 0, _style->fontSize, 1,
              _style->textColor);
}
} // namespace UI
} // namespace Theater

#endif // RayTheaterUI
