add_library(detect_table rrbb.cpp heuristic_filter.cpp mla.cpp mlc.cpp recursive_filter.cpp noise.cpp detect.cpp image_primitives.cpp utility.cpp homogenous_regions.cpp text_tools.cpp rlt_detect.cpp nrlt_detect.cpp debug_header.cpp)
target_link_libraries(detect_table util)
