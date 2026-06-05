cmake_minimum_required(VERSION 3.21)

project(qt6_runtime_gate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(qt6_runtime_gate qt6_runtime_gate.cpp)
target_link_libraries(qt6_runtime_gate PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
