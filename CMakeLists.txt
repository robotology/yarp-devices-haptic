# Copyright: (C) 2015 iCub Facility
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.8.9)
project(haptic-device)

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(YarpPlugin)
include(hapticdeviceHelpers)
set(BUILD_SHARED_LIBS TRUE)

yarp_configure_external_installation(hapticdevice WITH_PLUGINS)
yarp_begin_plugin_library(hapticdevicemod)
    add_subdirectory(drivers)
    add_subdirectory(wrapper)
    add_subdirectory(client)
yarp_end_plugin_library(hapticdevicemod)

install(TARGETS hapticdevicemod EXPORT hapticdevicemod
		LIBRARY DESTINATION lib COMPONENT shlib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include/hapticdevicemod COMPONENT dev)

yarp_install(FILES interface/IHapticDevice.h DESTINATION include/hapticdevice)
yarp_install(FILES ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake
             DESTINATION ${HAPTICDEVICE_DYNAMIC_PLUGINS_INSTALL_DIR})

yarp_install(FILES conf/geomagic.xml DESTINATION ${HAPTICDEVICE_CONTEXTS_INSTALL_DIR}/geomagic)
