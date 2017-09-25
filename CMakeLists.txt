cmake_minimum_required(VERSION 3.8)
project(CIS2750A0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -g")
set(SOURCE_FILES test.c src/LinkedListAPI.c include/LinkedListAPI.h include/CalendarParser.h src/CalendarParser.c src/help.c include/help.h)
add_executable(CIS2750A0 ${SOURCE_FILES})
