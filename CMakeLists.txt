cmake_minimum_required(VERSION 3.28)
project(graph_search)

set(CMAKE_CXX_STANDARD 23)

add_executable(graph_search main.cpp include/pathfinding.cpp include/window.cpp include/mst.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR /sfml)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

target_link_libraries(graph_search sfml-system sfml-window sfml-graphics sfml-audio)