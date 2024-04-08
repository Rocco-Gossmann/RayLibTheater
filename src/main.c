#include <stdio.h>

#include "./stage/scene.h"

void onMainLoad() {
    printf("main load\n");
}

int main(int argc, char** argv) {

    Scene *sc = NewScene(
        &onMainLoad,
        0, 0, 0, 0
    );

    printf("scene: %p\n", sc);

    DestroyScene(sc);
    return 0;
}
