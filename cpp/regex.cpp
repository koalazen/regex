#include "regex.h"

using nfa::NFA;
using std::string;
using std::vector;

namespace regex {

Regex::Regex(const string &regex) { nfa_ = parse(regex); }

bool Regex::match(const string &pattern) const { return nfa_.accept(pattern); }

bool Regex::isOperator(char c) {
  return std::find(operators.begin(), operators.end(), c) != operators.end();
}

int Regex::findMatchingPar(const string &word, int pos) {
  int count = 1;
  pos++;
  while (pos < word.size()) {
    if (word[pos] == '(') count++;
    if (word[pos] == ')') count--;
    if (count == 0) {
      return pos;
    }
    pos++;
  }
  return -1;
}

NFA Regex::parse(const string &word) {
  NFA a = NFA::empty();
  NFA b = NFA::empty();
  int i = 0;
  while (i < word.size()) {
    const char symbol = word[i];
    if (symbol == '(') {
      int idx = findMatchingPar(word, i);
      if (idx == -1) {
        throw std::runtime_error(
            "Invalid expression: Missing matching parenthesis.");
      }
      a.concat(std::move(b));
      b = parse(word.substr(i + 1, idx - i - 1));
      i = idx;
    } else if (isOperator(symbol)) {
      if (i == 0) {
        throw std::runtime_error(
            "Invalid expression: Expression starts with an operator.");
      }
      switch (symbol) {
        case '+':
          b.plus();
          break;
        case '*':
          b.star();
          break;
        case '?':
          b.question();
          break;
      }
    } else {
      a.concat(std::move(b));
      b = NFA::fromCharacter(symbol);
    }
    i++;
  }
  a.concat(std::move(b));
  return a;
}

const vector<char> Regex::operators = {'+', '*', '?'};

}  // namespace regex
