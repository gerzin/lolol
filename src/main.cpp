#include "devicemanager.hpp"
#include "utils/framelimiter.hpp"
#include <argparse/argparse.hpp>
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <spdlog/spdlog.h>

std::atomic_bool gShouldExit{false};
void signal_handler(int)
{
    gShouldExit.store(true);
}

auto main(int argc, char **argv) -> int
{
    using namespace lolol;
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    argparse::ArgumentParser program("lolol");
    program.add_description("Create a loopback video device and display your own content");

    try
    {
        DeviceManager manager{"LOLOL Virtual Camera"};

        auto &videoDevice = manager.get_video_device();

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
