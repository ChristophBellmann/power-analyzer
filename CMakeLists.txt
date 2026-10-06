cmake_minimum_required(VERSION 3.16.0)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/components")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(power-analyzer)

spiffs_create_partition_image(spiffs ${CMAKE_SOURCE_DIR}/data/power-analyzer FLASH_IN_PROJECT)
