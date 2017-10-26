cmake_minimum_required (VERSION 3.8)

file(GLOB FIX8_SRC
    "include/*.h"
    "include/*.hpp"
    "src/*.cpp"
)

include_directories(${Mette_ROOT}/poco/Net/include)
include_directories(${Mette_ROOT}/poco/Foundation/include)
include_directories(${Mette_ROOT}/tbb/include/)
include_directories(${Mette_ROOT}/fix8/include/fix8)

add_library(FIX8 ${FIX8_SRC})
include_directories(include/)

message("Done Processing Dir [" ${CMAKE_CURRENT_SOURCE_DIR} "]")

