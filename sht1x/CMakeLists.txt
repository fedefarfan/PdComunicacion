cmake_minimum_required(VERSION 3.10)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
include_directories ("${CMAKE_SOURCE_DIR}/main/include")
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/led_strip)

project(sht1x)



