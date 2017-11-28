#demo: demo.cpp
#	g++ -std=c++11 -o demo demo.cpp `pkg-config --cflags --libs opencv`   -lleptonica `Magick++-config --cxxflags --cppflags` -ltesseract `Magick++-config --ldflags --libs`
demo: demo.cpp detect_table.hpp detect_cell.hpp
	g++ -std=c++11 -o demo demo.cpp `pkg-config --cflags --libs opencv` -lleptonica -ltesseract

