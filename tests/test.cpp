#include <iostream>
#include <vector>
#include <string>
#include "test_engine.hpp"

using namespace std;

int main(){
  vector<string> files = files_in_folder(".");
  for (string file : files){
    cout << file << endl;
  }
}
