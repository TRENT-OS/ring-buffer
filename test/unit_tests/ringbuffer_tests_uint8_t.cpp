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

#define RING_BUFFER_SIZE 128
// One slot is wasted since this is a "one look ahead" type of a ring buffer.
#define RING_BUFFER_AVAILABLE_SLOTS (RING_BUFFER_SIZE - 1)

class Test_RingBuffer : public testing::Test
{
    protected:

    void SetUp() override
    {
        ring_buffer_init(&ring_buffer, buffer, RING_BUFFER_SIZE);
    }

    void fillRingBuffer(const size_t count)
    {
        for(size_t i = 0; i < count; i++)
        {
            ring_buffer_queue(
                &ring_buffer,
                (uint8_t)(i % RING_BUFFER_AVAILABLE_SLOTS));
        }
    }

    ring_buffer_t ring_buffer;
    uint8_t buffer[RING_BUFFER_SIZE];
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

        ring_buffer_queue_arr(
            &ring_buffer,
            (uint8_t*)test_string,
            sizeof(test_string));
    }

    inline static const char test_string[] = "Hello, Ring Buffer!";
};

class Test_RingBuffer_full : public Test_RingBuffer
{
    protected:

    void SetUp() override
    {
        Test_RingBuffer::SetUp();

        fillRingBuffer(ringBufferMax);
    }

    inline static const size_t ringBufferMax = RING_BUFFER_AVAILABLE_SLOTS;
};

TEST_F(Test_RingBuffer_filled, insert_100_items)
{
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(
        (RING_BUFFER_AVAILABLE_SLOTS - items_count),
        ring_buffer_available(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, peek_3rd)
{
    const size_t thirdElementIdx = 3;
    uint8_t item;

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(1, ring_buffer_peek(&ring_buffer, &item, thirdElementIdx));
    EXPECT_EQ(3, item);
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, peek_out_of_range)
{
    const size_t outOfRangeIdx = items_count;
    uint8_t item;

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_peek(&ring_buffer, &item, outOfRangeIdx));
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, peek_arr)
{
    const size_t thirdElementIdx = 3;
    const size_t elementsCount = items_count / 2;
    uint8_t elements[elementsCount];

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(
        elementsCount,
        ring_buffer_peek_arr(
            &ring_buffer,
            elements,
            elementsCount,
            thirdElementIdx));

    for(size_t i = 0; i < elementsCount; ++i)
    {
        EXPECT_EQ(i + thirdElementIdx, elements[i]);
    }

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, peek_arr_too_many)
{
    const size_t thirdElementIdx = 3;
    const size_t expectedElementsCount = items_count - thirdElementIdx;
    const size_t elementsCount = items_count * 2;
    uint8_t elements[elementsCount];

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(
        expectedElementsCount,
        ring_buffer_peek_arr(
            &ring_buffer,
            elements,
            elementsCount,
            thirdElementIdx));

    for(size_t i = 0; i < expectedElementsCount; ++i)
    {
        EXPECT_EQ(i + thirdElementIdx, elements[i]);
    }

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, peek_arr_out_of_range)
{
    const size_t outOfRangeIdx = items_count;
    const size_t expectedElementsCount = 0;
    const size_t elementsCount = items_count / 2;
    uint8_t elements[elementsCount];

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(
        0,
        ring_buffer_peek_arr(
            &ring_buffer,
            elements,
            elementsCount,
            outOfRangeIdx));

    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, pop)
{
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(1, ring_buffer_pop(&ring_buffer));
    EXPECT_EQ(items_count - 1, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_full, pop)
{
    EXPECT_EQ(ringBufferMax, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(1, ring_buffer_pop(&ring_buffer));
    EXPECT_EQ(ringBufferMax - 1, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer, pop_empty)
{
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_pop(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, pop_arr)
{
    const size_t poppedCount = 42;
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));
    EXPECT_EQ(poppedCount, ring_buffer_pop_arr(&ring_buffer, poppedCount));
    EXPECT_EQ(items_count - poppedCount, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, dequeue_all_items)
{
    EXPECT_FALSE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_EQ(items_count, ring_buffer_num_items(&ring_buffer));

    uint8_t item;
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

    uint8_t item;
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
        ring_buffer_dequeue_arr(&ring_buffer, (uint8_t*)array, sizeof(array)));

    EXPECT_EQ(half_of_string, ring_buffer_num_items(&ring_buffer));
    EXPECT_THAT(std::vector<char>(array, array + sizeof(array)),
                ::testing::ElementsAreArray(test_string, half_of_string));

    EXPECT_EQ(
        half_of_string,
        ring_buffer_dequeue_arr(&ring_buffer, (uint8_t*)array, sizeof(array)));

    EXPECT_THAT(std::vector<char>(array, array + sizeof(array)),
                ::testing::ElementsAreArray(
                    test_string + half_of_string,
                    half_of_string));

    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_FALSE(ring_buffer_is_full(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));

    EXPECT_EQ(
        0,
        ring_buffer_dequeue_arr(&ring_buffer, (uint8_t*)array, 1));
}

TEST_F(Test_RingBuffer, empty)
{
    EXPECT_FALSE(ring_buffer_is_full(&ring_buffer));
    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));

    uint8_t item;
    EXPECT_EQ(
        0,
        ring_buffer_dequeue_arr(&ring_buffer, &item, 1));
}

TEST_F(Test_RingBuffer, available)
{
    const size_t available_slots = ring_buffer_available(&ring_buffer);
    EXPECT_EQ(RING_BUFFER_AVAILABLE_SLOTS, available_slots);

    for(size_t i = 1; i <= available_slots; ++i)
    {
        ring_buffer_queue(&ring_buffer, (uint8_t)i);
        ASSERT_EQ(available_slots - i, ring_buffer_available(&ring_buffer));
    }

    EXPECT_EQ(0, ring_buffer_available(&ring_buffer));
    EXPECT_EQ(available_slots, ring_buffer_num_items(&ring_buffer));

    EXPECT_TRUE(ring_buffer_is_full(&ring_buffer));
    EXPECT_FALSE(ring_buffer_is_empty(&ring_buffer));

    for(size_t i = 0; i < available_slots; ++i)
    {
        ASSERT_EQ(i, ring_buffer_available(&ring_buffer));

        uint8_t item;
        ASSERT_EQ(1, ring_buffer_dequeue(&ring_buffer, &item));
    }

    EXPECT_EQ(available_slots, ring_buffer_available(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_num_items(&ring_buffer));

    EXPECT_FALSE(ring_buffer_is_full(&ring_buffer));
    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));
}

TEST_F(Test_RingBuffer, overfill_buffer)
{
    for(size_t i = 0; i < 1000; i++)
    {
        ring_buffer_queue(&ring_buffer, (i % RING_BUFFER_AVAILABLE_SLOTS));
    }

    // One slot is always empty for the look ahead.
    const size_t max_num_items = RING_BUFFER_AVAILABLE_SLOTS;
    EXPECT_EQ(max_num_items, ring_buffer_num_items(&ring_buffer));

    EXPECT_TRUE(ring_buffer_is_full(&ring_buffer));

    uint8_t item;
    for(size_t i = 111; ring_buffer_dequeue(&ring_buffer, &item) > 0; ++i)
    {
        ASSERT_EQ((i % RING_BUFFER_AVAILABLE_SLOTS), item);
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
        for(size_t i = 0; i < RING_BUFFER_AVAILABLE_SLOTS; i++)
        {
            uint8_t item;
            ASSERT_TRUE(ring_buffer_dequeue(&ring_buffer, &item));
            ASSERT_EQ(i, item);
            ring_buffer_queue(&ring_buffer, i);
        }
    }
}

TEST_F(Test_RingBuffer, no_overwrite)
{
    EXPECT_EQ(1, ring_buffer_queue_no_overwrite(&ring_buffer, 42));
    EXPECT_EQ(1, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_filled, no_overwrite)
{
    EXPECT_EQ(1, ring_buffer_queue_no_overwrite(&ring_buffer, 42));
}

TEST_F(Test_RingBuffer_full, no_overwrite)
{
    EXPECT_EQ(ringBufferMax, ring_buffer_num_items(&ring_buffer));

    EXPECT_EQ(0, ring_buffer_queue_no_overwrite(&ring_buffer, 42));
    EXPECT_EQ(ringBufferMax, ring_buffer_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer, tail_chases_head)
{
    EXPECT_TRUE(ring_buffer_is_empty(&ring_buffer));

    // Iterating over the ring twice
    for(size_t j = 0; j < 2; j++)
    {
        for(size_t i = 0; i < RING_BUFFER_AVAILABLE_SLOTS; i++)
        {
            ring_buffer_queue(&ring_buffer, i);

            uint8_t item;
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
            for(size_t i = RING_BUFFER_AVAILABLE_SLOTS; i < iterations; i++)
            {
                if(0 == ring_buffer_queue_no_overwrite(
                            &ring_buffer,
                            (uint8_t)(i % RING_BUFFER_AVAILABLE_SLOTS)))
                {
                    std::this_thread::yield();
                    --i;
                }
            }
        }
    };

    std::thread consumer
    {[this]
        {
            for(size_t i = 0; i < iterations; i++)
            {
                uint8_t item;
                if(ring_buffer_dequeue(&ring_buffer, &item))
                {
                    EXPECT_EQ(i % RING_BUFFER_AVAILABLE_SLOTS, item);
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
                if(0 == ring_buffer_queue_no_overwrite(
                            &ring_buffer,
                            (uint8_t)(i % RING_BUFFER_AVAILABLE_SLOTS)))
                {
                    std::this_thread::yield();
                    --i;
                }
            }
        }
    };

    std::thread consumer
    {[this]
        {
            for(size_t i = 0; i < iterations; i++)
            {
                uint8_t item;
                if(ring_buffer_dequeue(&ring_buffer, &item))
                {
                    EXPECT_EQ(i % RING_BUFFER_AVAILABLE_SLOTS, item);
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
