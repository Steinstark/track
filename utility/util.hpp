#ifndef TRACK_UTIL_HPP
#define TRACK_UTIL_HPP

#include <string>
#include <vector>
#include <functional>
#include <type_traits>
#include <cmath>

bool fileHasType(std::string file, std::string  extension);

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
typename std::enable_if<std::is_arithmetic<T>::value, double>::type welford(std::vector<V>& v, std::function<T(V)> f){
  if (v.size() < 2)
    return 0;
  double mean = 0, m2 = 0;
  for (int i = 0; i < v.size(); i++){
    T  val = f(v[i]);
    double delta = val - mean;
    mean += delta / (i+1);
    double delta2 = val - mean;
    m2 += delta*delta2;
  }
  return sqrt(m2/(v.size()-1));
}

#endif
