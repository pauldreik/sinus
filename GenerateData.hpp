#pragma once

#include <cassert>
#include <cstring>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>

enum class DataKind
{
  /// uniformly distributed values, absolute value is 1e-30 or smaller
  TINY,
  /// uniformly distributed values, within +-100
  MEDIUM,
  /// linearly increasing 0 - 100*pi, realistic for my application
  INCREASING,
  /// binary garbage, "line noise"
  GARBAGE
};

// makes Nelements of type T with data
template<typename T>
std::vector<T>
makeData(std::size_t Nelements, DataKind kind, int seed)
{
  static_assert(std::is_arithmetic_v<T>, "use float or double for T");

  // worst seeding code ever :-(
  std::seed_seq seeder{ seed };
  std::mt19937_64 engine(seeder);
  std::vector<T> ret;
  ret.reserve(Nelements);
  switch (kind) {
    case DataKind::TINY: {
      std::uniform_real_distribution<T> uniform_dist(-1e-30, 1e-30);
      for (size_t i = 0; i < Nelements; ++i) {
        ret.emplace_back(uniform_dist(engine));
      }
    } break;

    case DataKind::MEDIUM: {
      std::uniform_real_distribution<T> uniform_dist(-100, 100);
      for (size_t i = 0; i < Nelements; ++i) {
        ret.emplace_back(uniform_dist(engine));
      }
    } break;
    case DataKind::INCREASING: {
      const auto endvalue = 100 * M_PI;
      const double step = endvalue / Nelements;
      double x = 0;
      for (size_t i = 0; i < Nelements; ++i) {
        ret.emplace_back(x);
        x += step;
      }
    } break;
    case DataKind::GARBAGE: {
      for (size_t i = 0; i < Nelements;) {
        using Output = decltype(engine());
        constexpr auto Nbytes = std::lcm(sizeof(T), sizeof(Output));
        constexpr auto Noutput = Nbytes / sizeof(Output);
        constexpr auto N_of_T = Nbytes / sizeof(T);
        Output tmpbin[Noutput];
        for (auto& e : tmpbin) {
          e = engine();
        }

        T val[N_of_T];
        std::memcpy(&val, &tmpbin, Nbytes);
        for (size_t j = 0; j < N_of_T && i < Nelements; ++j, ++i) {
          ret.emplace_back(val[j] * 1e-38f);
        }
      }
    } break;
    default:
      throw std::runtime_error("unimplemented kind");
  }
  assert(ret.size() == Nelements);

  return ret;
}
