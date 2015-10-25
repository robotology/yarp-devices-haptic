# Copyright: (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Ugo Pattacini
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.8.9)
project(tests-geomagic)

find_package(YARP         REQUIRED)
find_package(ICUBcontrib  REQUIRED)
find_package(hapticdevice REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})

include(ICUBcontribHelpers)
icubcontrib_set_default_prefix()

include_directories(${YARP_INCLUDE_DIRS} ${hapticdevice_INCLUDE_DIRS})

add_executable(test-geomagic-force-feedback  test-geomagic-force-feedback.cpp)
add_executable(test-geomagic-retrieve-data   test-geomagic-retrieve-data.cpp)
add_executable(test-geomagic-transformation  test-geomagic-transformation.cpp)

target_link_libraries(test-geomagic-force-feedback ${YARP_LIBRARIES})
target_link_libraries(test-geomagic-retrieve-data  ${YARP_LIBRARIES})
target_link_libraries(test-geomagic-transformation ${YARP_LIBRARIES})

install(TARGETS     test-geomagic-force-feedback
                    test-geomagic-retrieve-data
                    test-geomagic-transformation
        DESTINATION bin)

