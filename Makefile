test: test.cpp
	g++ -g -std=c++11 test.cpp -o test `Magick++-config --cxxflags --cppflags` `Magick++-config --ldflags --libs` `pkg-config --cflags --libs opencv`   -lleptonica -ltesseract

detect_table.o: detect_table.cpp
	g++ -g -std=c++11 -c detect_table.cpp

detect_cell.o: detect_cell.cpp
	g++ -g -std=c++11 -c detect_cell.cpp

find_grid2.o: find_grid2.cpp
	g++ -g -std=c++11 -c find_grid2.cpp

textbox_ocr.o: textbox_ocr.cpp
	g++ -g -std=c++11 -c textbox_ocr.cpp

track: track.cpp detect_table.o detect_cell.o find_grid2.o textbox_ocr.o
	g++ -g -std=c++11 -o track track.cpp detect_table.o detect_cell.o textbox_ocr.o find_grid2.o `Magick++-config --cxxflags --cppflags` `Magick++-config --ldflags --libs` `pkg-config --cflags --libs opencv` -llept -ltesseract

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

testRTree: testRTree.cpp
	g++ -std=c++11 testRTree.cpp -o testRTree -I./RTree

rrbb: rrbb.cpp
	g++ -g -std=c++11 rrbb.cpp -o rrbb -I./RTree `pkg-config --cflags --libs opencv`
