#
# Utilities Library
#
# Copyright (C) 2021, HENSOLDT Cyber GmbH
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.17)

#-------------------------------------------------------------------------------
project(Ring-Buffer C)

#-------------------------------------------------------------------------------
# LIBRARY
#-------------------------------------------------------------------------------
add_library(${PROJECT_NAME}
    "ringbuffer.c"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC "."
)
