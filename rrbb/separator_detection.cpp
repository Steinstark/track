//TODO
//implement separator detection

using namespace std;
using namespace cv;

vector<int> detect_separator(const Mat& stats){
  vector<int> separators;
  for (int i = 0; i < stats.rows; i++){
    if (density[i] <= 0.02 && containsText(bb[i]))
      separators.push_back(i);
  }
  return separators
}
