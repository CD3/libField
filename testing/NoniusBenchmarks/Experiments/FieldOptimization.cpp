#include "Field.hpp"
#include "nonius/nonius.h++"

NONIUS_BENCHMARK("EXPERIMENT | Field element assignment | Baseline | getData()",
                 [](nonius::chronometer meter) {
                   Field<double, 3> Temperature(100, 100, 100);

                   meter.measure([&]() {
                     for (int i = 0; i < 100; ++i)
                       for (int j = 0; j < 100; ++j)
                         for (int k = 0; k < 100; ++k)
                           Temperature.getData()[i][j][k] = 10;
                   });
                 })

NONIUS_BENCHMARK(
    "EXPERIMENT | Field element assignment | Trial 1 | operator(Collection)",
    [](nonius::chronometer meter) {
      Field<double, 3> Temperature(100, 100, 100);
      std::vector<int> I(3);

      meter.measure([&]() {
        for (int i = 0; i < 100; ++i)
          for (int j = 0; j < 100; ++j)
            for (int k = 0; k < 100; ++k) {
              I              = {i, j, k};
              Temperature(I) = 10;
            }
      });
    })

NONIUS_BENCHMARK(
    "EXPERIMENT | Field element assignment | Trial 2 | operator(int,int,int)",
    [](nonius::chronometer meter) {
      Field<double, 3> Temperature(100, 100, 100);
      std::vector<int> I(3);

      meter.measure([&]() {
        for (int i = 0; i < 100; ++i)
          for (int j = 0; j < 100; ++j)
            for (int k = 0; k < 100; ++k) Temperature(i, j, k) = 10;
      });
    })

NONIUS_BENCHMARK("EXPERIMENT | Field element assignment | Trial 3 | operator[]",
                 [](nonius::chronometer meter) {
                   Field<double, 3> Temperature(100, 100, 100);

                   meter.measure([&]() {
                     for (int i = 0; i < 100; ++i)
                       for (int j = 0; j < 100; ++j)
                         for (int k = 0; k < 100; ++k)
                           Temperature[i][j][k] = 10;
                   });
                 })
