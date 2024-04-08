#include <stdio.h>

#include "./stage/scene.h"
#include "stage/stage.h"

#include <raylib.h>

void onMainLoad() {
    printf("main load\n");
}

int main(int argc, char** argv) {


    Scene *sc = NewScene(
        &onMainLoad,
        0, 0, 0, 0
    );

    if(StageBuild(640, 360, "Hello Stage")) 
        StagePlay();

    else StagePrintLastError();


    DestroyScene(sc);
    return 0;
}
