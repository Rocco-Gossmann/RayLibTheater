#include "stage.h"
#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

bool _stage_build = false;
const char* _lastError = "";


bool StageBuild(int width, int height, const char *title) {
    return StageBuildScale(width, height, title, 1.0);
}
bool StageBuildScale(int width, int height, const char *title, float scale) {
    InitWindow(width * scale, height * scale, title); 
    return true;
}





void StagePlay() {
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Yo, this is your first RayLib Window", 8, 8, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
}







void StagePrintLastError() {
    printf("Last Stage-Error: %s", _lastError);
}




