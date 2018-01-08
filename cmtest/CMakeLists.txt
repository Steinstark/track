cmake_minimum_required(VERSION 3.5)

project(track)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/ocr")
add_subdirectory(ocr)

add_executable(testprog ${PROJECT_SOURCE_DIR}/test.cpp)
target_link_libraries(testprog ocr)
