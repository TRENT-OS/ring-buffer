#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RINGBUFFER_IMPL_H
#define RINGBUFFER_IMPL_H

#include "ringbuffer.h"

/**
 * @file
 * Implementation of ring buffer functions.
 */
#define ring_buffer_init_T_IMPL(_TYPE_) \
    void ring_buffer_##_TYPE_##_init( \
        ring_buffer_##_TYPE_##_t *ring_buffer, \
        void* buffer, \
        size_t size) \
    { \
        ring_buffer->tail_index = 0; \
        ring_buffer->head_index = 0; \
        ring_buffer->size       = size; \
        ring_buffer->buffer     = (_TYPE_*)buffer; \
    }

#define ring_buffer_queue_T_IMPL(_TYPE_) \
    void ring_buffer_##_TYPE_##_queue( \
        ring_buffer_##_TYPE_##_t *buffer, \
        _TYPE_ data) \
    { \
        /* Is buffer full? */ \
        if(ring_buffer_##_TYPE_##_is_full(buffer)) { \
            /* Is going to overwrite the oldest byte */ \
            /* Increase tail index */ \
            buffer->tail_index = (buffer->tail_index + 1) % buffer->size; \
        } \
 \
        /* Place data in buffer */ \
        buffer->buffer[buffer->head_index] = data; \
        buffer->head_index = (buffer->head_index + 1) % buffer->size; \
    }

#define ring_buffer_queue_no_overwrite_T_IMPL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_queue_no_overwrite( \
        ring_buffer_##_TYPE_##_t *buffer, \
        _TYPE_ data) \
    { \
        /* Is buffer full? */ \
        if(ring_buffer_##_TYPE_##_is_full(buffer)) { \
            return 0; \
        } \
 \
        /* Place data in buffer */ \
        buffer->buffer[buffer->head_index] = data; \
        buffer->head_index = (buffer->head_index + 1) % buffer->size; \
 \
        return 1; \
    }

#define ring_buffer_queue_arr_T_IMPL(_TYPE_) \
    void ring_buffer_##_TYPE_##_queue_arr( \
        ring_buffer_##_TYPE_##_t *buffer, \
        const void *data, size_t size) \
    { \
        /* Add bytes; one by one */ \
        size_t i; \
        for(i = 0; i < size; i++) { \
            ring_buffer_##_TYPE_##_queue( \
                buffer, \
                ((_TYPE_*)data)[i]); \
        } \
    }

#define ring_buffer_dequeue_T_IMPL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_dequeue( \
        ring_buffer_##_TYPE_##_t *buffer, \
        _TYPE_ *data) \
    { \
        if(ring_buffer_##_TYPE_##_is_empty(buffer)) { \
            /* No items */ \
            return 0; \
        } \
 \
        *data = buffer->buffer[buffer->tail_index]; \
        buffer->tail_index = (buffer->tail_index + 1) % buffer->size; \
        return 1; \
    }

#define ring_buffer_dequeue_arr_T_IMPL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_dequeue_arr( \
        ring_buffer_##_TYPE_##_t *buffer, \
        void *data, \
        size_t len) \
    { \
        if(ring_buffer_##_TYPE_##_is_empty(buffer)) { \
            /* No items */ \
            return 0; \
        } \
 \
        _TYPE_ *data_ptr = (_TYPE_*)data; \
        size_t cnt = 0; \
        while((cnt < len) \
            && ring_buffer_##_TYPE_##_dequeue(buffer, data_ptr)) \
        { \
            cnt++; \
            data_ptr++; \
        } \
        return cnt; \
    }

#define ring_buffer_peek_T_IMPL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_peek( \
        ring_buffer_##_TYPE_##_t *buffer, \
        _TYPE_ *data, \
        size_t index) \
    { \
        if(index >= ring_buffer_##_TYPE_##_num_items(buffer)) { \
            /* No items at index */ \
            return 0; \
        } \
 \
        /* Add index to pointer */ \
        size_t data_index = (buffer->tail_index + index) % buffer->size; \
        *data = buffer->buffer[data_index]; \
        return 1; \
    }

#define ring_buffer_peek_arr_T_IMPL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_peek_arr( \
        ring_buffer_##_TYPE_##_t *buffer, \
        void *data, \
        size_t len, \
        size_t start_idx) \
    { \
        if(ring_buffer_##_TYPE_##_is_empty(buffer)) { \
            /* No items */ \
            return 0u; \
        } \
 \
        _TYPE_* destination = (_TYPE_*)data; \
        size_t rslt = 0u; \
        for(size_t index = start_idx; index < (start_idx + len); ++index) \
        { \
            if(0 == ring_buffer_##_TYPE_##_peek(buffer, destination, index)) \
            { \
                break; \
            } \
 \
            ++destination; \
            ++rslt; \
        } \
 \
        return rslt; \
    }

#define ring_buffer_pop_T_IMPL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_pop(ring_buffer_##_TYPE_##_t *buffer) \
    { \
    if(ring_buffer_##_TYPE_##_is_empty(buffer)) { \
        /* No items */ \
        return 0; \
    } \
 \
    buffer->tail_index = (buffer->tail_index + 1) % buffer->size; \
 \
    return 1; \
    }

#define ring_buffer_pop_arr_T_IMPL(_TYPE_) \
    size_t ring_buffer_##_TYPE_##_pop_arr( \
        ring_buffer_##_TYPE_##_t *buffer, \
        size_t len) \
    { \
        size_t poppedCount = 0; \
 \
        for(size_t i = 0; i < len; ++i) \
        { \
            if(0 == ring_buffer_##_TYPE_##_pop(buffer)) \
            { \
                break; \
            } \
 \
            ++poppedCount; \
        } \
 \
      return poppedCount; \
    }

#define ring_buffer_is_empty_T_IMPL(_TYPE_) \
    inline bool ring_buffer_##_TYPE_##_is_empty( \
        ring_buffer_##_TYPE_##_t *buffer) \
    { \
        return (buffer->head_index == buffer->tail_index); \
    }

#define ring_buffer_is_full_T_IMPL(_TYPE_) \
    inline bool ring_buffer_##_TYPE_##_is_full( \
        ring_buffer_##_TYPE_##_t *buffer) \
    { \
        return ((buffer->head_index + 1) % buffer->size) \
                == buffer->tail_index; \
    }

#define ring_buffer_num_items_T_IMPL(_TYPE_) \
    inline size_t ring_buffer_##_TYPE_##_num_items( \
        ring_buffer_##_TYPE_##_t *buffer) \
    { \
        return (buffer->head_index >= buffer->tail_index) \
            ? buffer->head_index - buffer->tail_index \
            : buffer->size - buffer->tail_index + buffer->head_index; \
    }

#define ring_buffer_available_T_IMPL(_TYPE_) \
    inline size_t ring_buffer_##_TYPE_##_available( \
        ring_buffer_##_TYPE_##_t *buffer) \
    { \
        return (buffer->size - 1) \
                - ring_buffer_##_TYPE_##_num_items(buffer); \
    }

#define ring_buffer_T_DEFINE(_TYPE_) \
    ring_buffer_init_T_IMPL(_TYPE_) \
    ring_buffer_queue_T_IMPL(_TYPE_) \
    ring_buffer_queue_no_overwrite_T_IMPL(_TYPE_) \
    ring_buffer_queue_arr_T_IMPL(_TYPE_) \
    ring_buffer_dequeue_T_IMPL(_TYPE_) \
    ring_buffer_dequeue_arr_T_IMPL(_TYPE_) \
    ring_buffer_peek_T_IMPL(_TYPE_) \
    ring_buffer_peek_arr_T_IMPL(_TYPE_) \
    ring_buffer_pop_T_IMPL(_TYPE_) \
    ring_buffer_pop_arr_T_IMPL(_TYPE_) \
    ring_buffer_is_empty_T_IMPL(_TYPE_) \
    ring_buffer_is_full_T_IMPL(_TYPE_) \
    ring_buffer_num_items_T_IMPL(_TYPE_) \
    ring_buffer_available_T_IMPL(_TYPE_)

#endif /* RINGBUFFER_IMPL_H */

#ifdef __cplusplus
} // extern "C"
#endif
