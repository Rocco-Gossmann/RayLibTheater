#include "stage.h"
#include "actor.h"
#include "core/double_linked_list.h"
#include <algorithm>
#include <cstdio>
#include <raylib.h>
#include <unordered_set>


namespace Stage {

Stage::Stage(const int width, const int height, float scale): 
    _scene(0x0), 
    _viewportOrigin({0, 0}), 
    _stageRect({0, 0, static_cast<float>(width), static_cast<float>(-height)}) ,
    _viewportRect({0, 0, static_cast<float>(width)*scale, static_cast<float>(height)*scale}) ,
    _stageWidth(width), _stageHeight(height), 
    _play({0, NULL}), 
    _backgroundColor(Color{0x00, 0x00, 0xAA, 0xff}),
    _borderColor(Color{0x00, 0x88, 0xff, 0xff}),
    _stageTitle("< RayWrapC - Project >")
{ 
    
    #define ENTITY_ATTRIBUTE(name) _attributelists.insert( { Actor::Attributes::name , std::unordered_set<Actor*>() });
    #include "../EntityAttributes.hpp"
    #include "internal_attribute_list.hpp"

}

Stage::~Stage() {
    _scene = NULL;
}

void Stage::Play(Scene* sc) {

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(_viewportRect.width, _viewportRect.height, _stageTitle); 
    if(!IsWindowReady()) {
        throw "window not ready";
    }
    
    _stage = LoadRenderTexture(_stageWidth, _stageHeight);
    if(!IsRenderTextureReady(_stage)) {
        CloseWindow();
        throw "failed to create render texture"; 
    }

    _play.stage = this;
    switchScene(sc);

    while (!WindowShouldClose() && _scene != NULL) {
    
        if(IsWindowResized()) 
            onResize();

        //TODO: Update delta time in _play;

        // Tick all the actors
        for( auto ticker : _attributelists.at(Actor::TICKING)) {
            if(!ticker->OnTick(_play)) {
                AddActorAttribute(ticker, Actor::DEAD);
            }
        }

        bool sceneOver = _scene->hasAttribute(Actor::DEAD);

        // if the scene is over, switch to the next scene
        if(sceneOver) switchScene(NULL);

        // Otherwise Remove all who should no longer act in the Scene
        for( auto killed : _attributelists.at(Actor::DEAD)) {
            RemoveActorFromStage(killed);                  
        }
        


        BeginTextureMode(_stage);
           ClearBackground(_backgroundColor); 
            _scene->OnStageDraw(_play);
        EndTextureMode();

        BeginDrawing();

            ClearBackground(_borderColor);

            DrawTexturePro(_stage.texture, 
                           _stageRect, 
                           _viewportRect, 
                           _viewportOrigin, 0, WHITE);


            _scene->OnWindowDraw(_play);

        EndDrawing();
    }

    switchScene(0);
    UnloadRenderTexture(_stage);
}

void Stage::switchScene(Scene* sc) {
    if (_scene != NULL) {
        AddActorAttribute(_scene, Actor::DEAD);
        if(sc == NULL) 
            sc = _scene->OnUnload(_play);
        else
            _scene->OnUnload(_play);
    }

    if (sc != NULL) {
        sc->OnLoad(_play);
        _scene = sc;
        RemoveActorAttribute(_scene, Actor::DEAD);
        AddActorAttribute(_scene, Actor::TICKING);
    }
}

Stage Stage::BorderColor(Color c) {
    _borderColor = c;
    return *this;
}

Stage Stage::BackgroundColor(Color c) {
    _backgroundColor = c;
    return *this;
}

void Stage::onResize() {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float scale = std::min(screenWidth / _stageWidth, screenHeight / _stageHeight);

    _viewportRect.width = _stageWidth * scale; 
    _viewportRect.height = _stageHeight * scale; 
    _viewportRect.x = (screenWidth - _viewportRect.width) * 0.5;
    _viewportRect.y = (screenHeight - _viewportRect.height) * 0.5;
}

void Stage::AddActorAttribute(Actor* a, Actor::Attributes at) {
    if (a == NULL || a->hasAttribute(at)) return;
    a->addAttribute(at);
    _attributelists.at(at).insert(a);
}

void Stage::RemoveActorAttribute(Actor* a, Actor::Attributes at) {
    a->removeAttribute(at);

    auto al = _attributelists.at(at);
    auto ac = al.find(a);
    if(ac != al.end()) 
        al.erase(ac);
}

void Stage::RemoveActorFromStage(Actor* a) {
    
    #define ENTITY_ATTRIBUTE(name) RemoveActorAttribute(a, Actor::name);
    #include "../EntityAttributes.hpp"
    #include "./internal_attribute_list.hpp"

}



Core::DoubleLinkedList<Actor> Stage::MakeActorVisible(Actor* a) {
    _visibleActors.Append(a); 
}


} // namespace Stage
