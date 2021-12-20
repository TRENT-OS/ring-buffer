/*
 *  Ring buffer of uint8_t.
 *
 *  Copyright (C) 2019, HENSOLDT Cyber GmbH
 *
 *  SPDX-License-Identifier: MIT
 */
#ifndef D59F8EB1_F761_4574_BC98_181924B196A0
#define D59F8EB1_F761_4574_BC98_181924B196A0

#ifdef __cplusplus
extern "C"
{
#endif

#include "ringbuffer.h"

#define RING_BUFFER_SIZE 128

ring_buffer_T(uint8_t)
ring_buffer_T_DECL(uint8_t)

typedef ring_buffer_uint8_t_t ring_buffer_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // D59F8EB1_F761_4574_BC98_181924B196A0
