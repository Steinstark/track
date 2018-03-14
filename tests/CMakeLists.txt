add_library(test_utils test_utils.cpp dataset_utils.cpp)
add_executable(detectiontest tableDetectionTest.cpp)
target_link_libraries(detectiontest test_utils detect_table tree_helper xml image_util ${Boost_LIBRARIES} ${OpenCV_LIBS})
