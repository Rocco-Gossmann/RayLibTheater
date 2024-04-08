#ifndef STAGE_SCENE_H 
#define STAGE_SCENE_H 1

#include <stdlib.h>
#include <stdbool.h>

typedef void GenericSceneFunction(void);
typedef bool SceneTickFunction(void);
typedef void* SceneUnloadFunction(void);

typedef struct{
    GenericSceneFunction *OnLoad;
    SceneTickFunction *OnTick;
    GenericSceneFunction *OnStageDraw;
    GenericSceneFunction *OnWindowDraw;
    SceneUnloadFunction *OnUnload;
} Scene;


Scene* NewScene(
    GenericSceneFunction *OnLoad
,    SceneTickFunction *OnTick
,    GenericSceneFunction *OnStageDraw
,    GenericSceneFunction *OnWindowDraw
,    SceneUnloadFunction *OnUnload
);
void DestroyScene(Scene *s);

#endif 
