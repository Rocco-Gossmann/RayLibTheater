#include "debug.h"
#include <iostream>

namespace Actors {

bool Debug::OnTick(Stage::Play p) {

    std::cout << " The Deeeeee Bug is crawling " << std::endl;
    return true; 

}



}
