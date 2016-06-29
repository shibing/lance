cmake_minimum_required(VERSION 3.0.0)

project(lance C CXX)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(SYSTEM
    deps/http-parser
    deps/rapidjson/include
)

add_definitions(-DASIO_STAND_ALONE)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

find_package (Threads)

file(GLOB SORUCES src/*.hpp src/*.cpp deps/http-parser/http_parser.c)

add_executable(lance ${SORUCES})
target_link_libraries (lance ${CMAKE_THREAD_LIBS_INIT})
