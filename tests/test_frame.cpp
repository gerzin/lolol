#include "frame.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <cstring>

using lolol::Frame;

class FrameTest : public ::testing::Test
{
  protected:
    Frame frame;
};

TEST_F(FrameTest, DefaultConstructorCreatesEmptyFrame)
{
    EXPECT_EQ(frame.data_size(), 0);
    EXPECT_TRUE(frame.buffer().empty());
    EXPECT_TRUE(frame.view().empty());
}

TEST_F(FrameTest, ResizeChangesSize)
{
    frame.resize(100);
    EXPECT_EQ(frame.data_size(), 100);
    EXPECT_EQ(frame.buffer().size(), 100);
    EXPECT_EQ(frame.view().size(), 100);
}

TEST_F(FrameTest, SetDataCopiesData)
{
    const std::array<char, 5> test_data = {'H', 'e', 'l', 'l', 'o'};

    frame.set_data(test_data.data(), test_data.size());

    EXPECT_EQ(frame.data_size(), 5);
    EXPECT_EQ(std::memcmp(frame.data(), test_data.data(), 5), 0);
}

TEST_F(FrameTest, SetDataWithDifferentSizes)
{
    const std::array<char, 3> small_data = {'A', 'B', 'C'};
    frame.set_data(small_data.data(), small_data.size());
    EXPECT_EQ(frame.data_size(), 3);

    const std::array<char, 10> large_data = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    frame.set_data(large_data.data(), large_data.size());
    EXPECT_EQ(frame.data_size(), 10);
    EXPECT_EQ(std::memcmp(frame.data(), large_data.data(), 10), 0);
}

TEST_F(FrameTest, BufferAccessProvidesMutableAccess)
{
    frame.resize(10);
    auto &buffer = frame.buffer();

    buffer[0] = std::byte{42};
    buffer[9] = std::byte{99};

    EXPECT_EQ(static_cast<int>(buffer[0]), 42);
    EXPECT_EQ(static_cast<int>(buffer[9]), 99);
}

TEST_F(FrameTest, ConstBufferAccessProvidesConstAccess)
{
    frame.resize(5);
    frame.buffer()[0] = std::byte{123};

    const Frame &const_frame = frame;
    const auto &const_buffer = const_frame.buffer();

    EXPECT_EQ(static_cast<int>(const_buffer[0]), 123);
    EXPECT_EQ(const_buffer.size(), 5);
}

TEST_F(FrameTest, ViewProvidesSpanAccess)
{
    const std::array<char, 4> test_data = {10, 20, 30, 40};
    frame.set_data(test_data.data(), test_data.size());

    auto view = frame.view();

    EXPECT_EQ(view.size(), 4);
    EXPECT_EQ(static_cast<int>(view[0]), 10);
    EXPECT_EQ(static_cast<int>(view[1]), 20);
    EXPECT_EQ(static_cast<int>(view[2]), 30);
    EXPECT_EQ(static_cast<int>(view[3]), 40);
}

TEST_F(FrameTest, DataPointerIsValidAfterSetData)
{
    const std::array<char, 6> test_data = {'W', 'o', 'r', 'l', 'd', '!'};
    frame.set_data(test_data.data(), test_data.size());

    const void *data_ptr = frame.data();
    EXPECT_NE(data_ptr, nullptr);

    const char *char_ptr = static_cast<const char *>(data_ptr);
    EXPECT_EQ(std::memcmp(char_ptr, test_data.data(), 6), 0);
}

TEST_F(FrameTest, MultipleResizesWork)
{
    frame.resize(10);
    EXPECT_EQ(frame.data_size(), 10);

    frame.resize(50);
    EXPECT_EQ(frame.data_size(), 50);

    frame.resize(5);
    EXPECT_EQ(frame.data_size(), 5);

    frame.resize(0);
    EXPECT_EQ(frame.data_size(), 0);
}

TEST_F(FrameTest, SetDataOnEmptyFrame)
{
    const std::array<char, 3> test_data = {1, 2, 3};

    EXPECT_EQ(frame.data_size(), 0);
    frame.set_data(test_data.data(), test_data.size());
    EXPECT_EQ(frame.data_size(), 3);
}

TEST_F(FrameTest, SetDataOverwritesPreviousData)
{
    const std::array<char, 3> first_data = {'A', 'B', 'C'};
    frame.set_data(first_data.data(), first_data.size());

    const std::array<char, 5> second_data = {'X', 'Y', 'Z', '1', '2'};
    frame.set_data(second_data.data(), second_data.size());

    EXPECT_EQ(frame.data_size(), 5);
    EXPECT_EQ(std::memcmp(frame.data(), second_data.data(), 5), 0);
}

TEST_F(FrameTest, BufferAndViewStayInSync)
{
    frame.resize(8);
    frame.buffer()[0] = std::byte{77};
    frame.buffer()[7] = std::byte{88};

    auto view = frame.view();
    EXPECT_EQ(static_cast<int>(view[0]), 77);
    EXPECT_EQ(static_cast<int>(view[7]), 88);
}

TEST_F(FrameTest, CStyleInterfaceWorksWithCFunctions)
{
    // Simulate working with a C API
    unsigned char c_buffer[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    frame.set_data(c_buffer, sizeof(c_buffer));

    // Verify we can pass to C functions
    EXPECT_EQ(frame.data_size(), sizeof(c_buffer));

    const unsigned char *result = static_cast<const unsigned char *>(frame.data());
    for (size_t i = 0; i < sizeof(c_buffer); ++i)
    {
        EXPECT_EQ(result[i], c_buffer[i]);
    }
}
