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
 * The size of a ring buffer.
 * Due to the design only <tt> RING_BUFFER_SIZE-1 </tt> items
 * can be contained in the buffer.
 * The buffer size must be a power of two.
*/
#define RING_BUFFER_SIZE 128


/**
 * Simplifies the use of <tt>struct ring_buffer_t</tt>.
 */
typedef struct ring_buffer_t ring_buffer_t;

/**
 * Structure which holds a ring buffer.
 * The buffer contains a buffer array
 * as well as metadata for the ring buffer.
 */
struct ring_buffer_t {
  /** Buffer memory. */
  uint8_t buffer[RING_BUFFER_SIZE];
  /** Index of tail. */
  size_t tail_index;
  /** Index of head. */
  size_t head_index;
};

/**
 * Initializes the ring buffer pointed to by <em>buffer</em>.
 * This function can also be used to empty/reset the buffer.
 * @param buffer The ring buffer to initialize.
 */
void ring_buffer_init(ring_buffer_t *buffer);

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
void ring_buffer_queue_arr(ring_buffer_t *buffer, const uint8_t *data, size_t size);

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
size_t ring_buffer_dequeue_arr(ring_buffer_t *buffer, uint8_t *data, size_t len);
/**
 * Peeks a ring buffer, i.e. returns an element without removing it.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @param index The index to peek.
 * @return 1 if data was returned; 0 otherwise.
 */
size_t ring_buffer_peek(ring_buffer_t *buffer, uint8_t *data, size_t index);

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
  return ((buffer->head_index + 1) % RING_BUFFER_SIZE) == buffer->tail_index;
}

/**
 * Returns the number of items in a ring buffer.
 * @param buffer The buffer for which the number of items should be returned.
 * @return The number of items in the ring buffer.
 */
inline size_t ring_buffer_num_items(ring_buffer_t *buffer) {
    return (buffer->head_index >= buffer->tail_index)
            ? buffer->head_index - buffer->tail_index
            : RING_BUFFER_SIZE - buffer->tail_index + buffer->head_index;
}

#endif /* RINGBUFFER_H */

#ifdef __cplusplus
} // extern "C"
#endif
