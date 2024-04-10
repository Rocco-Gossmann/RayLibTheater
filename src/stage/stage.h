#include "core/double_linked_list.h"
#include <unordered_map>
#include <unordered_set>
#ifndef STAGE_STAGE_H
#define STAGE_STAGE_H 1

#include <stdbool.h>
#include "raylib.h"
#include "actor.h"
#include "scene.h"
#include "play.h"

namespace Stage {


class Stage {
friend class Builder;

public:
    ~Stage();

    Stage BorderColor(Color);
    Stage BackgroundColor(Color);

    /** 
    * @brief Adds an actor to a group with a given Attribute
    *
    * Actor::TICKING => The Actors OnTick method will be invoked each frame
    * Actor::DEAD => Actor Will be removed from all Groups before next Draw 
    *
    * give an Actor the Actor::DEAD Attribute to remove it from all lists
    *
    * You can also add other Attributes via the "EntityAttributes.hpp"
    */
    void AddActorAttribute(Actor*, Actor::Attributes);

    /** 
    * @brief Removes an Attribute from a given Actor
    */
    void RemoveActorAttribute(Actor*, Actor::Attributes);

    Core::DoubleLinkedList<Actor> MakeActorVisible(Actor*);


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

    ::Stage::Play _play;

    std::unordered_map<Actor::Attributes, std::unordered_set<Actor*>> _attributelists;

    Core::DoubleLinkedList<Actor> _visibleActors;

    Stage(int width, int height, float scale = 1.0);
    void Play(Scene* sc);

    void RemoveActorFromStage(Actor*);
    void switchScene(Scene*);
    void onResize();
};

} // namespace Stage

#endif
