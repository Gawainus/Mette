cmake_minimum_required (VERSION 3.8)


include_directories(${Mette_ROOT}/tbb/include/)
#include_directories(${Mette_ROOT}/tbb/src/)
#include_directories(${Mette_ROOT}/tbb/src/rml/include/)

file(GLOB TBB_SRC
    "src/tbb/concurrent_queue.cpp"
)

add_library(tbb ${TBB_SRC})


message("Done Processing Dir [" ${CMAKE_CURRENT_SOURCE_DIR} "]")
