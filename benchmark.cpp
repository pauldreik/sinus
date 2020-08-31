#include "benchmark/benchmark.h"

#include "GenerateData.hpp"
#include "VectorLibrary.hpp"

template<typename Real, typename Impl>
static void
vectorlib(benchmark::State& state, DataKind kind, Real, Impl)
{
  int seed = 1;
  const auto data = makeData<Real>(3000, kind, seed);
  auto datacopy = data;
  for (auto _ : state) {
    datacopy.assign(data.begin(), data.end());
    Impl::inplace_sin(datacopy.data(), data.size());
    benchmark::DoNotOptimize(data.data());
  }
  state.counters["elements per second"] =
    (int64_t(state.iterations()) * int64_t(data.size()));
}
BENCHMARK_CAPTURE(vectorlib, tiny, DataKind::TINY, 0.f, VectorLibrary());
BENCHMARK_CAPTURE(vectorlib, medium, DataKind::MEDIUM, 0.f, VectorLibrary());
BENCHMARK_CAPTURE(vectorlib,
                  increasing,
                  DataKind::INCREASING,
                  0.f,
                  VectorLibrary());
BENCHMARK_CAPTURE(vectorlib, garbage, DataKind::GARBAGE, 0.f, VectorLibrary());

BENCHMARK_MAIN();
