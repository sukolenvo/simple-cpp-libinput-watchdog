cmake_minimum_required(VERSION 3.22.1)

project(simple-cpp-in-dog)

find_package(Libinput REQUIRED)
pkg_check_modules(LIBUDEV libudev REQUIRED)

add_library(simple-cpp-in-dog-options INTERFACE)
target_compile_features(simple-cpp-in-dog-options INTERFACE cxx_std_23)

add_subdirectory(src)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.10.0
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)
    enable_testing()
    add_subdirectory(test)
    include(CTest)
endif ()