#include "stage.h"
#include "entity.h"
#include <algorithm>
#include <raylib.h>


namespace Stage {

Stage::Stage(const int width, const int height, float scale): 
    _scene(0x0), 
    _viewportOrigin({0, 0}), 
    _stageRect({0, 0, static_cast<float>(width), static_cast<float>(-height)}) ,
    _viewportRect({0, 0, static_cast<float>(width)*scale, static_cast<float>(height)*scale}) ,
    _stageWidth(width), _stageHeight(height), 
    _play({0}), 
    _backgroundColor(Color{0x00, 0x00, 0xAA, 0xff}),
    _borderColor(Color{0x00, 0x88, 0xff, 0xff}),
    _stageTitle("< RayWrapC - Project >")
{ 
    
    #define ENTITY_ATTRIBUTE(name) _attributelists.insert( { Entity::Attributes::name , std::unordered_set<Entity*>() });
    ENTITY_ATTRIBUTE(DEAD)
    #include "../EntityAttributes.hpp"

}

Stage::~Stage() {
    _scene = NULL;
    CloseWindow();
}


Stage Stage::Title(const char* t) {
    _stageTitle = t;
    return *this;
}

Stage Stage::BorderColor(Color c) {
    _borderColor = c;
    return *this;
}
Stage Stage::BackgroundColor(Color c) {
    _backgroundColor = c;
    return *this;
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

    switchScene(sc);

    while (!WindowShouldClose() && _scene != NULL) {
    
        if(IsWindowResized()) 
            onResize();
    
        for (Entity* tickle : _tickables) {
            if (tickle->OnTick(_play)) {

            }
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
        if( sc == NULL )
            sc = _scene->OnUnload(_play);
        else 
            _scene->OnUnload(_play);
    }

    if (sc != NULL) {
        sc->OnLoad(_play);
        _scene = sc;
    }
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

void Stage::AddAttributeToEntity(Entity* e, Entity::Attributes a) {
    if (e == NULL || e->hasAttribute(a)) return;
    e->addAttribute(a);
    _attributelists.at(a).insert(e);
}

} // namespace Stage
