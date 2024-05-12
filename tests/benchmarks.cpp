#include <benchmark/benchmark.h>

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "earthorbits/earthorbits.h"
#include "earthorbits/parsetle.h"

using namespace eob;

struct KeyValue {
  std::string key;
  int value;
};

struct MyMap {
  std::vector<KeyValue> map;
  bool contains(const std::string& key) {
    return std::find_if(map.begin(), map.end(), [&key](const auto& kv) {
             return kv.key == key;
           }) != map.end();
  }
  int get(const std::string& key) {
    return std::find_if(map.begin(), map.end(),
                        [&key](const auto& kv) { return kv.key == key; })
        ->value;
  }
};

static void BM_MyMap(benchmark::State& state) {
  MyMap my_map{.map = {
                   {"hubba", 1},
                   {"bubba", 2},
                   {"foo", 3},
                   {"bar", 4},
                   {"wack", 5},
                   {"b", 6},
                   {"c", 8},
                   {"asdfasdf", 9},
                   {"asdfaaadf", 10},
                   {"gfagasd", 11},
               }};

  for (auto _ : state) {  // NOLINT(deadcode.DeadStores)
    my_map.get("gfagasd");
  }
}
// Register the function as a benchmark
// BENCHMARK(BM_MyMap);

static void BM_StdUnorderedMap(benchmark::State& state) {
  // Code before the loop is not measured
  std::unordered_map<std::string, int> std_map{
      {"hubba", 1},      {"bubba", 2},    {"foo", 3}, {"bar", 4},
      {"wack", 5},       {"b", 6},        {"c", 8},   {"asdfasdf", 9},
      {"asdfaaadf", 10}, {"gfagasd", 11},
  };

  for (auto _ : state) {
    std_map["gfagasd"];
  }
}
// BENCHMARK(BM_StdUnorderedMap);

static void BM_StdMap(benchmark::State& state) {
  std::map<std::string, int> std_map{
      {"hubba", 1},      {"bubba", 2},    {"foo", 3}, {"bar", 4},
      {"wack", 5},       {"b", 6},        {"c", 8},   {"asdfasdf", 9},
      {"asdfaaadf", 10}, {"gfagasd", 11},
  };

  for (auto _ : state) {
    std_map["gfagasd"];
  }
}
// BENCHMARK(BM_StdMap);

static void BM_ParseTles(benchmark::State& state) {
  std::string s =
      R"(1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995
2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.49960977447473)";

  for (auto _ : state) {
    auto tle = ParseTle(s);
  }
}
BENCHMARK(BM_ParseTles);

static void BM_CalcGMST(benchmark::State& state) {
  auto now = std::chrono::system_clock::now();
  for (auto _ : state) {
    auto gmst = calc_gmst(now);
  }
}
BENCHMARK(BM_CalcGMST);

BENCHMARK_MAIN();
