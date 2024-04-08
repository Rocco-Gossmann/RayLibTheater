#ifndef STAGE_STAGE_H 
#define STAGE_STAGE_H 1

#include <stdbool.h>

/**
 * @brief return the last error, in case anything goes wrong anyehere
 *
 * @return a message describing the error
 */
void StagePrintLastError();


/**
 * @brief 
 *
 * @param width  - the stages with in pixels
 * @param height - the stages height in pixels
 * @param title  - title of the window
 *
 * @return true on succes else false (call PrintLastError() for more info)
 */
bool StageBuild(int width, int height, const char *title);



/**
 * @brief 
 *
 * @param width  - the stages with in pixels
 * @param height - the stages height in pixels
 * @param title  - title of the window
 * @param scale  - multiplier by which the window will be scaled (e.g. 2.0 = each pixlel of the stage, takes 2 pixels in the Window)
 *
 * @return true on succes else false (call PrintLastError() for more info)
 */
bool StageBuildScale(int width, int height, const char *title, float scale);

void StagePlay();

#endif
