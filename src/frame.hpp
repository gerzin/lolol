#pragma once
#include <cstddef>
#include <span>
#include <vector>

namespace lolol
{
class Frame
{
  public:
    Frame() = default;
    ~Frame() = default;

    // Modern C++23 interface
    [[nodiscard]] auto buffer() -> std::vector<std::byte> &;
    [[nodiscard]] auto buffer() const -> const std::vector<std::byte> &;
    [[nodiscard]] auto view() const -> std::span<const std::byte>;

    // C-style compatibility interface
    [[nodiscard]] auto data_size() const -> std::size_t;
    [[nodiscard]] auto data() const -> const void *;
    auto set_data(const void *data, std::size_t size) -> void;
    auto resize(std::size_t size) -> void;

  private:
    std::vector<std::byte> mBuffer;
};

} // namespace lolol