#pragma once

#include <chrono>

namespace lolol::utils
{

/**
 * @class FrameLimiter
 * @brief A utility class to limit the frame rate of a process.
 *
 * This class uses RAII to measure the time taken for a frame to process
 * and sleeps for the remaining time to maintain a consistent frame rate.
 */
class FrameLimiter
{
  public:
    explicit FrameLimiter(int fps);
    ~FrameLimiter();

    FrameLimiter(const FrameLimiter &) = delete;
    FrameLimiter &operator=(const FrameLimiter &) = delete;
    FrameLimiter(FrameLimiter &&) = delete;
    FrameLimiter &operator=(FrameLimiter &&) = delete;
    /**
     * @brief Check if the frame limiter is disabled.
     * @return true if disabled, false otherwise.
     */
    [[nodiscard]] bool is_disabled() const;
    /**
     * @brief Disable the frame limiter, preventing it from sleeping.
     */
    void disable();
    /**
     * @brief Enable the frame limiter, allowing it to sleep to maintain frame rate.
     */
    void enable();

  private:
    std::chrono::nanoseconds mFrameDuration;
    std::chrono::steady_clock::time_point mFrameStart;
    bool mDisabled{false};
};
} // namespace lolol::utils