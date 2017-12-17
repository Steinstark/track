test_engine.o: ../tests/test_engine.cpp ../tests/test_engine.hpp
	g++ -g -std=c++11 -c ../tests/test_engine.cpp `Magick++-config --cxxflags --cppflags` `Magick++-config --ldflags --libs` -I../RTree

pugixml.o: ../tests/pugixml.cpp ../tests/pugixml.hpp
	g++ -g -std=c++11 -c ../tests/pugixml.cpp

find_grid: find_grid.cpp find_grid.hpp test_engine.o pugixml.o
	g++ -g -std=c++11 find_grid.cpp test_engine.o pugixml.o -o find_grid `pkg-config --cflags --libs opencv` `Magick++-config --ldflags --libs` -lboost_system -lboost_filesystem -I../tests
