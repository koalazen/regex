from heapq import heappush, heappop
import benchmark
import re


class State(object):

    def __init__(self):
        self._transition_table = {}

    def add_transition(self, symbol, state):
        if symbol not in self._transition_table:
            self._transition_table[symbol] = frozenset()
        self._transition_table[symbol] = self._transition_table[
            symbol].union(frozenset([state]))

    def transition(self, symbol):
        if symbol in self._transition_table:
            return self._transition_table[symbol]
        return frozenset()

    def __str__(self):
        return str(self._transition_table)


class NFA(object):

    def __init__(self, start_states, accept_states):
        self._start_states = frozenset(start_states)
        self._accept_states = frozenset(accept_states)
        self._cache = {}

    def _clear_cache(self):
        self._cache = {}

    @classmethod
    def empty(cls):
        state = State()
        return cls([state], [state])

    @classmethod
    def from_character(cls, character):
        start = State()
        accept = State()
        start.add_transition(character, accept)
        return cls([start], [accept])

    def _link_empty(self, left_states, right_states):
        for left_state in left_states:
            for right_state in right_states:
                left_state.add_transition('', right_state)

    def concat(self, rhs):
        self._clear_cache()
        self._link_empty(self._accept_states, rhs._start_states)
        self._accept_states = rhs._accept_states

    def star(self):
        self._clear_cache()
        self._accept_states = self._accept_states.union(self._start_states)
        self._link_empty(self._accept_states, self._start_states)

    def plus(self):
        self._clear_cache()
        self._link_empty(self._accept_states, self._start_states)

    def question(self):
        self._clear_cache()
        start = State()
        for state in self._start_states:
            start.add_transition('', state)
        self._start_states = frozenset([start])
        self._accept_states = self._accept_states.union(self._start_states)

    def _symbol_transition(self, states, symbol):
        new_states = frozenset()
        for state in states:
            transition_states = state.transition(symbol)
            if not transition_states.issubset(new_states):
                new_states = new_states.union(transition_states)
        return new_states

    def _expand_empty(self, states):
        completed = False
        while not completed:
            completed = True
            for state in states:
                transition_states = state.transition('')
                if not transition_states.issubset(states):
                    completed = False
                    states = states.union(transition_states)
        return states

    def _transition(self, states, symbol):
        states = self._expand_empty(states)
        states = self._symbol_transition(states, symbol)
        return self._expand_empty(states)

    def _cached_transition(self, states, symbol):
        transition_context = (states, symbol)
        if transition_context in self._cache:
            return self._cache[transition_context]
        transition_states = self._transition(states, symbol)
        self._cache[transition_context] = transition_states
        return transition_states

    def accept(self, msg):
        states = self._start_states
        for symbol in msg:
            states = self._cached_transition(states, symbol)
        return len(states.intersection(self._accept_states)) > 0

    def __str__(self):
        res = "start:\n"
        for state in self._start_states:
            res += str(state) + "\n"
        res += "accept:\n"
        for state in self._accept_states:
            res += str(state) + "\n"
        return res


class Regex(object):

    _op_symbols = ['+', '*', '?']
    _op_precedence = {'concat': 2, '+': 1, '*': 1, '?': 1}

    def __init__(self, pattern):
        self._nfa = self._parse(pattern)

    def match(self, pattern):
        return self._nfa.accept(pattern)

    @classmethod
    def _is_op_symbol(cls, symbol):
        return symbol in cls._op_symbols

    @classmethod
    def _find_matching_par(cls, regex, i):
        c = 1
        for j, symbol in enumerate(regex[i + 1:]):
            if symbol == '(':
                c += 1
            elif symbol == ')':
                c -= 1
            if c == 0:
                return j + i + 1
        return None

    @classmethod
    def _parse(cls, regex):
        a = NFA.empty()
        b = NFA.empty()
        i = 0
        while i < len(regex):
            symbol = regex[i]
            if symbol == '(':
                j = cls._find_matching_par(regex, i)
                if j is None:
                    raise ValueError(
                        'Invalid expression. Missing matching parenthesis.')
                a.concat(b)
                b = cls._parse(regex[i + 1:j])
                i = j
            elif cls._is_op_symbol(symbol):
                if i == 0:
                    raise ValueError(
                        'Invalid expression. Operator without matching operand.')
                if symbol == '+':
                    b.plus()
                elif symbol == '*':
                    b.star()
                elif symbol == '?':
                    b.question()
            else:
                a.concat(b)
                b = NFA.from_character(symbol)
            i += 1
        a.concat(b)
        return a


def main():
    pattern = '(fo)*(bo)*'
    test = 'fobobo'

    r = Regex(pattern)
    r2 = re.compile(pattern)

    print r.match(test)
    m = r2.match(test)
    print m is not None and m.group(0) == test

    benchmark.benchmark_micros(lambda: r.match(test))
    benchmark.benchmark_micros(lambda: r2.match(test))

if __name__ == "__main__":
    main()
