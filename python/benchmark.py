import regex
import re
import time


def benchmark(f, num_times):
    start = time.time()
    for i in xrange(num_times):
        f()
    end = time.time()
    return (end - start) / float(num_times)


def benchmark_micros(f):
    elapsed = benchmark(f, 10000)
    return elapsed * 1e6


def main():
    pattern = '(foo(ba)?)*(bar)+'
    test = 'foofoobabarbar'

    r = regex.Regex(pattern)
    r2 = re.compile(pattern)

    print "Custom implementation matches:", "yes" if r.match(test) else "no"
    m = r2.match(test)
    match = m is not None and m.group(0) == test
    print "Re module implementation matches:", "yes" if match else "no"

    print "Custom implementation runs in {{:.{precision}}} microseconds".format(
        precision=3).format(
        benchmark_micros(lambda: r.match(test)))
    print "Re module implementation runs in {{:.{precision}}} microseconds".format(
        precision=3).format(
        benchmark_micros(lambda: r2.match(test)))

if __name__ == "__main__":
    main()
