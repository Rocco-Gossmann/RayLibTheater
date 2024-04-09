#include "./stage/stage.h"
#include "./scenes/main.h"


int main(int argc, char** argv) {
        
    Scenes::MainScene scMain;

    Stage::Stage(320, 200, 2.0)
        .Play(&scMain);

    return 0;
}
