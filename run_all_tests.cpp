#include <gtest/gtest.h>

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

int main(int argc, char **argv)
{
    auto worker = g3::LogWorker::createLogWorker();
    auto handle = worker->addDefaultLogger("YukiToolsetTests", "./");
    g3::initializeLogging(worker.get());

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
