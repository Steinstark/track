track: track.cpp
	g++ -std=c++11 `Magick++-config --cxxflags --cppflags` -o track track.cpp `Magick++-config --ldflags --libs` -lleptonica -ltesseract
