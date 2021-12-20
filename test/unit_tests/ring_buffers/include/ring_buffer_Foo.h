/*
 *  Ring buffer of Foo structure.
 *
 *  Copyright (C) 2019, HENSOLDT Cyber GmbH
 *
 *  SPDX-License-Identifier: MIT
 */
#ifndef D03CE6FE_78D8_41C7_A309_AE7573DCFF2E
#define D03CE6FE_78D8_41C7_A309_AE7573DCFF2E

#ifdef __cplusplus
extern "C"
{
#endif

#include "ringbuffer.h"

typedef struct
{
    size_t  idx;
    double  bar;
    int64_t foo;
    size_t  array[42];
} Foo;

#define RING_BUFFER_SIZE 1024

ring_buffer_T(Foo)
ring_buffer_T_DECL(Foo)

typedef ring_buffer_Foo_t ring_buffer_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // D03CE6FE_78D8_41C7_A309_AE7573DCFF2E
