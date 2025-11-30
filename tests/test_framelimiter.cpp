#include "utils/framelimiter.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

using lolol::utils::FrameLimiter;

class FrameLimiterTest : public ::testing::Test
{
  protected:
    // Helper to measure elapsed time
    auto measure_duration(auto fn) -> std::chrono::nanoseconds
    {
        auto start = std::chrono::steady_clock::now();
        fn();
        auto end = std::chrono::steady_clock::now();
        return end - start;
    }
};

TEST_F(FrameLimiterTest, LimitsFrameRate)
{
    constexpr int fps = 60;
    constexpr auto expected_duration = std::chrono::milliseconds(1000 / fps);
    constexpr auto tolerance = std::chrono::milliseconds(5);

    auto duration = measure_duration([&]() {
        FrameLimiter limiter(fps);
        // Simulate some work that takes less than one frame
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    });

    // Should be approximately 1/60th of a second (16.67ms)
    EXPECT_GE(duration, expected_duration - tolerance);
    EXPECT_LE(duration, expected_duration + tolerance);
}

TEST_F(FrameLimiterTest, DoesNotWaitIfWorkTakesLongerThanFrame)
{
    constexpr int fps = 60;
    constexpr auto frame_time = std::chrono::milliseconds(1000 / fps);
    constexpr auto work_time = frame_time + std::chrono::milliseconds(10);

    auto duration = measure_duration([&]() {
        FrameLimiter limiter(fps);
        // Simulate work that takes longer than one frame
        std::this_thread::sleep_for(work_time);
    });

    // Should not add extra sleep time
    EXPECT_LE(duration, work_time + std::chrono::milliseconds(2));
}

TEST_F(FrameLimiterTest, WorksWithDifferentFPS)
{
    constexpr int fps = 30;
    constexpr auto expected_duration = std::chrono::milliseconds(1000 / fps);
    constexpr auto tolerance = std::chrono::milliseconds(5);

    auto duration = measure_duration([&]() {
        FrameLimiter limiter(fps);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    });

    // Should be approximately 1/30th of a second (33.33ms)
    EXPECT_GE(duration, expected_duration - tolerance);
    EXPECT_LE(duration, expected_duration + tolerance);
}

TEST_F(FrameLimiterTest, MaintainsConsistentFrameRate)
{
    constexpr int fps = 100;
    constexpr int num_frames = 5;
    constexpr auto expected_frame_time = std::chrono::milliseconds(1000 / fps);

    auto total_duration = measure_duration([&]() {
        for (int i = 0; i < num_frames; ++i)
        {
            FrameLimiter limiter(fps);
            // Minimal work
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    auto expected_total = expected_frame_time * num_frames;
    auto tolerance = std::chrono::milliseconds(10);

    EXPECT_GE(total_duration, expected_total - tolerance);
    EXPECT_LE(total_duration, expected_total + tolerance);
}
