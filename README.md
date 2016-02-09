## Basic regex implementation in various languages.

The implementation has a very simple syntax. We allow the __*__, __+__ and __?__ operators as well as grouping with parenthesis.
The implementation is based on finite automata. Nondeterministic finite automata are used. We transform them at runtime into finite automata with caching.
The implementation is based on: https://swtch.com/~rsc/regexp/regexp1.html

### Benchmarks:

* regex: "(foo(ba)?)*(bar)+"
* test: "foofoobabarbar"

#### C++:
* Compilation flags: -O3
* this implementation: 13 &#181; (microseconds)
* stdlib implementation: 2 &#181;

#### Python: 
* this implementation: 6.53 &#181;
* Re module implementation: 0.77 &#181;

Note: I am surprised that this is faster than the C++ implementation.
