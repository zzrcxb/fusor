add_library(FusorPass MODULE
    # List your source files here.
        fusor.cpp utils.hpp pred/array.cpp bases.hpp inheritance.hpp utils.cpp trans/bogus.cpp)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(FusorPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that.
set_target_properties(FusorPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
