cmake_minimum_required(VERSION 3.22.0)
cmake_policy(SET CMP0016 NEW)
set(CMAKE_CXX_STANDARD 17)
project(async_server VERSION 1.0.0 DESCRIPTION "Знакомство с асинхронным вводом/выводом на C++")
# find_package(Boost 1.82.0)
find_package(Boost COMPONENTS program_options REQUIRED)
# INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
find_package(Threads REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(tcp_server ${CMAKE_SOURCE_DIR}/src/async_server/tcp_server.cpp)
add_executable(tcp_client ${CMAKE_SOURCE_DIR}/src/async_server/tcp_client.cpp)
target_link_libraries(tcp_client ${Boost_LIBRARIES})
target_link_libraries(tcp_server ${Boost_LIBRARIES})
target_link_libraries(tcp_server Threads::Threads)
target_link_libraries(tcp_client Threads::Threads)