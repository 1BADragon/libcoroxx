cmake_minimum_required(VERSION 3.5)
project(libcoroxx C CXX)

option(BUILD_TESTS "Build testing code" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(deps/libcoro)

set(libcoroxx_SRCS
    include/coro.hpp            src/coro.cpp
    include/coro/loop.hpp       src/loop.cpp
    include/coro/rawtask.hpp    src/rawtask.cpp
    include/coro/rawqueue.hpp   src/rawqueue.cpp

    include/coro/queue.hpp
    include/coro/task.hpp
)

add_library(coroxx STATIC ${libcoroxx_SRCS})
target_include_directories(coroxx PUBLIC include/
    PRIVATE src)
target_link_libraries(coroxx coro)
set_target_properties(coroxx
    PROPERTIES
    CXX_STANDARD 17
)

if (BUILD_TESTS)
    enable_testing()

    add_subdirectory(tests)
endif()
