test_engine.o: test_engine.cpp
	g++ -std=c++11 -c test_engine.cpp

test: test.cpp test_engine.o
	g++ -std=c++11 test_engine.o test.cpp -o test -lboost_system -lboost_filesystem
