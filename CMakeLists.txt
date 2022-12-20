cmake_minimum_required(VERSION 3.0.2)

project(speed_detector)

add_compile_options(-std=c++14)

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
    ${YAML_CPP_INCLUDE_DIR}
    )

add_executable(
    ${PROJECT_NAME}
     src/${PROJECT_NAME}.cpp)

add_executable(
    data_collecting
    src/data_collecting.cpp
)

target_link_libraries(
    speed_detector
    ${OpenCV_LIBS}
    ${YAML_CPP_LIBRARIES}
    )

target_link_libraries(
    data_collecting
    ${OpenCV_LIBS}
    )