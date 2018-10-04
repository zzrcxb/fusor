//
// Created by neil on 10/4/18.
//

#ifndef PROJECT_UTILS_HPP
#define PROJECT_UTILS_HPP

#define True true
#define False false // For Python lovers!
#define IN_SET(ELEM, SET) (SET.find(ELEM) != SET.end())  // STL sucks!!!
#define IN_MAP(KEY, MAP) (MAP.find(KEY) != MAP.end())    // STL sucks!!!
#define ISINSTANCE(OBJ_P, CLASS) (dyn_cast<CLASS>(OBJ_P))  // C++ sucks!!!

#endif //PROJECT_UTILS_HPP
