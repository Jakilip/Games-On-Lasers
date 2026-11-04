cmake_minimum_required(VERSION 4.2)
project(GOL)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL3 REQUIRED CONFIG)

file(GLOB_RECURSE GOL_SOURCES CONFIGURE_DEPENDS
        DACs/*.cpp
        Utilities/*.cpp
        Rendering/*.cpp
        Input/*.cpp)

# plt-windows / plt-posix are mutually exclusive platform backends
list(FILTER GOL_SOURCES EXCLUDE REGEX "plt-(windows|posix)\\.cpp$")

if(WIN32)
    list(APPEND GOL_SOURCES DACs/HeliosDAC/idn/plt-windows.cpp)
else()
    list(APPEND GOL_SOURCES DACs/HeliosDAC/idn/plt-posix.cpp)
endif()

add_library(GOL STATIC ${GOL_SOURCES}
        Utilities/Collisions.cpp
        Utilities/Collisions.h
        Utilities/Random.cpp
        Utilities/Random.h)
target_include_directories(GOL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GOL
        PUBLIC usb-1.0 ws2_32 winmm iphlpapi
        PRIVATE SDL3::SDL3)

add_executable(GOLTest testing.cpp)
target_link_libraries(GOLTest PRIVATE GOL)