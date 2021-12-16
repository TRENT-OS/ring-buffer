#
# The Test Main Target.
#
# Links gtest framwork and enables testing.
#
# Copyright (C) 2021, HENSOLDT Cyber GmbH
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.17)

if (BUILD_TESTING)
    set(CMAKE_CXX_STANDARD 20)

    project(test_main LANGUAGES CXX)

    add_library(${PROJECT_NAME}
        "test_main.cpp")

    find_package(GTest REQUIRED)

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            GTest::GTest
            GTest::Main
    )
endif ()
