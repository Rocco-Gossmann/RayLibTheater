#include "./stage/theater.h"
#include "./scenes/main.h"


int main(int argc, char** argv) {
        
    Scenes::MainScene scMain;

    Stage::Builder(320, 200, 2.0)
        .Title("RayLib - Window 🎉") 
        .Play(&scMain);

    return 0;
}
