cmake_minimum_required(VERSION 3.26)
project(Cubist)

set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_STANDARD 17)

add_executable(Cubist main.cpp
        MinuteBarRecord.h
        backtest_util.h
        MinuteBarRecord.cpp.txt
        Security.h
        CSVReader.h
        TargetPositionRecord.h
        SecurityUniverse.h
        WeekIntervalData.h
        Fill.h
        PortfolioSnapshot.h
        MarketDataManager.h
        FillManager.h
        ExecutionAlgorithm.h
        XYAlgorithm.h
        XYAlgorithm.cpp
        SecurityUniverse.cpp
        SecurityUniverse.cpp
        SecurityUniverse.cpp
        MarketDataManager.cpp
        Security.cpp
        PortfolioManager.h
        Backtester.h
        Backtester.cpp
        FillManager.cpp
        PortfolioManager.cpp
        DataModel.h
        MinuteBarRecord.cpp
        Fill.cpp
        TargetPositionRecord.cpp
        PortfolioSnapshot.cpp
        PnLRecord.h
        PnLRecord.cpp
)
target_link_libraries(Cubist)
