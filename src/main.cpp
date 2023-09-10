#include <iostream>
#include <chrono>
#include "MarketDataManager.h"
#include "FillManager.h"
#include "PortfolioManager.h"
#include "Backtester.h"
#include "XYAlgorithm.h"
#include "SecurityUniverse.h"
#include "ArgParser.h"
#include <climits>

int main(int argc, char* argv[]) {

    auto start_time = std::chrono::high_resolution_clock::now();

    /* Input parse */
    double X, Y;
    std::string input_dir, output_dir;
    parse(argc, argv, X, Y, input_dir, output_dir);

    MarketDataManager mdm(input_dir + "/bar_data.csv");
    FillManager fm;
    PortfolioManager pm;
    XYAlgorithm strategy(X, Y);
    SecurityUniverse::getInstance().setOutputDir(output_dir);

    Backtester backtest(mdm, fm, pm,
                         input_dir + "/target_positions.csv",
                         &strategy);

    backtest.execute();

    auto end_time = std::chrono::high_resolution_clock::now() - start_time;

    long double seconds = std::chrono::duration<double>(end_time).count();
    std::cout << std::fixed << std::setprecision(6) << seconds << " seconds" << std::endl;

    return 0;
}