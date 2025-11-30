#include "videodevice.hpp"
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace
{
/**
 * Wrapper for ioctl system call with error handling
 *
 * @note Throws std::runtime_error on failure
 */
void xioctl(int fd, unsigned long request, void *arg)
{
    if (ioctl(fd, request, arg) == -1)
    {
        throw std::runtime_error("ioctl failed: " + std::to_string(errno));
    }
}

} // namespace
namespace lolol
{

VideoDevice::Buffer::~Buffer()
{
    if (start)
    {
        munmap(start, length);
    }
}

auto VideoDevice::open_device() noexcept(false)
{
    VideoDeviceHandle handle{open(mDevicePath.c_str(), O_RDWR)};
    if (mHandle < 0)
    {
        throw std::runtime_error("Failed to open device: " + mDevicePath);
    }
    return handle;
}

VideoDevice::VideoDevice(std::string_view devicePath) noexcept(false) : mDevicePath(devicePath)
{
    mHandle = open_device();
    init_format();
    init_request_buffers();
    init_buffers();
}

VideoDevice::~VideoDevice()
{
    xioctl(mHandle, VIDIOC_STREAMOFF, &mBufType);
    close(mHandle);
}

auto VideoDevice::get_device_path() const -> std::string_view
{
    return mDevicePath;
}

void VideoDevice::init_format() noexcept(false)
{
    mFmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    mFmt.fmt.pix.width = PIXEL_WIDTH;
    mFmt.fmt.pix.height = PIXEL_HEIGHT;
    mFmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    mFmt.fmt.pix.field = V4L2_FIELD_NONE;
    xioctl(mHandle, VIDIOC_S_FMT, &mFmt);
    xioctl(mHandle, VIDIOC_G_FMT, &mFmt);
}

void VideoDevice::init_request_buffers() noexcept(false)
{
    mReqBufs.count = BUFFER_COUNT;
    mReqBufs.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    mReqBufs.memory = V4L2_MEMORY_MMAP;
    xioctl(mHandle, VIDIOC_REQBUFS, &mReqBufs);
    xioctl(mHandle, VIDIOC_REQBUFS, &mReqBufs);
    if (mReqBufs.count < 2)
    {
        throw std::runtime_error("Insufficient buffer memory");
    }
}

void VideoDevice::init_buffers() noexcept(false)
{
    mBuffers.resize(mReqBufs.count);
    for (std::uint32_t i = 0; i < mReqBufs.count; ++i)
    {
        v4l2_buffer buf{};
        buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        xioctl(mHandle, VIDIOC_QUERYBUF, &buf);
        mBuffers[i].length = buf.length;
        mBuffers[i].start = mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, mHandle, buf.m.offset);
        if (mBuffers[i].start == MAP_FAILED)
        {
            throw std::runtime_error("mmap failed");
        }
        std::memset(mBuffers[i].start, 0, mBuffers[i].length);
        xioctl(mHandle, VIDIOC_QBUF, &buf);
    }

    xioctl(mHandle, VIDIOC_STREAMON, &mBufType);
}
} // namespace lolol