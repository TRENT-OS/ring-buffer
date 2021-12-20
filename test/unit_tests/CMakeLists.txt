#
# Unit tests.
#
# Copyright (C) 2021, HENSOLDT Cyber GmbH
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.17)

project(ringbuffer_tests CXX)
set(CMAKE_CXX_STANDARD 20)

#-------------------------------------------------------------------------------
add_subdirectory(ring_buffers)

add_executable("${PROJECT_NAME}_test"
    ringbuffer_tests_Foo.cpp
    ringbuffer_tests_uint8_t.cpp
)

target_link_libraries("${PROJECT_NAME}_test"
    PRIVATE
        test_main
        ring_buffers
)

find_package(GTest REQUIRED)
gtest_discover_tests("${PROJECT_NAME}_test" AUTO)
