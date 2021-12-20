#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

/**
 * @file
 * Prototypes and structures for the ring buffer module.
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

/**
 * Structure which holds a ring buffer.
 * The buffer contains a buffer array
 * as well as metadata for the ring buffer.
 */
#define ring_buffer_T(_TYPE_) \
    typedef struct ring_buffer_##_TYPE_##_t \
    { \
        /** Index of tail. */ \
        size_t tail_index; \
        /** Index of head. */ \
        size_t head_index; \
        /** Size of the buffer */ \
        size_t size; \
        /** Buffer memory. */ \
        _TYPE_* buffer; \
    } ring_buffer_##_TYPE_##_t;

/**
 * Initializes the ring buffer pointed to by <em>buffer</em>.
 * This function can also be used to empty/reset the buffer.
 * @param ring_buffer The ring buffer to initialize.
 * @param buffer Pointer to the underlying memory of the ring buffer.
 * @param buffer_size Size of the buffer
 */
#define ring_buffer_init_T_DECL(_TYPE_) \
    void ring_buffer_##_TYPE_##_init( \
        ring_buffer_##_TYPE_##_t *ring_buffer, \
        void* buffer, \
        size_t buffer_size);

/**
 * Adds an element to a ring buffer.
 *
 * @param buffer The buffer in which the data should be placed.
 * @param data The element to place.
 */
#define ring_buffer_queue_T_DECL(_TYPE_) \
    void ring_buffer_##_TYPE_##_queue( \
        ring_buffer_##_TYPE_##_t *buffer, _TYPE_ data);

/**
 * Adds an element to a ring buffer only if there is enough space.
 *
 * @param buffer The buffer in which the data should be placed.
 * @param data The element to place.
 * @return 1 if data was written; 0 otherwise.
 */
#define ring_buffer_queue_no_overwrite_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_queue_no_overwrite( \
            ring_buffer_##_TYPE_##_t *buffer, _TYPE_ data);

/**
 * Adds an array of elements to a ring buffer.
 * @param buffer The buffer in which the data should be placed.
 * @param data A pointer to the array of elements to place in the queue.
 * @param size The size of the array.
 */
#define ring_buffer_queue_arr_T_DECL(_TYPE_) \
    void ring_buffer_##_TYPE_##_queue_arr( \
        ring_buffer_##_TYPE_##_t *buffer, \
        const void *data, \
        size_t size);

/**
 * Returns the oldest element in a ring buffer.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @return 1 if data was returned; 0 otherwise.
 */
#define ring_buffer_dequeue_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_dequeue( \
        ring_buffer_##_TYPE_##_t *buffer, \
        _TYPE_ *data);

/**
 * Returns the <em>len</em> oldest elements in a ring buffer.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the array at which the data should be placed.
 * @param len The maximum number of elements to return.
 * @return The number of elements returned.
 */
#define ring_buffers_dequeue_arr_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_dequeue_arr( \
        ring_buffer_##_TYPE_##_t *buffer, \
        void *data, \
        size_t len);

/**
 * Peeks a ring buffer, i.e. returns an element without removing it.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @param index The index to peek.
 * @return 1 if data was returned; 0 otherwise.
 */
#define ring_buffer_peek_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_peek( \
        ring_buffer_##_TYPE_##_t *buffer, \
        _TYPE_ *data, \
        size_t index);

/**
 * Peeks the <em>len</em> elements in a ring buffer without removing them,
 * starting at a given index.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the array at which the data should be placed.
 * @param len The maximum number of elements to return.
 * @param start_idx Index of the first element to peek.
 * @return The number of elements returned.
 */
#define ring_buffer_peek_arr_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_peek_arr( \
        ring_buffer_##_TYPE_##_t *buffer, \
        void *data, \
        size_t len, \
        size_t start_idx);

/**
 * Removes the next element in the queue.
 * @param buffer The buffer from which the element should be popped.
 * @return 1 if element was popped; 0 otherwise.
 */
#define ring_buffer_pop_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_pop(ring_buffer_##_TYPE_##_t *buffer);

/**
 * Removes the given number of elements in the queue.
 * @param buffer The buffer from which the elements should be popped.
 * @param len The maximum number of elements to be popped.
 * @return 1 if element was popped; 0 otherwise.
 */
#define ring_buffer_pop_arr_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_pop_arr( \
        ring_buffer_##_TYPE_##_t *buffer, \
        size_t len);

/**
 * Returns whether a ring buffer is empty.
 * @param buffer The buffer for which it should be returned whether it is empty.
 * @return 1 if empty; 0 otherwise.
 */
#define ring_buffer_is_empty_T_DECL(_TYPE_) \
    bool ring_buffer_##_TYPE_##_is_empty(ring_buffer_##_TYPE_##_t *buffer);
/**
 * Returns whether a ring buffer is full.
 * @param buffer The buffer for which it should be returned whether it is full.
 * @return 1 if full; 0 otherwise.
 */
#define ring_buffer_is_full_T_DECL(_TYPE_) \
    bool ring_buffer_##_TYPE_##_is_full(ring_buffer_##_TYPE_##_t *buffer);

/**
 * Returns the number of items in a ring buffer.
 * @param buffer The buffer for which the number of items should be returned.
 * @return The number of items in the ring buffer.
 */
#define ring_buffer_num_items_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_num_items(ring_buffer_##_TYPE_##_t *buffer);

/**
 * Returns the amount of available slots.
 * @param buffer The buffer for which the number of available slots should be
 *                  returned.
 * @return The number of items available slots.
 */
#define ring_buffer_available_T_DECL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_available(ring_buffer_##_TYPE_##_t *buffer);

#define ring_buffer_T_DECL(_TYPE_) \
    ring_buffer_init_T_DECL(_TYPE_) \
    ring_buffer_queue_T_DECL(_TYPE_) \
    ring_buffer_queue_no_overwrite_T_DECL(_TYPE_) \
    ring_buffer_queue_arr_T_DECL(_TYPE_) \
    ring_buffer_dequeue_T_DECL(_TYPE_) \
    ring_buffers_dequeue_arr_T_DECL(_TYPE_) \
    ring_buffer_peek_T_DECL(_TYPE_) \
    ring_buffer_peek_arr_T_DECL(_TYPE_) \
    ring_buffer_pop_T_DECL(_TYPE_) \
    ring_buffer_pop_arr_T_DECL(_TYPE_) \
    ring_buffer_is_empty_T_DECL(_TYPE_) \
    ring_buffer_is_full_T_DECL(_TYPE_) \
    ring_buffer_num_items_T_DECL(_TYPE_) \
    ring_buffer_available_T_DECL(_TYPE_)

#include "ringbuffer.impl.h"

#endif /* RINGBUFFER_H */

#ifdef __cplusplus
} // extern "C"
#endif
