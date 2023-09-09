#include <iostream>
#include <chrono>
#include "MarketDataManager.h"
#include "FillManager.h"
#include "PortfolioManager.h"
#include "Backtester.h"
#include "XYAlgorithm.h"
#include "SecurityUniverse.h"
#include <climits>

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    MarketDataManager mdm("/Users/apple/Macbook Non-iCloud/Playground/Cubist/cpp_project/bar_data.csv");
    FillManager fm;
    PortfolioManager pm;
    XYAlgorithm strategy(0.5, 0.5);
    SecurityUniverse::getInstance().setOutputDir("/Users/apple/Macbook Non-iCloud/Playground/Cubist/output_dir/");

    Backtester backtest(mdm, fm, pm,
                         "/Users/apple/Macbook Non-iCloud/Playground/Cubist/cpp_project/target_positions.csv",
                         &strategy);

    backtest.execute();

    auto end_time = std::chrono::high_resolution_clock::now() - start_time;

    long double seconds = std::chrono::duration<double>(end_time).count();
    std::cout << std::fixed << std::setprecision(6) << seconds << " seconds" << std::endl;

    return 0;
}