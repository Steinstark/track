cmake_minimum_required(VERSION 3.5)

project(track)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS "-g")

find_package( PkgConfig REQUIRED)
find_package( OpenCV REQUIRED )
find_package( Boost COMPONENTS filesystem REQUIRED )
pkg_check_modules(MAGICK REQUIRED Magick++)
pkg_check_modules(LEPT REQUIRED lept)

include_directories(${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/rrbb" "${PROJECT_SOURCE_DIR}/celldetection" "${PROJECT_SOURCE_DIR}/ocr" "${PROJECT_SOURCE_DIR}/utility" "${PROJECT_SOURCE_DIR}/trex" "${PROJECT_SOURCE_DIR}/tests" ${MAGICK_INCLUDE_DIRS})

add_subdirectory(utility)
add_subdirectory(rrbb)
add_subdirectory(celldetection)
add_subdirectory(ocr)
add_subdirectory(trex)
add_subdirectory(tests)

add_executable(track ${PROJECT_SOURCE_DIR}/track.cpp)


target_compile_options(track PUBLIC ${MAGICK_CFLAGS_OTHER})
target_link_libraries(track detect_table detect_cell textbox_content find_grid util image_util tree_helper ${LEPT_LIBRARIES} tesseract ${OpenCV_LIBS} ${MAGICK_LIBRARIES})

project(imagegen)
add_executable(imagegen ${PROJECT_SOURCE_DIR}/imagegen.cpp)
target_link_libraries(imagegen detect_table detect_cell textbox_content find_grid util image_util tree_helper ${LEPT_LIBRARIES} tesseract ${OpenCV_LIBS})

#project(detectiontest)
#add_executable(detectiontest ${PROJECT_SOURCE_DIR}/tableDetectionTest.cpp)
#target_link_libraries(detectiontest test_utils ${Boost_LIBRARIES} ${OpenCV_LIBS})
