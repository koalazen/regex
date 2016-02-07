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
