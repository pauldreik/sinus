#include "benchmark/benchmark.h"

#include "GenerateData.hpp"
#include "VectorLibrary.hpp"
#include "StandardLibrary.hpp"
#include "NoOpLib.hpp"

template<typename Real, typename Impl>
static void
generic(benchmark::State& state, DataKind kind, Real, Impl)
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

template<typename Real>
static void
vectorlib(benchmark::State& state, DataKind kind, Real dummyval) {
    generic(state,kind,dummyval,VectorLibrary());
}
template<typename Real>
static void
stdlib(benchmark::State& state, DataKind kind, Real dummyval) {
    generic(state,kind,dummyval,StandardLibrary());
}
template<typename Real>
static void
nooplib(benchmark::State& state, DataKind kind, Real dummyval) {
    generic(state,kind,dummyval,NoOpLib());
}

BENCHMARK_CAPTURE(nooplib, overhead_measure_tiny, DataKind::TINY, 0.f);
BENCHMARK_CAPTURE(vectorlib, tiny, DataKind::TINY, 0.f);
BENCHMARK_CAPTURE(vectorlib, medium, DataKind::MEDIUM, 0.f);
BENCHMARK_CAPTURE(vectorlib,
                  increasing,
                  DataKind::INCREASING,
                  0.f);
BENCHMARK_CAPTURE(vectorlib, garbage, DataKind::GARBAGE, 0.f);


BENCHMARK_CAPTURE(stdlib, tiny, DataKind::TINY, 0.f);
BENCHMARK_CAPTURE(stdlib, medium, DataKind::MEDIUM, 0.f);
BENCHMARK_CAPTURE(stdlib,
                  increasing,
                  DataKind::INCREASING,
                  0.f);
BENCHMARK_CAPTURE(stdlib, garbage, DataKind::GARBAGE, 0.f);

BENCHMARK_MAIN();
