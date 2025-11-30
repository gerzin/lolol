#include "devicemanager.hpp"

#include <spdlog/spdlog.h>

#include "utils/framelimiter.hpp"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <thread>

namespace lolol
{

DeviceManager::DeviceManager(std::string_view device_name) : mDeviceName(device_name)
{
    spdlog::info("Initializing DeviceManager for device: {}", device_name);

    create_virtual_camera();
    wait_for_device();
    create_video_device();

    mReady = true;
    spdlog::info("DeviceManager initialization complete");
}

DeviceManager::~DeviceManager()
{
    spdlog::info("Shutting down DeviceManager");
    mVideoDevice.reset();

    // Clean up virtual camera
    std::string cmd = "sudo modprobe -r v4l2loopback";
    if (std::system(cmd.c_str()) != 0)
    {
        spdlog::warn("Failed to remove v4l2loopback module");
    }
}

auto DeviceManager::get_video_device() -> VideoDevice &
{
    if (!mReady || !mVideoDevice)
    {
        throw std::runtime_error("DeviceManager is not ready");
    }
    return *mVideoDevice;
}

auto DeviceManager::is_ready() const -> bool
{
    return mReady;
}

void DeviceManager::create_virtual_camera()
{
    spdlog::info("Creating virtual camera device: {}", mDeviceName);

    // Load v4l2loopback kernel module with device label
    std::string cmd = "sudo modprobe v4l2loopback video_nr=42 card_label=\"" + mDeviceName + "\"";

    if (std::system(cmd.c_str()) != 0)
    {
        throw std::runtime_error("Failed to load v4l2loopback kernel module");
    }

    mDevicePath = "/dev/video42";
    spdlog::info("Virtual camera module loaded, expected path: {}", mDevicePath);
}

void DeviceManager::wait_for_device()
{
    spdlog::info("Waiting for device to become available: {}", mDevicePath);

    constexpr int max_attempts{50};
    constexpr auto wait_interval{std::chrono::milliseconds(100)};

    for (int attempt{0}; attempt < max_attempts; ++attempt)
    {
        lolol::utils::FrameLimiter loopLimiter{10};
        if (std::filesystem::exists(mDevicePath))
        {
            loopLimiter.disable();
            // Extra delay to ensure device is fully initialized
            lolol::utils::FrameLimiter limiter{20};
            spdlog::info("Device is now available: {}", mDevicePath);
            return;
        }
    }

    throw std::runtime_error("Timeout waiting for virtual camera device: " + mDevicePath);
}

void DeviceManager::create_video_device()
{
    spdlog::info("Creating VideoDevice for: {}", mDevicePath);

    try
    {
        mVideoDevice = std::make_unique<VideoDevice>(mDevicePath);
        spdlog::info("VideoDevice created successfully");
    }
    catch (const std::exception &ex)
    {
        spdlog::error("Failed to create VideoDevice: {}", ex.what());
        throw;
    }
}

} // namespace lolol
