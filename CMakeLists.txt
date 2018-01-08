cmake_minimum_required(VERSION 3.5)

project(track)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

find_package( OpenCV REQUIRED )
find_package(ImageMagick COMPONENTS Magick++)

include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/rrbb" "${PROJECT_SOURCE_DIR}/RTree" "${PROJECT_SOURCE_DIR}/celldetection" "${PROJECT_SOURCE_DIR}/ocr" "${PROJECT_SOURCE_DIR}/trex" "${ImageMagick_INCLUDE_DIRS}")
add_subdirectory(rrbb)
add_subdirectory(celldetection)
add_subdirectory(ocr)
add_subdirectory(trex)

add_executable(track ${PROJECT_SOURCE_DIR}/track.cpp)

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

target_link_libraries(track detect_table detect_cell textbox_content find_grid leptonica tesseract ${OpenCV_LIBS} ${ImageMagick_LIBRARIES})
