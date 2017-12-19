
heuristic_filter.o: heuristic_filter.cpp heuristic_filter.hpp
	g++ -g -std=c++11 -c heuristic_filter.cpp -I../RTree `pkg-config --cflags --libs opencv`

homogenous_regions.o: homogenous_regions.cpp homogenous_regions.hpp
	g++ -g -std=c++11 -c homogenous_regions.cpp `pkg-config --cflags --libs opencv`

recursive_filter.o: recursive_filter.cpp recursive_filter.hpp
	g++ -g -std=c++11 -c recursive_filter.cpp `pkg-config --cflags --libs opencv` -I../RTree -lboost_system

mla.o: mla.cpp mla.hpp 
	g++ -g -std=c++11 -c mla.cpp `pkg-config --cflags --libs opencv`

mlc.o: mlc.cpp mlc.hpp 
	g++ -g -std=c++11 -c mlc.cpp `pkg-config --cflags --libs opencv`

tree_helper.o: tree_helper.cpp tree_helper.hpp
	g++ -g -std=c++11 -c tree_helper.cpp `pkg-config --cflags --libs opencv` -I../RTree

noise.o: noise.cpp noise.hpp
	g++ -g -std=c++11 -c noise.cpp `pkg-config --cflags --libs opencv` -I../RTree

utility.o: utility.cpp utility.hpp
	g++ -g -std=c++11 -c utility.cpp `pkg-config --cflags --libs opencv` -I../RTree	

rrbb: rrbb.cpp heuristic_filter.o homogenous_regions.o recursive_filter.o mla.o mlc.o tree_helper.o noise.o utility.o
	g++ -g -std=c++11 rrbb.cpp -o rrbb heuristic_filter.o homogenous_regions.o recursive_filter.o mla.o mlc.o tree_helper.o noise.o utility.o -I../RTree -I../ `pkg-config --cflags --libs opencv`

recursive_filter: recursive_filter.cpp
	g++ -g -std=c++11 recursive_filter.cpp -o recursive_filter -I../RTree -I../ `pkg-config --cflags --libs opencv` -lboost_system

multitest: multitest.cpp
	g++ -g -std=c++11 multitest.cpp -o multitest -lboost_system

tracktest: tracktest.cpp
	g++ -g -std=c++11 tracktest.cpp -o tracktest `pkg-config --cflags --libs opencv`
