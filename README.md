# Linux IPC benchmarks

This is a collection of benchmarks of Linux interprocess communication mechanisms.

At present, it benchmarks round-trip latency for pipes and eventfd.

To build the benchmarks, simply run `make`. The `pipes` and `eventfd` binaries
both take the following flags:

```
  -i, -n, --iters=COUNT      number of iterations to measure; default: 100000
  -w, --warmup-iters=COUNT   number of iterations before measurement; default:
                             1000
  -?, --help                 Give this help list
      --usage                Give a short usage message
```
