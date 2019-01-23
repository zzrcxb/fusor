add_library(FusorPass MODULE
    # List your source files here.
        fusor.cpp pred/array.cpp utils.cpp trans/bogus.cpp pred/float.cpp trans/second.cpp trans/flatten.cpp trans/dataflow.cpp)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(FusorPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that.
set_target_properties(FusorPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
