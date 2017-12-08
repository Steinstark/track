
heuristic_filter.o: heuristic_filter.cpp heuristic_filter.hpp
	g++ -g -std=c++11 -c heuristic_filter.cpp -I../RTree `pkg-config --cflags --libs opencv`

homogenous_regions.o: homogenous_regions.cpp homogenous_regions.hpp
	g++ -g -std=c++11 -c homogenous_regions.cpp `pkg-config --cflags --libs opencv`

rrbb: rrbb.cpp heuristic_filter.o homogenous_regions.o
	g++ -g -std=c++11 rrbb.cpp -o rrbb heuristic_filter.o homogenous_regions.o -I../RTree -I../ `pkg-config --cflags --libs opencv`

recursive_filter: recursive_filter.cpp
	g++ -g -std=c++11 recursive_filter.cpp -o recursive_filter -I../RTree -I../ `pkg-config --cflags --libs opencv`
