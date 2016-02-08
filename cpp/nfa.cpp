#include <algorithm>
#include <iterator>
#include <stdexcept>

#include "nfa.h"

using std::string;
using std::vector;

namespace nfa {

//------------------------------------------------------------------------------
// StateSet implementation.

StateSet::StateSet() {}

StateSet::StateSet(const StateSet &rhs) : states_(rhs.states_) {}

StateSet::StateSet(StateSet &&rhs) : states_(std::move(rhs.states_)) {}

StateSet::StateSet(const vector<State *> &states) {
  for (State *state : states) {
    states_.insert(state);
  }
}

void StateSet::operator=(const StateSet &rhs) { states_ = rhs.states_; }

void StateSet::operator=(StateSet &&rhs) { states_ = std::move(rhs.states_); }

void StateSet::add(State *state) { states_.insert(state); }

void StateSet::merge(const StateSet &rhs) {
  states_.insert(rhs.states_.begin(), rhs.states_.end());
}

bool StateSet::contains(State *state) const {
  return states_.find(state) != states_.end();
}

bool StateSet::operator<(const StateSet &rhs) const {
  return states_ < rhs.states_;
}

void StateSet::clear() { states_.clear(); }

int StateSet::size() const { return states_.size(); }

StateSet::iterator StateSet::begin() { return states_.begin(); }
StateSet::const_iterator StateSet::begin() const { return states_.begin(); }
StateSet::iterator StateSet::end() { return states_.end(); }
StateSet::const_iterator StateSet::end() const { return states_.end(); }

//------------------------------------------------------------------------------
// State

void State::addTransition(char symbol, State *state) {
  transitions_[symbol].add(state);
}

const StateSet &State::transitionStates(char symbol) const {
  return transitions_[symbol];
}

//------------------------------------------------------------------------------
// StateAllocator

StateAllocator::StateAllocator() {}
StateAllocator::StateAllocator(StateAllocator &&rhs)
    : allocated_states_(std::move(rhs.allocated_states_)) {}

void StateAllocator::operator=(StateAllocator &&rhs) {
  allocated_states_ = std::move(rhs.allocated_states_);
}

void StateAllocator::merge(StateAllocator &&rhs) {
  allocated_states_.insert(rhs.allocated_states_.begin(),
                           rhs.allocated_states_.end());
  rhs.allocated_states_.clear();
}

State *StateAllocator::newState() {
  auto *state = new State();
  allocated_states_.insert(state);
  return state;
}

StateAllocator::~StateAllocator() {
  for (auto *s : allocated_states_) {
    delete s;
  }
}

//------------------------------------------------------------------------------
// NFA

NFA::NFA() {}
NFA::NFA(NFA &&rhs)
    : allocator_(std::move(rhs.allocator_)),
      start_states_(std::move(rhs.start_states_)),
      accept_states_(std::move(rhs.accept_states_)),
      transition_cache_(std::move(rhs.transition_cache_)) {}

void NFA::operator=(NFA &&rhs) {
  allocator_ = std::move(rhs.allocator_);
  start_states_ = std::move(rhs.start_states_);
  accept_states_ = std::move(rhs.accept_states_);
  transition_cache_ = std::move(rhs.transition_cache_);
}

State *NFA::newState() { return allocator_.newState(); }

void NFA::setStartingStates(const vector<State *> &states) {
  start_states_ = StateSet(states);
}

void NFA::setAcceptStates(const vector<State *> &states) {
  accept_states_ = StateSet(states);
}

void NFA::concat(NFA &&rhs) {
  clearCache();
  allocator_.merge(std::move(rhs.allocator_));
  for (State *left : accept_states_) {
    for (State *right : rhs.start_states_) {
      left->addTransition('\0', right);
    }
  }
  accept_states_ = rhs.accept_states_;
}

void NFA::question() {
  clearCache();
  State *start = newState();
  for (auto *right : start_states_) {
    start->addTransition('\0', right);
  }
  start_states_ = StateSet({start});
  accept_states_.add(start);
}

void NFA::plus() {
  clearCache();
  for (State *left : accept_states_) {
    for (State *right : start_states_) {
      left->addTransition('\0', right);
    }
  }
}

void NFA::star() {
  clearCache();
  question();
  plus();
}

NFA NFA::empty() {
  NFA nfa;
  auto s = nfa.newState();
  nfa.setStartingStates({s});
  nfa.setAcceptStates({s});
  return nfa;
}

NFA NFA::fromCharacter(char c) {
  NFA nfa;
  auto *s1 = nfa.newState();
  auto *s2 = nfa.newState();
  s1->addTransition(c, s2);
  nfa.setStartingStates({s1});
  nfa.setAcceptStates({s2});
  return nfa;
}

bool NFA::accept(const string &word) const {
  StateSet states = start_states_;
  for (auto c : word) {
    auto transition_context = std::make_pair(states, c);
    if (transition_cache_.find(transition_context) != transition_cache_.end()) {
      states = transition_cache_.at(transition_context);
      continue;
    }
    expandEmpty(states);
    expandSymbol(states, c);
    expandEmpty(states);
    transition_cache_[transition_context] = states;
  }
  for (State *s : states) {
    if (accept_states_.contains(s)) return true;
  }
  return false;
}

void NFA::expandEmpty(StateSet &states) const {
  bool finished = false;
  while (!finished) {
    int orig_size = states.size();
    for (State *s : states) {
      states.merge(s->transitionStates(EMPTY));
    }
    if (states.size() == orig_size) {
      finished = true;
    }
  }
}

void NFA::expandSymbol(StateSet &states, char symbol) const {
  StateSet new_states;
  for (State *s : states) {
    new_states.merge(s->transitionStates(symbol));
  }
  states = new_states;
}

void NFA::clearCache() { transition_cache_.clear(); }

const char NFA::EMPTY;

}  // namespace nfa
