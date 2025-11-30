#include "frame.hpp"

#include <cstring>

namespace lolol
{

auto Frame::buffer() -> std::vector<std::byte> &
{
    return mBuffer;
}

auto Frame::buffer() const -> const std::vector<std::byte> &
{
    return mBuffer;
}

auto Frame::view() const -> std::span<const std::byte>
{
    return mBuffer;
}

auto Frame::data_size() const -> std::size_t
{
    return mBuffer.size();
}

auto Frame::data() const -> const void *
{
    return mBuffer.data();
}

auto Frame::set_data(const void *data, std::size_t size) -> void
{
    mBuffer.resize(size);
    std::memcpy(mBuffer.data(), data, size);
}

auto Frame::resize(std::size_t size) -> void
{
    mBuffer.resize(size);
}

} // namespace lolol