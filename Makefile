track: track.cpp
	g++ -std=c++11 `Magick++-config --cxxflags --cppflags` -o track track.cpp `Magick++-config --ldflags --libs` -lleptonica -ltesseract

detect_table: detect_table.cpp
	g++ -std=c++11 detect_table.cpp -o detect_table `pkg-config --cflags --libs opencv`
