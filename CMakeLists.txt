cmake_minimum_required(VERSION 3.8)
project(CIS2750A0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -g")
set(SOURCE_FILES test.c LinkedListAPI.c LinkedListAPI.h CalendarParser.h CalendarParser.c)
add_executable(CIS2750A0 ${SOURCE_FILES})
