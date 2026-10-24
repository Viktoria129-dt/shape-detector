cmake_minimum_required(VERSION 3.16)
project(ShapeDetector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(project_figures main.cpp)

target_include_directories(project_figures PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(project_figures PRIVATE ${OpenCV_LIBS})