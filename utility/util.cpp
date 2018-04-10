#include "util.hpp"
#include <cmath>
#include <functional>
#include <type_traits>
#include <vector>

using namespace std;

bool fileHasType(string file, string extension){
  int pos = file.find_last_of(".");
  return file.substr(pos+1) == extension;
}

double gaussWeight(double x, double mean, double var){
  return exp(-pow(x-mean,2)/(2*var))/(sqrt(2*M_PI*var));
}
