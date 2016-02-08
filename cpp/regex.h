#include <string>
#include <vector>
#include <memory>
#include "nfa.h"

namespace regex {

class Regex {
 public:
  Regex(const std::string &regex);
  bool match(const std::string &pattern) const;

 private:
  static bool isOperator(char c);
  static int findMatchingPar(const std::string &word, int pos);
  static nfa::NFA parse(const std::string &word);

  nfa::NFA nfa_;
  static const std::vector<char> operators;
};

}  // namespace regex
