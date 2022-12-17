cmake_minimum_required(VERSION 3.0)

project(speed_detector)

find_package(OpenCV REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
    )

add_executable(
    ${PROJECT_NAME}
     src/${PROJECT_NAME}.cpp)

target_link_libraries(
    speed_detector
    ${OpenCV_LIBS}
    )