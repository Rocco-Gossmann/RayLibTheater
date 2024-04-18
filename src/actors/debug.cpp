#include "debug.h"
#include <iostream>

namespace Actors {

using namespace Stage;

DebugActor::DebugActor() : 
  Actor(),
  Transform2D(this),
  Ticking(this)
{ }  


bool DebugActor::OnTick(Play p) {
  std::cout << " the deeebug is crawling " << std::endl;
  
  return false;
};



}
