# Theater::Timer

Timers are an essential tool for game development.
be it for animations or game events and logic.

RayTheater comes with the `Theater::Timer` class, 
that can be used to schedule function executions based on times passed.

## How to use it

```c++
#include "RayTheater.hpp"
#include <raylib.h>
//...
class MyScene : public Theater::Scene {
private:
    Theater::Timer m_timer;

public:
    void OnStart(Theater::Play p) override{

    // Set the timer duration
        m_timer.setDuration(2000);

    // define, what happens when it runs out
        m_timer.onFinish([this](Theater::Timer* t) {
            std::cout << "Timer finished!" << std::endl;
        });

    // Start the Timer
        p.stage->setTimer(&m_timer);

    }


    // make sure to remove the timer when leaving the scene
    void OnEnd(Theater::Play p) override{
        p.stage->removeTimer(&m_timer);
    }
}
```
## Tracking Timer - Progression




