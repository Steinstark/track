cmake_minimum_required(VERSION 3.5)

project(track)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS "-g")

find_package(PkgConfig REQUIRED)
find_package( OpenCV REQUIRED )
pkg_check_modules(MAGICK REQUIRED Magick++)

include_directories(${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/rrbb" "${PROJECT_SOURCE_DIR}/RTree" "${PROJECT_SOURCE_DIR}/celldetection" "${PROJECT_SOURCE_DIR}/ocr" "${PROJECT_SOURCE_DIR}/utility" "${PROJECT_SOURCE_DIR}/trex" ${MAGICK_INCLUDE_DIRS})

add_subdirectory(utility)
add_subdirectory(rrbb)
add_subdirectory(celldetection)
add_subdirectory(ocr)
add_subdirectory(trex)

add_executable(track ${PROJECT_SOURCE_DIR}/track.cpp)

target_compile_options(track PUBLIC ${MAGICK_CFLAGS_OTHER})
target_link_libraries(track detect_table detect_cell textbox_content find_grid util tree_helper leptonica tesseract ${OpenCV_LIBS} ${MAGICK_LIBRARIES})
