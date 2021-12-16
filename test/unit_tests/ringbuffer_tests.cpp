/*
 *  Ring Buffer Unit Tests
 *
 *  Copyright (C) 2019, HENSOLDT Cyber GmbH
 *
 *  SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include "ringbuffer.h"

#include <cstdint>
#include <climits>

using ::testing::ElementsAreArray;

class Test_RingBuffer : public testing::Test
{
    protected:

    void SetUp() override
    {
        ring_buffer_init(&ring_buffer);
    }

    void fillRingBuffer(const size_t count)
    {
        for(size_t i = 0; i < count; i++)
        {
            ring_buffer_queue(&ring_buffer, (char)(i % (RING_BUFFER_SIZE - 1)));
        }
    }

    ring_buffer_t ring_buffer;
};

class Test_RingBuffer_filled : public Test_RingBuffer
{
    protected:

    void SetUp() override
    {
        Test_RingBuffer::SetUp();

        fillRingBuffer(items_count);
    }

    inline static const size_t items_count = 100;
};

class Test_RingBuffer_string : public Test_RingBuffer
{
    protected:

    void SetUp() override
    {
        Test_RingBuffer::SetUp();

        ring_buffer_queue_arr(&ring_buffer, test_string, sizeof(test_string));
    }

    inline static const char test_string[] = "Hello, Ring Buffer!";
};

class Test_RingBuffer_full : public Test_RingBuffer
{
    protected:

    void SetUp() override
    {
        Test_RingBuffer::SetUp();

        const size_t ringBufferMax = (RING_BUFFER_SIZE - 1);
        fillRingBuffer(ringBufferMax);
    }
};

TEST_F(Test_RingBuffer_filled, insert_100_items)
{
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, peek_3rd)
{
    const char thirdElementIdx = 3;
    char item;

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(1, ring_buffer_peek(&ring_buffer, &item, thirdElementIdx));
    EXPECT_EQ(3, item);
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, peek_out_of_range)
{
    const char outOfRangeIdx = items_count;
    char item;

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_peek(&ring_buffer, &item, outOfRangeIdx));
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, dequeue_all_items)
{
    EXPECT_FALSE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));

    char item;
    for(size_t i = 0; ring_buffer_dequeue(&ring_buffer, &item) > 0; ++i)
    {
        EXPECT_EQ(items_count - i - 1, ring_buffer_num_items(&ring_buffer));
        EXPECT_EQ(i, item) << "index " << i;
    }

    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_FALSE(ring_buffer_is_full(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_string, add_string)
{
    EXPECT_FALSE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_EQ(sizeof(test_string), ring_buffer_num_items(&ring_buffer));

    char item;
    for(size_t i = 0; ring_buffer_dequeue(&ring_buffer, &item) > 0; ++i)
    {
        EXPECT_EQ(test_string[i], item) << "index " << i;
    }

    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_FALSE(ring_buffer_is_full(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_string, dequeue_array_in_2_parts)
{
    const size_t half_of_string = sizeof(test_string) / 2;
    char array[half_of_string];

    EXPECT_EQ(
        half_of_string,
        ring_buffer_dequeue_arr(&ring_buffer, array, sizeof(array)));

    EXPECT_EQ(half_of_string, ring_buffer_num_items(&ring_buffer));
    EXPECT_THAT(std::vector<char>(array, array + sizeof(array)),
                ::testing::ElementsAreArray(test_string, half_of_string));

    EXPECT_EQ(
        half_of_string,
        ring_buffer_dequeue_arr(&ring_buffer, array, sizeof(array)));

    EXPECT_THAT(std::vector<char>(array, array + sizeof(array)),
                ::testing::ElementsAreArray(
                    test_string + half_of_string,
                    half_of_string));

    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_FALSE(ring_buffer_is_full(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));

    EXPECT_EQ(
        0,
        ring_buffer_dequeue_arr(&ring_buffer, array, 1));
}

TEST_F(Test_RingBuffer, empty)
{
    EXPECT_FALSE(ring_buffer_is_full(&ring_buffer));
    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));

    char item;
    EXPECT_EQ(
        0,
        ring_buffer_dequeue_arr(&ring_buffer, &item, sizeof(item)));
}

TEST_F(Test_RingBuffer, overfill_buffer)
{
    for(size_t i = 0; i < 1000; i++)
    {
        ring_buffer_queue(&ring_buffer, (i % (RING_BUFFER_SIZE - 1)));
    }

    // One slot is always empty for the look ahead.
    const size_t max_num_items = RING_BUFFER_SIZE - 1;
    EXPECT_EQ(max_num_items, ring_buffer_num_items(&ring_buffer));

    EXPECT_TRUE(ring_buffer_is_full(&ring_buffer));

    char item;
    for(size_t i = 111; ring_buffer_dequeue(&ring_buffer, &item) > 0; ++i)
    {
        ASSERT_EQ((i % (RING_BUFFER_SIZE - 1)), item);
    }

    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_full, head_chases_tail)
{
    EXPECT_TRUE(ring_buffer_is_full(&ring_buffer));

    // Iterating over the ring twice
    for(size_t j = 0; j < 2; j++)
    {
        for(size_t i = 0; i < (RING_BUFFER_SIZE - 1); i++)
        {
            char item;
            ASSERT_TRUE(ring_buffer_dequeue(&ring_buffer, &item));
            ASSERT_EQ(i, item);
            ring_buffer_queue(&ring_buffer, i);
        }
    }
}

TEST_F(Test_RingBuffer, tail_chases_head)
{
    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));

    // Iterating over the ring twice
    for(size_t j = 0; j < 2; j++)
    {
        for(size_t i = 0; i < (RING_BUFFER_SIZE - 1); i++)
        {
            ring_buffer_queue(&ring_buffer, i);

            char item;
            ASSERT_TRUE(ring_buffer_dequeue(&ring_buffer, &item));
            ASSERT_EQ(i, item);
        }
    }
}

TEST_F(Test_RingBuffer_full, head_chases_tail_concurrently)
{
    EXPECT_TRUE(ring_buffer_is_full(&ring_buffer));

    const size_t iterations = 4096;

    std::thread producer
    {[this]
        {
            for(size_t i = (RING_BUFFER_SIZE - 1); i < iterations; i++)
            {
                if(ring_buffer_is_full(&ring_buffer))
                {
                    std::this_thread::yield();
                    --i;
                }
                else
                {
                    ring_buffer_queue(
                        &ring_buffer,
                        (char)(i % (RING_BUFFER_SIZE - 1)));
                }
            }
        }
    };

    std::thread consumer
    {[this]
        {
            for(size_t i = 0; i < iterations; i++)
            {
                char item;
                if(ring_buffer_dequeue(&ring_buffer, &item))
                {
                    EXPECT_EQ(i % (RING_BUFFER_SIZE - 1), item);
                }
                else
                {
                    std::this_thread::yield();
                    --i;
                }
            }
        }
    };

    producer.join();
    consumer.join();

    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer))
        << ring_buffer_num_items(&ring_buffer);
}

TEST_F(Test_RingBuffer, tail_chases_head_concurrently)
{
    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));

    const size_t iterations = 4096;

    std::thread producer
    {[this]
        {
            for(size_t i = 0; i < iterations; i++)
            {
                if(ring_buffer_is_full(&ring_buffer))
                {
                    std::this_thread::yield();
                    --i;
                }
                else
                {
                    ring_buffer_queue(
                        &ring_buffer,
                        (char)(i % (RING_BUFFER_SIZE - 1)));
                }
            }
        }
    };

    std::thread consumer
    {[this]
        {
            for(size_t i = 0; i < iterations; i++)
            {
                char item;
                if(ring_buffer_dequeue(&ring_buffer, &item))
                {
                    EXPECT_EQ(i % (RING_BUFFER_SIZE - 1), item);
                }
                else
                {
                    std::this_thread::yield();
                    --i;
                }
            }
        }
    };

    producer.join();
    consumer.join();

    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer))
        << ring_buffer_num_items(&ring_buffer);
}
