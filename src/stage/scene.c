#include "./scene.h"

#include <stdio.h>
#include <stddef.h>


Scene* NewScene(
    GenericSceneFunction *OnLoad
,    SceneTickFunction *OnTick
,    GenericSceneFunction *OnStageDraw
,    GenericSceneFunction *OnWindowDraw
,    SceneUnloadFunction *OnUnload
) {
    Scene* sc = (Scene*)malloc(sizeof(Scene));
    return sc;
}

void DestroyScene(Scene *s) {
    if (s != NULL) {
        printf("destroying Scene: %p\n", s);
        free(s);
    }
}

