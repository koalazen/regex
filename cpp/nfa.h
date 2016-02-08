#include <map>
#include <set>
#include <utility>
#include <vector>
#include <string>

namespace nfa {

class State;

class StateSet {
 public:
  StateSet();
  StateSet(const StateSet &rhs);
  StateSet(StateSet &&rhs);
  StateSet(const std::vector<State *> &states);

  void operator=(const StateSet &rhs);
  void operator=(StateSet &&rhs);
  bool operator<(const StateSet &rhs) const;

  void add(State *state);
  void merge(const StateSet &rhs);
  void clear();
  int size() const;
  bool contains(State *state) const;

  // Iterator behavior.
  using iterator = std::set<State *>::iterator;
  using const_iterator = std::set<State *>::const_iterator;
  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

 private:
  std::set<State *> states_;
};

class State {
 public:
  void addTransition(char symbol, State *state);
  const StateSet &transitionStates(char symbol) const;

 private:
  State() {}
  friend class StateAllocator;
  mutable std::map<char, StateSet> transitions_;
};

class StateAllocator {
 public:
  StateAllocator();
  StateAllocator(StateAllocator &&rhs);
  ~StateAllocator();
  void operator=(StateAllocator &&rhs);

  State *newState();
  void merge(StateAllocator &&rhs);

 private:
  std::set<State *> allocated_states_;
};

class NFA {
 public:
  NFA();
  NFA(NFA &&rhs);
  void operator=(NFA &&rhs);

  // ---------------------------------------------------------------------------
  // NFA setup methods.

  State *newState();
  void setStartingStates(const std::vector<State *> &states);
  void setAcceptStates(const std::vector<State *> &states);

  // ---------------------------------------------------------------------------
  // NFA operations methods.

  void concat(NFA &&rhs);
  void question();
  void plus();
  void star();

  // ---------------------------------------------------------------------------
  // Factory methods.

  static NFA empty();
  static NFA fromCharacter(char c);

  // ---------------------------------------------------------------------------
  // NFA run methods.

  bool accept(const std::string &word) const;

 private:
  void expandEmpty(StateSet &states) const;
  void expandSymbol(StateSet &states, char symbol) const;
  void clearCache();

  static const char EMPTY = '\0';
  StateAllocator allocator_;
  StateSet start_states_;
  StateSet accept_states_;
  mutable std::map<std::pair<StateSet, char>, StateSet> transition_cache_;
};

}  // namespace nfa
