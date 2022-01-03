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
typedef struct ring_buffer_t {
  /** Index of tail. */
  size_t tail_index;
  /** Index of head. */
  size_t head_index;
  /** Size of the buffer */
  size_t size;
  /** Buffer memory. */
  uint8_t* buffer;
} ring_buffer_t;

/**
 * Initializes the ring buffer pointed to by <em>buffer</em>.
 * This function can also be used to empty/reset the buffer.
 * @param ring_buffer The ring buffer to initialize.
 * @param buffer Pointer to the underlying memory of the ring buffer.
 * @param buffer_size Size of the buffer
 */
void ring_buffer_init(
    ring_buffer_t *ring_buffer,
    void* buffer,
    size_t buffer_size);

/**
 * Adds a byte to a ring buffer.
 *
 * @param buffer The buffer in which the data should be placed.
 * @param data The byte to place.
 */
void ring_buffer_queue(ring_buffer_t *buffer, uint8_t data);

/**
 * Adds a byte to a ring buffer only if there is enough space.
 *
 * @param buffer The buffer in which the data should be placed.
 * @param data The byte to place.
 * @return 1 if data was written; 0 otherwise.
 */
size_t ring_buffer_queue_no_overwrite(ring_buffer_t *buffer, uint8_t data);

/**
 * Adds an array of bytes to a ring buffer.
 * @param buffer The buffer in which the data should be placed.
 * @param data A pointer to the array of bytes to place in the queue.
 * @param size The size of the array.
 */
void ring_buffer_queue_arr(ring_buffer_t *buffer, const void *data, size_t size);

/**
 * Returns the oldest byte in a ring buffer.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @return 1 if data was returned; 0 otherwise.
 */
size_t ring_buffer_dequeue(ring_buffer_t *buffer, uint8_t *data);

/**
 * Returns the <em>len</em> oldest bytes in a ring buffer.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the array at which the data should be placed.
 * @param len The maximum number of bytes to return.
 * @return The number of bytes returned.
 */
size_t ring_buffer_dequeue_arr(ring_buffer_t *buffer, void *data, size_t len);

/**
 * Peeks a ring buffer, i.e. returns an element without removing it.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @param index The index to peek.
 * @return 1 if data was returned; 0 otherwise.
 */
size_t ring_buffer_peek(ring_buffer_t *buffer, uint8_t *data, size_t index);

/**
 * Peeks the <em>len</em> elements in a ring buffer without removing them,
 * starting at a given index.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the array at which the data should be placed.
 * @param len The maximum number of bytes to return.
 * @param start_idx Index of the first element to peek.
 * @return The number of bytes returned.
 */
size_t
ring_buffer_peek_arr(
    ring_buffer_t *buffer,
    void *data,
    size_t len,
    size_t start_idx);

/**
 * Returns whether a ring buffer is empty.
 * @param buffer The buffer for which it should be returned whether it is empty.
 * @return 1 if empty; 0 otherwise.
 */
inline bool ring_buffer_is_empty(ring_buffer_t *buffer) {
  return (buffer->head_index == buffer->tail_index);
}

/**
 * Returns whether a ring buffer is full.
 * @param buffer The buffer for which it should be returned whether it is full.
 * @return 1 if full; 0 otherwise.
 */
inline bool ring_buffer_is_full(ring_buffer_t *buffer) {
  return ((buffer->head_index + 1) % buffer->size) == buffer->tail_index;
}

/**
 * Returns the number of items in a ring buffer.
 * @param buffer The buffer for which the number of items should be returned.
 * @return The number of items in the ring buffer.
 */
inline size_t ring_buffer_num_items(ring_buffer_t *buffer) {
    return (buffer->head_index >= buffer->tail_index)
            ? buffer->head_index - buffer->tail_index
            : buffer->size - buffer->tail_index + buffer->head_index;
}

/**
 * Returns the amount of available slots.
 * @param buffer The buffer for which the number of available slots should be
 *                  returned.
 * @return The number of items available slots.
 */
inline size_t ring_buffer_available(ring_buffer_t *buffer) {
    return (buffer->size - 1) - ring_buffer_num_items(buffer);
}

#endif /* RINGBUFFER_H */

#ifdef __cplusplus
} // extern "C"
#endif
