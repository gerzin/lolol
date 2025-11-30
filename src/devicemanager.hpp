#pragma once

#include "videodevice.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace lolol
{

class DeviceManager
{
  public:
    explicit DeviceManager(std::string_view device_name);
    ~DeviceManager();

    [[nodiscard]] auto get_video_device() -> VideoDevice &;
    [[nodiscard]] auto is_ready() const -> bool;

  private:
    void create_virtual_camera();
    void wait_for_device();
    void create_video_device();

    std::string mDeviceName;
    std::string mDevicePath;
    std::unique_ptr<VideoDevice> mVideoDevice;
    bool mReady{false};
};

} // namespace lolol