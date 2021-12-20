#
# Ring Buffers of different types
#
# Copyright (C) 2021, HENSOLDT Cyber GmbH
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.17)

project(ring_buffers C)

#-------------------------------------------------------------------------------
add_library(${PROJECT_NAME}
    src/ring_buffer_Foo.c
    src/ring_buffer_uint8.c
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "include"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Ring-Buffer
)
