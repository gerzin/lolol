#include "utils/framelimiter.hpp"
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <spdlog/spdlog.h>

std::atomic_bool gShouldExit{false};
void signal_handler(int)
{
    gShouldExit.store(true);
}

auto main() -> int
{
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try
    {
        while (!gShouldExit.load())
        {
            lolol::utils::FrameLimiter limiter{30};
        }
    }
    catch (const std::exception &ex)
    {
        spdlog::error("Exception caught in main: {}", ex.what());
        return EXIT_FAILURE;
    }
}
