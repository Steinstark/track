#track: track.cpp
#	g++ -std=c++11 -o track track.cpp `pkg-config --cflags --libs opencv`   -lleptonica `Magick++-config --cxxflags --cppflags` -ltesseract `Magick++-config --ldflags --libs`

detect_table.o: detect_table.cpp
	g++ -g -std=c++11 -c detect_table.cpp

detect_cell.o: detect_cell.cpp
	g++ -g -std=c++11 -c detect_cell.cpp

find_grid.o: find_grid.cpp
	g++ -g -std=c++11 -c find_grid.cpp

track: track.cpp find_grid.cpp detect_table.o detect_cell.o find_grid.o
	g++ -g -std=c++11 -o track track.cpp find_grid.o detect_table.o detect_cell.o `pkg-config --cflags --libs opencv` -lleptonica -ltesseract

detect_table: detect_table.cpp
	g++ -std=c++11 detect_table.cpp -o detect_table `pkg-config --cflags --libs opencv`
detect_cell_1: detect_cell_1.cpp
	g++ -std=c++11 detect_cell_1.cpp -o detect_cell_1 `pkg-config --cflags --libs opencv`
detect_cell_2: detect_cell_2.cpp
	g++ -g -std=c++11 detect_cell_2.cpp -o detect_cell_2 `pkg-config --cflags --libs opencv`
detect_cell_exp: detect_cell_exp.cpp
	g++ -g -std=c++11 detect_cell_exp.cpp -o detect_cell_exp `pkg-config --cflags --libs opencv`
	./detect_cell_exp
find_grid: find_grid.cpp
	g++ -std=c++11 find_grid.cpp -o find_grid `pkg-config --cflags --libs opencv`

