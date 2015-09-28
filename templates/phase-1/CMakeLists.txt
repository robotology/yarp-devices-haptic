# Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
# Author: Ugo Pattacini
# email:  ugo.pattacini@iit.it

cmake_minimum_required(VERSION 2.8.9)
set(PROJECTNAME geomagic-driver)
project(${PROJECTNAME})

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

set(src_files driver.cpp)
source_group("Source Files" FILES ${src_files})

include_directories(${YARP_INCLUDE_DIRS})
add_executable(${PROJECTNAME} ${src_files})
target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES})
