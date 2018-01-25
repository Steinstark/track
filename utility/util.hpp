#ifndef TRACK_UTIL_HPP
#define TRACK_UTIL_HPP

#include <string>
#include <vector>
#include <functional>
#include <type_traits>

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
typename std::enable_if<std::is_arithmetic<T>::value, double>::type variance(std::vector<V> v, std::function<V(T)> f){
  double m2 = pow(mean(v, f),2);
  double m1 = mean(v, [&f](T e){ return pow(f(e),2);});
  return m1 - m2;
}
  
#endif
