/*
 *  Unit tests of ring buffer of Foo structure.
 *
 *  Copyright (C) 2019, HENSOLDT Cyber GmbH
 *
 *  SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include "ringbuffer.h"
#include "ring_buffer_Foo.h"

#include <cstdint>
#include <climits>

using ::testing::ElementsAreArray;

// One slot is wasted since this is a "one look ahead" type of a ring buffer.
#define RING_BUFFER_AVAILABLE_SLOTS (RING_BUFFER_SIZE - 1)

class Test_RingBuffer_Foo : public testing::Test
{
    protected:

    void SetUp() override
    {
        ring_buffer_Foo_init(
            &ring_buffer,
            buffer,
            RING_BUFFER_SIZE);
    }

    void fillRingBuffer(const size_t count)
    {
        for(size_t i = 0; i < count; i++)
        {
            const Foo ringBufferElement{ i };
            ring_buffer_Foo_queue(
                &ring_buffer,
                ringBufferElement);
        }
    }

    ring_buffer_t ring_buffer;
    Foo buffer[RING_BUFFER_SIZE];
};

class Test_RingBuffer_Foo_filled : public Test_RingBuffer_Foo
{
    protected:

    void SetUp() override
    {
        Test_RingBuffer_Foo::SetUp();

        fillRingBuffer(items_count);
    }

    inline static const size_t items_count = 100;
};

class Test_RingBuffer_Foo_full : public Test_RingBuffer_Foo
{
    protected:

    void SetUp() override
    {
        Test_RingBuffer_Foo::SetUp();

        fillRingBuffer(ringBufferMax);
    }

    inline static const size_t ringBufferMax = RING_BUFFER_AVAILABLE_SLOTS;
};

TEST_F(Test_RingBuffer_Foo_filled, insert_100_items)
{
    EXPECT_EQ(
        items_count,
        ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, peek_3rd)
{
    const size_t thirdElementIdx = 3;
    Foo item;

    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(
        1,
        ring_buffer_Foo_peek(
            &ring_buffer,
            &item,
            thirdElementIdx));
    EXPECT_EQ(3, item.idx);
    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, peek_out_of_range)
{
    const size_t outOfRangeIdx = items_count;
    Foo item;

    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(
        0,
        ring_buffer_Foo_peek(
            &ring_buffer,
            &item,
            outOfRangeIdx));
    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, peek_arr)
{
    const size_t thirdElementIdx = 3;
    const size_t elementsCount = items_count / 2;
    Foo elements[elementsCount];

    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(
        elementsCount,
        ring_buffer_Foo_peek_arr(
            &ring_buffer,
            elements,
            elementsCount,
            thirdElementIdx));

    for(size_t i = 0; i < elementsCount; ++i)
    {
        EXPECT_EQ(i + thirdElementIdx, elements[i].idx);
    }

    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, peek_arr_too_many)
{
    const size_t thirdElementIdx = 3;
    const size_t expectedElementsCount = items_count - thirdElementIdx;
    const size_t elementsCount = items_count * 2;
    Foo elements[elementsCount];

    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(
        expectedElementsCount,
        ring_buffer_Foo_peek_arr(
            &ring_buffer,
            elements,
            elementsCount,
            thirdElementIdx));

    for(size_t i = 0; i < expectedElementsCount; ++i)
    {
        EXPECT_EQ(i + thirdElementIdx, elements[i].idx);
    }

    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, peek_arr_out_of_range)
{
    const size_t outOfRangeIdx = items_count;
    const size_t expectedElementsCount = 0;
    const size_t elementsCount = items_count / 2;
    Foo elements[elementsCount];

    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(
        0,
        ring_buffer_Foo_peek_arr(
            &ring_buffer,
            elements,
            elementsCount,
            outOfRangeIdx));

    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, pop)
{
    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(1, ring_buffer_Foo_pop(&ring_buffer));
    EXPECT_EQ(items_count - 1, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_full, pop)
{
    EXPECT_EQ(ringBufferMax, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(1, ring_buffer_Foo_pop(&ring_buffer));
    EXPECT_EQ(ringBufferMax - 1, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo, pop_empty)
{
    EXPECT_EQ(0, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_Foo_pop(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, pop_arr)
{
    const size_t poppedCount = 42;
    EXPECT_EQ(items_count, ring_buffer_Foo_num_items(&ring_buffer));
    EXPECT_EQ(
        poppedCount,
        ring_buffer_Foo_pop_arr(&ring_buffer, poppedCount));
    EXPECT_EQ(
        items_count - poppedCount,
        ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, dequeue_all_items)
{
    EXPECT_FALSE(ring_buffer_Foo_is_empty(&ring_buffer));
    EXPECT_EQ(
        items_count,
        ring_buffer_Foo_num_items(&ring_buffer));

    Foo item;
    for(
        size_t i = 0;
        ring_buffer_Foo_dequeue(&ring_buffer, &item) > 0;
        ++i)
    {
        ASSERT_EQ(
            items_count - i - 1,
            ring_buffer_Foo_num_items(&ring_buffer));
        ASSERT_EQ(i, item.idx) << "index " << i;
    }

    EXPECT_TRUE(ring_buffer_Foo_is_empty(&ring_buffer));
    EXPECT_FALSE(ring_buffer_Foo_is_full(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo, empty)
{
    EXPECT_FALSE(ring_buffer_Foo_is_full(&ring_buffer));
    EXPECT_TRUE(ring_buffer_Foo_is_empty(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_Foo_num_items(&ring_buffer));

    Foo item;
    EXPECT_EQ(
        0,
        ring_buffer_Foo_dequeue_arr(
            &ring_buffer,
            &item,
            1));
}

TEST_F(Test_RingBuffer_Foo, available)
{
    const size_t available_slots = ring_buffer_Foo_available(&ring_buffer);
    EXPECT_EQ(RING_BUFFER_AVAILABLE_SLOTS, available_slots);

    for(size_t i = 1; i <= available_slots; ++i)
    {
        ring_buffer_Foo_queue(&ring_buffer, { i });
        ASSERT_EQ(
            (available_slots - i),
            ring_buffer_Foo_available(&ring_buffer));
    }

    EXPECT_EQ(0, ring_buffer_Foo_available(&ring_buffer));
    EXPECT_EQ(available_slots, ring_buffer_Foo_num_items(&ring_buffer));

    EXPECT_TRUE(ring_buffer_Foo_is_full(&ring_buffer));
    EXPECT_FALSE(ring_buffer_Foo_is_empty(&ring_buffer));

    for(size_t i = 0; i < available_slots; ++i)
    {
        ASSERT_EQ(i, ring_buffer_Foo_available(&ring_buffer));

        Foo item;
        ASSERT_EQ(1, ring_buffer_Foo_dequeue(&ring_buffer, &item));
    }

    EXPECT_EQ(available_slots, ring_buffer_Foo_available(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_Foo_num_items(&ring_buffer));

    EXPECT_FALSE(ring_buffer_Foo_is_full(&ring_buffer));
    EXPECT_TRUE(ring_buffer_Foo_is_empty(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo, overfill_buffer)
{
    for(size_t i = 0; i < (RING_BUFFER_SIZE * 3); i++)
    {
        ring_buffer_Foo_queue(&ring_buffer, {i});
    }

    // One slot is always empty for the look ahead.
    const size_t max_num_items = RING_BUFFER_AVAILABLE_SLOTS;
    EXPECT_EQ(
        max_num_items,
        ring_buffer_Foo_num_items(&ring_buffer));

    EXPECT_TRUE(ring_buffer_Foo_is_full(&ring_buffer));

    Foo item;
    for(
        size_t i = 2049;
        ring_buffer_Foo_dequeue(&ring_buffer, &item) > 0;
        ++i)
    {
        ASSERT_EQ(i, item.idx);
    }

    EXPECT_TRUE(ring_buffer_Foo_is_empty(&ring_buffer));
    EXPECT_EQ(0, ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_full, head_chases_tail)
{
    EXPECT_TRUE(ring_buffer_Foo_is_full(&ring_buffer));

    // Iterating over the ring twice
    for(size_t j = 0; j < 2; j++)
    {
        for(size_t i = 0; i < RING_BUFFER_AVAILABLE_SLOTS; i++)
        {
            Foo item;
            ASSERT_TRUE(
                ring_buffer_Foo_dequeue(&ring_buffer, &item));
            ASSERT_EQ(i, item.idx);
            ring_buffer_Foo_queue(&ring_buffer, {i});
        }
    }
}

TEST_F(Test_RingBuffer_Foo, no_overwrite)
{
    EXPECT_EQ(
        1,
        ring_buffer_Foo_queue_no_overwrite(&ring_buffer, {42}));
    EXPECT_EQ(
        1,
        ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo_filled, no_overwrite)
{
    EXPECT_EQ(
        1,
        ring_buffer_Foo_queue_no_overwrite(&ring_buffer, {42}));
}

TEST_F(Test_RingBuffer_Foo_full, no_overwrite)
{
    EXPECT_EQ(
        ringBufferMax,
        ring_buffer_Foo_num_items(&ring_buffer));

    EXPECT_EQ(
        0,
        ring_buffer_Foo_queue_no_overwrite(&ring_buffer, {42}));
    EXPECT_EQ(
        ringBufferMax,
        ring_buffer_Foo_num_items(&ring_buffer));
}

TEST_F(Test_RingBuffer_Foo, tail_chases_head)
{
    EXPECT_TRUE(ring_buffer_Foo_is_empty(&ring_buffer));

    // Iterating over the ring twice
    for(size_t j = 0; j < 2; j++)
    {
        for(size_t i = 0; i < (RING_BUFFER_AVAILABLE_SLOTS); i++)
        {
            ring_buffer_Foo_queue(&ring_buffer, {i});

            Foo item;
            ASSERT_TRUE(
                ring_buffer_Foo_dequeue(&ring_buffer, &item));
            ASSERT_EQ(i, item.idx);
        }
    }
}

TEST_F(Test_RingBuffer_Foo_full, head_chases_tail_concurrently)
{
    EXPECT_TRUE(ring_buffer_Foo_is_full(&ring_buffer));

    const size_t iterations = 4096;

    std::thread producer
    {[this]
        {
            for(size_t i = ringBufferMax; i < iterations; i++)
            {
                const Foo item {i};

                if(0 == ring_buffer_Foo_queue_no_overwrite(
                            &ring_buffer,
                            item))
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
                Foo item;

                if(ring_buffer_Foo_dequeue(&ring_buffer, &item))
                {
                    EXPECT_EQ(i, item.idx);
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

    EXPECT_TRUE(ring_buffer_Foo_is_empty(&ring_buffer))
        << ring_buffer_Foo_num_items(&ring_buffer);
}

TEST_F(Test_RingBuffer_Foo, tail_chases_head_concurrently)
{
    EXPECT_TRUE(ring_buffer_Foo_is_empty(&ring_buffer));

    const size_t iterations = 4096;

    std::thread producer
    {[this]
        {
            for(size_t i = 0; i < iterations; i++)
            {
                if(0 == ring_buffer_Foo_queue_no_overwrite(
                            &ring_buffer,
                            {i}))
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
                Foo item;
                if(ring_buffer_Foo_dequeue(&ring_buffer, &item))
                {
                    EXPECT_EQ(i, item.idx);
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

    EXPECT_TRUE(ring_buffer_Foo_is_empty(&ring_buffer))
        << ring_buffer_Foo_num_items(&ring_buffer);
}
