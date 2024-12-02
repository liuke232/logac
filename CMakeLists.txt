cmake_minimum_required(VERSION 3.1)
project(logac) 

option(BUILD_TESTS "Build unit tests" ON)

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS_RELEASE -O2)
set(CMAKE_C_FLAGS_DEBUG -g)

file(GLOB SRC_LIST "src/*.c")

message("SRC_LIST = " ${SRC_LIST})

include_directories(
    include
    src
)

set(LIBS -lpthread)

add_library(logac SHARED ${SRC_LIST})

target_compile_options(logac PRIVATE
    -Wall
    -Wextra
    -Werror
)

# target_link_libraries(logac PUBLIC pthread)

if(BUILD_TESTS)
    add_executable(main test/main.c)
    add_executable(test_msgqueue test/unittest/test_msgqueue.c)

    target_link_libraries(main PRIVATE logac)
    target_link_libraries(test_msgqueue PRIVATE logac)
endif()




