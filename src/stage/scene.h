#ifndef STAGE_SCENE_H 
#define STAGE_SCENE_H 1

#include <stdlib.h>
#include <stdbool.h>

#include "play.h"
#include "entity.h"

namespace Stage {

    class Scene : public Entity {
    public:

        Scene();

        virtual void OnLoad(Play);
        virtual Scene* OnUnload(Play);
        virtual void OnStageDraw(Play);
        virtual void OnWindowDraw(Play);

        virtual bool OnTick(Play);

        virtual void OnStageRemove(Play);
    };

}

#endif 
