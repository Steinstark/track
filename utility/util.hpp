#ifndef TRACK_UTIL_HPP
#define TRACK_UTIL_HPP

#include <string>
#include <vector>
#include <functional>
#include <type_traits>
#include <cmath>

bool fileHasType(std::string file, std::string  extension);

class OnlineStat{
public:
  OnlineStat(): size(0), mean(0), m2(0) {}
  
  template <typename T>
  double welfordStep(T val){
    double delta = val - mean;
    mean += delta/(++size);
    double delta2 = val - mean;
    m2 += delta*delta2;
    if (size < 2)
      return 0;
    return sqrt(m2/(size-1));
  }
  void reset(){
    size = 0;
    mean = 0;
    m2 = 0;
    
  }
private:
  size_t size;
  double mean, m2;
};

template<typename V, typename T>
typename std::enable_if<std::is_arithmetic<T>::value, double>::type mean(std::vector<V>& v, std::function<T(V)> f){
  T val = 0;
  for (int i = 0; i < v.size(); i++){
    val += f(v[i]);
  }
  return (double)val/v.size();
}

template <typename V, typename T>
typename std::enable_if<std::is_arithmetic<T>::value, double>::type variance(std::vector<V>& v, std::function<T(V)> f){
  double m2 = pow(mean<V,T>(v, f),2);
  double m1 = mean<V, T>(v, [&f](V e){return pow(f(e),2);});
  return m1 - m2;
}

template <typename V, typename T>
typename std::enable_if<std::is_arithmetic<T>::value, double>::type welford(const std::vector<V>& v, std::function<T(V)> f){
  double mean = 0, m2 = 0;
  if (v.size() < 2)
    return 0;
  for (int i = 0; i < v.size(); i++){
    T  val = f(v[i]);
    double delta = val - mean;
    mean += delta / (i+1);
    double delta2 = val - mean;
    m2 += delta*delta2;
  }
  return sqrt(m2/(v.size()-1));
}

template <typename Number>
double binapprox(const std::vector<Number>& x) {
  // Compute the mean and standard deviation
  int n = x.size();
  double sum = 0;
  int i;
  for (i = 0; i < n; i++) {
    sum += x[i];
  }
  double mu = sum/n;

  sum = 0;
  for (i = 0; i < n; i++) {
    sum += (x[i]-mu)*(x[i]-mu);
  }
  double sigma = sqrt(sum/n);

  // Bin x across the interval [mu-sigma, mu+sigma]
  int bottomcount = 0;
  int bincounts[1001];
  for (i = 0; i < 1001; i++) {
    bincounts[i] = 0;
  }
  double scalefactor = 1000/(2*sigma);
  double leftend =  mu-sigma;
  double rightend = mu+sigma;
  int bin;

  for (i = 0; i < n; i++) {
    if (x[i] < leftend) {
      bottomcount++;
    }
    else if (x[i] < rightend) {
      bin = (int)((x[i]-leftend) * scalefactor);
      bincounts[bin]++;
    }
  }

  // If n is odd
  if (n & 1) {
    // Find the bin that contains the median
    int k = (n+1)/2;
    int count = bottomcount;

    for (i = 0; i < 1001; i++) {
      count += bincounts[i];

      if (count >= k) {
        return (i+0.5)/scalefactor + leftend;
      }
    }
  }
  
  // If n is even
  else {
    // Find the bins that contains the medians
    int k = n/2;
    int count = bottomcount;
    
    for (i = 0; i < 1001; i++) {
      count += bincounts[i];
      
      if (count >= k) {
        int j = i;
        while (count == k) {
          j++;
          count += bincounts[j];
        }
        return (i+j+1)/(2*scalefactor) + leftend;
      }
    }
  } 
} 

#endif
