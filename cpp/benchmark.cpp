#include <ctime>
#include <iostream>
#include <regex>
#include <string>

#include "regex.h"

using regex::Regex;
using std::cout;
using std::endl;
using std::string;

template <typename F>
double benchmark_micros(const F& f, int num_times = 10000) {
  auto start = std::clock();
  for (int i = 0; i < num_times; i++) {
    f();
  }
  auto end = std::clock();
  return ((end - start) / num_times) / (CLOCKS_PER_SEC / 1e6);
}

int main() {
  Regex r("(foo(ba)?)*(bar)+");
  std::regex re("(foo(ba)?)*(bar)+");
  const string test = "foofoobabarbar";
  cout << "Custom implementation matches: " << (r.match(test) ? "yes" : "no")
       << endl;
  cout << "Stdlib implementation matches: "
       << (std::regex_match(test, re) ? "yes" : "no") << endl;
  cout << "Custom implementation: "
       << benchmark_micros([&]() { r.match("foofoobabarbar"); })
       << " microseconds." << endl;
  cout << "Stdlib implementation: "
       << benchmark_micros([&]() { std::regex_match(test, re); })
       << " microseconds." << endl;
  return 0;
}
