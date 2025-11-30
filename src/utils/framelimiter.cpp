#include "framelimiter.hpp"

#include <thread>

namespace lolol::utils
{

FrameLimiter::FrameLimiter(int fps)
    : mFrameDuration(std::chrono::nanoseconds(1'000'000'000 / fps)), mFrameStart(std::chrono::steady_clock::now())
{
}

FrameLimiter::~FrameLimiter()
{
    auto frame_end = std::chrono::steady_clock::now();
    auto elapsed = frame_end - mFrameStart;
    auto sleep_time = mFrameDuration - elapsed;

    if (!mDisabled && sleep_time > std::chrono::nanoseconds::zero())
    {
        std::this_thread::sleep_for(sleep_time);
    }
}

bool FrameLimiter::is_disabled() const
{
    return mDisabled;
}

void FrameLimiter::disable()
{
    mDisabled = true;
}

void FrameLimiter::enable()
{
    mDisabled = false;
}

} // namespace lolol::utils