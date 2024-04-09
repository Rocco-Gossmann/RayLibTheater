#include <unordered_map>
#include <unordered_set>
#ifndef STAGE_STAGE_H
#define STAGE_STAGE_H 1

#include <stdbool.h>
#include "raylib.h"
#include "entity.h"
#include "scene.h"
#include "play.h"

namespace Stage {


class Stage {
public:
    /**
       * @brief
       *
       * @param width  - the stages with in pixels
       * @param height - the stages height in pixels
       * @param scale  - multiplier by which the window will be scaled (e.g. 2.0 =
       * each pixlel of the stage, takes 2 pixels in the Window)
       */
    Stage(int width, int height, float scale = 1.0);

    ~Stage();

    void Play(Scene*);

    Stage Title(const char*);
    Stage BorderColor(Color);
    Stage BackgroundColor(Color);

    void AddAttributeToEntity(Entity*, Entity::Attributes a);

private:
    const char* _stageTitle;
    float _stageWidth;
    float _stageHeight;
    RenderTexture2D _stage;
    Scene* _scene;
    Rectangle _viewportRect;
    Rectangle _stageRect;
    Vector2 _viewportOrigin;
    
    Color _borderColor;
    Color _backgroundColor;

    std::unordered_set<Entity*> _tickables;

    ::Stage::Play _play;

    void switchScene(Scene*);
    void onResize();

    std::unordered_map<Entity::Attributes, std::unordered_set<Entity*>> _attributelists;
};

} // namespace Stage

#endif
