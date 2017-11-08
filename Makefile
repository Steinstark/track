#track: track.cpp
#	g++ -std=c++11 -o track track.cpp `pkg-config --cflags --libs opencv`   -lleptonica `Magick++-config --cxxflags --cppflags` -ltesseract `Magick++-config --ldflags --libs`
track: track.cpp detect_table.hpp detect_cell.hpp
	g++ -std=c++11 -o track track.cpp `pkg-config --cflags --libs opencv` -lleptonica -ltesseract

detect_table: detect_table.cpp
	g++ -std=c++11 detect_table.cpp -o detect_table `pkg-config --cflags --libs opencv`
detect_cell_1: detect_cell_1.cpp
	g++ -std=c++11 detect_cell_1.cpp -o detect_cell_1 `pkg-config --cflags --libs opencv`
detect_cell_2: detect_cell_2.cpp
	g++ -g -std=c++11 detect_cell_2.cpp -o detect_cell_2 `pkg-config --cflags --libs opencv`
detect_cell_exp: detect_cell_exp.cpp
	g++ -g -std=c++11 detect_cell_exp.cpp -o detect_cell_exp `pkg-config --cflags --libs opencv`
	./detect_cell_exp
hough: hough.cpp
	g++ -std=c++11 hough.cpp -o hough  `pkg-config --cflags --libs opencv`
contfix: contfix.cpp
	g++ -std=c++11 contfix.cpp -o contfix  `pkg-config --cflags --libs opencv`
contfind: contfind.cpp
	g++ -std=c++11 contfind.cpp -o contfind  `pkg-config --cflags --libs opencv`
