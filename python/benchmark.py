import regex
import re
import time


def benchmark(f, num_times):
    start = time.time()
    for i in xrange(num_times):
        f()
    end = time.time()
    return (end - start) / float(num_times)


def benchmark_micros(f, precision=3):
    elapsed = benchmark(f, 100000)
    print "{{:.{precision}}} microseconds".format(precision=precision).format(elapsed * 1e6)


def main():
    pattern = '(foo(ba)?)*(bar)+'
    test = 'foofoobabarbar'

    r = regex.Regex(pattern)
    r2 = re.compile(pattern)

    print r.match(test)
    m = r2.match(test)
    print m is not None and m.group(0) == test

    benchmark_micros(lambda: r.match(test))
    benchmark_micros(lambda: r2.match(test))

if __name__ == "__main__":
    main()
