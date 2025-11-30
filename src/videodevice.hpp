#pragma once

#include "frame.hpp"
#include <cstdint>
#include <linux/videodev2.h>
#include <string>
#include <string_view>
#include <vector>

namespace lolol
{

class VideoDevice
{
    using VideoDeviceHandle = int;

    constexpr static std::uint32_t PIXEL_WIDTH{1280};
    constexpr static std::uint32_t PIXEL_HEIGHT{720};
    constexpr static std::uint32_t BUFFER_COUNT{4};

    struct Buffer
    {
        void *start{nullptr};
        std::size_t length{0};

        Buffer() = default;
        ~Buffer();
    };

  public:
    explicit VideoDevice(std::string_view devicePath) noexcept(false);
    ~VideoDevice();
    [[nodiscard]] auto get_device_path() const -> std::string_view;
    void display_frame(const Frame &frame) noexcept(false);

  private:
    [[nodiscard]] auto open_device() noexcept(false);
    void init_format() noexcept(false);
    void init_request_buffers() noexcept(false);
    void init_buffers() noexcept(false);

    std::string mDevicePath;
    VideoDeviceHandle mHandle{-1};
    v4l2_format mFmt{};
    v4l2_requestbuffers mReqBufs{};
    std::vector<Buffer> mBuffers;
    v4l2_buf_type mBufType{V4L2_BUF_TYPE_VIDEO_OUTPUT};
};
} // namespace lolol