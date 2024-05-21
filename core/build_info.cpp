#include "c.h"
#include "build_info.h"

extern "C" {

const char* us_library_build_info() { return BUILD_INFO; }

}  // end extern "C"
