cmake_minimum_required(VERSION 2.8)
project(circle-detector)

SET(CMAKE_BUILD_TYPE debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O4 -march=native -Wfatal-errors")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-configs")

set(whycon_srcs circle_detector.cpp many_circle_detector.cpp localization_system.cpp)

# TODO: agregar find para libpng
find_package(GSL REQUIRED)
set(LIBS ${GSL_LIBRARIES})

# create config file
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_SOURCE_DIR}/config.h)

# generate whycon shared library
add_library(whycon SHARED ${whycon_srcs})

# generate extra executables
add_executable(localization-system main.cpp)
target_link_libraries(localization-system whycon ${LIBS})
