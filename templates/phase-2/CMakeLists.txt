# Copyright: (C) 2015 iCub Facility
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.8.9)
project(geomagic)

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

include(YarpPlugin)
set(BUILD_SHARED_LIBS TRUE)

yarp_configure_external_installation(${PROJECT_NAME} WITH_PLUGINS)
yarp_begin_plugin_library(geomagicmod)
    add_subdirectory(driver)
    add_subdirectory(wrapper)
yarp_end_plugin_library(geomagicmod)

install(TARGETS geomagicmod EXPORT geomagicmod
		LIBRARY DESTINATION lib COMPONENT shlib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include/geomagicmod COMPONENT dev)

yarp_install(FILES conf/${PROJECT_NAME}.xml DESTINATION ${GEOMAGIC_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})

