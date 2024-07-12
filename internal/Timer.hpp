#ifndef RAYTHEATER_TIMER_H
#define RAYTHEATER_TIMER_H

#include "./internal/Play.h"
#include <functional>

namespace Theater {

// BM: Class
//==============================================================================
class Timer {
public:
  Timer();

  typedef std::function<void(Timer *, float passTime, float goalTime)>
      t_TimerProgressHandler;

  typedef std::function<void(Timer *)> t_TimerFinishHandler;

  /** @brief if this function is called, this timer will restart
   * when ever the it reaches its end (otherwide it is removed from the stage)
   */
  Timer *loop();

  /** @brief restarts the timer for the given number of milliseconds */
  Timer *setDuration(float milliseconds);

  /** @brief changes the timers target time, but keeps the timer running (think
   * of it as advancing a kitchen timer, without letting it run out first) */
  Timer *updateDuration(float milliseconds);

  /** @brief set a handler for processing timer progress
   * @param progressCooldown - min delay between handler calls
   * @param handler - lambda function called on each progress step
   */
  Timer *onProgress(float progressCooldown, t_TimerProgressHandler handler);

  /** @brief sets the handler to be called, once the timer finished
   * @param handler
   */
  Timer *onFinish(t_TimerFinishHandler handler);

private:
  /** @brief if true, reset timer to 0 on next stageUpdate */
  bool m_ResetRequested;

  /** @brief keeps track of that the current timers goal is*/
  float m_TimerGoal;

  /** @brief keeps track of that the current timers goal is*/
  float m_ActiveTimerGoal;

  /** @brief Milliseconds passed since the timer started */
  float m_TimerValue;

  /** @brief defines if the timer restarts automatcially */
  bool m_loop;

  /** @brief called, once the fimer reaches its target */
  t_TimerFinishHandler m_OnFinish;

  /** @brief called, once the fimer reaches its target */
  t_TimerProgressHandler m_OnProgress;

  /** @brief milliseconds to wait, before sending the next progress event */
  float m_ProgressResolution = 0;

  /** @brief keeps track of at what progress to fire the next Event */
  float m_NextProgress = 0;

private:
  friend class Stage;
  bool stageUpdate(Play p);
};

// BM: Implementation
//==============================================================================
inline Timer::Timer()
    : m_TimerGoal(0), m_TimerValue(0), m_ActiveTimerGoal(0),
      m_ResetRequested(false), m_OnFinish([](Timer *t) {}), m_loop(false),
      m_OnProgress([](Timer *o, float t, float g) {}) {}

inline Timer *Timer::loop() {
  m_loop = true;
  return this;
}

inline Timer *Timer::setDuration(float millisecs) {
  m_TimerGoal = millisecs;
  return this;
}

inline Timer *Timer::updateDuration(float millisecs) {
  m_TimerGoal = millisecs;
  return this;
}

inline Timer *Timer::onProgress(float progressCooldown,
                                t_TimerProgressHandler handler) {
  m_ProgressResolution = progressCooldown;
  m_OnProgress = handler;

  return this;
}

inline Timer *Timer::onFinish(t_TimerFinishHandler handler) {
  m_OnFinish = handler;
  return this;
}

inline bool Timer::stageUpdate(Play p) {
  m_ActiveTimerGoal = m_TimerGoal;

  if (m_ResetRequested)
    m_TimerValue = 0;
  else
    m_TimerValue += p.deltaTime * 1000.0;

  if (m_TimerValue >= m_ActiveTimerGoal) {
    m_OnFinish(this);
    if (m_loop) {
      m_TimerValue = 0;
    } else {
      return false;
    }
  } else if (m_TimerValue >= m_NextProgress) {
    m_OnProgress(this, m_TimerValue, m_ActiveTimerGoal);
    m_NextProgress += m_ProgressResolution;
  }

  return true;
}

} // namespace Theater

#endif // RAYTHEATER_TIMER_H
