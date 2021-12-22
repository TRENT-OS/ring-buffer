#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RINGBUFFER_IMPL_H
#define RINGBUFFER_IMPL_H

/**
 * @file
 * Implementation of ring buffer functions.
 */

static
void ring_buffer_init(
    ring_buffer_t *ring_buffer,
    void* buffer,
    size_t buffer_size)
{
    ring_buffer->tail_index = 0;
    ring_buffer->head_index = 0;
    ring_buffer->buffer = (uint8_t*)buffer;
    ring_buffer->size = buffer_size;
}

static
void ring_buffer_queue(ring_buffer_t *buffer, uint8_t data) {
  /* Is buffer full? */
  if(ring_buffer_is_full(buffer)) {
    /* Is going to overwrite the oldest byte */
    /* Increase tail index */
    buffer->tail_index = (buffer->tail_index + 1) % buffer->size;
  }

  /* Place data in buffer */
  buffer->buffer[buffer->head_index] = data;
  buffer->head_index = (buffer->head_index + 1) % buffer->size;
}

static
size_t ring_buffer_queue_no_overwrite(ring_buffer_t *buffer, uint8_t data) {
  /* Is buffer full? */
  if(ring_buffer_is_full(buffer)) {
    return 0;
  }

  /* Place data in buffer */
  buffer->buffer[buffer->head_index] = data;
  buffer->head_index = (buffer->head_index + 1) % buffer->size;

  return 1;
}

static
void ring_buffer_queue_arr(ring_buffer_t *buffer, const void *data, size_t size) {
  /* Add bytes; one by one */
  size_t i;
  for(i = 0; i < size; i++) {
    ring_buffer_queue(buffer, ((uint8_t*)data)[i]);
  }
}

static
size_t ring_buffer_dequeue(ring_buffer_t *buffer, uint8_t *data) {
  if(ring_buffer_is_empty(buffer)) {
    /* No items */
    return 0;
  }

  *data = buffer->buffer[buffer->tail_index];
  buffer->tail_index = (buffer->tail_index + 1) % buffer->size;
  return 1;
}

static
size_t ring_buffer_dequeue_arr(ring_buffer_t *buffer, void *data, size_t len) {
  if(ring_buffer_is_empty(buffer)) {
    /* No items */
    return 0;
  }

  uint8_t *data_ptr = (uint8_t*)data;
  size_t cnt = 0;
  while((cnt < len) && ring_buffer_dequeue(buffer, data_ptr)) {
    cnt++;
    data_ptr++;
  }
  return cnt;
}

static
size_t ring_buffer_peek(ring_buffer_t *buffer, uint8_t *data, size_t index) {
  if(index >= ring_buffer_num_items(buffer)) {
    /* No items at index */
    return 0;
  }

  /* Add index to pointer */
  size_t data_index = (buffer->tail_index + index) % buffer->size;
  *data = buffer->buffer[data_index];
  return 1;
}

static
size_t ring_buffer_peek_arr(
    ring_buffer_t *buffer,
    void *data,
    size_t len,
    size_t start_idx)
{
  if(ring_buffer_is_empty(buffer)) {
    /* No items */
    return 0u;
  }

  uint8_t* destination = (uint8_t*)data;
  size_t rslt = 0u;
  for(size_t index = start_idx; index < (start_idx + len); ++index)
  {
    if(0 == ring_buffer_peek(buffer, destination, index))
    {
        break;
    }

    ++destination;
    ++rslt;
  }

  return rslt;
}

static
size_t ring_buffer_pop(ring_buffer_t *buffer)
{
  if(ring_buffer_is_empty(buffer)) {
    /* No items */
    return 0;
  }

  buffer->tail_index = (buffer->tail_index + 1) % buffer->size;

  return 1;
}

static
size_t ring_buffer_pop_arr(ring_buffer_t *buffer, size_t len)
{
  size_t poppedCount = 0;

  for(size_t i = 0; i < len; ++i)
  {
    if(0 == ring_buffer_pop(buffer))
    {
        break;
    }

    ++poppedCount;
  }

  return poppedCount;
}

#endif /* RINGBUFFER_IMPL_H */

#ifdef __cplusplus
} // extern "C"
#endif
