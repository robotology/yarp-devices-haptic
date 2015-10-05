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
    add_subdirectory(client)
yarp_end_plugin_library(geomagicmod)

install(TARGETS geomagicmod EXPORT geomagicmod
		LIBRARY DESTINATION lib COMPONENT shlib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include/geomagicmod COMPONENT dev)

file(WRITE  ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake "# Copyright: (C) 2015 iCub Facility\n")
file(APPEND ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake "# Authors: Ugo Pattacini <ugo.pattacini@iit.it>\n")
file(APPEND ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake "# CopyPolicy: Released under the terms of the GNU GPL v2.0.\n")
file(APPEND ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake "\n")
file(APPEND ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake "if(NOT geomagic_FOUND)\n")
file(APPEND ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake "set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})\n")
file(APPEND ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake "    set(geomagic_FOUND TRUE)\n")
file(APPEND ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake "endif()\n")

yarp_install(FILES conf/${PROJECT_NAME}.xml                          DESTINATION ${GEOMAGIC_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})
yarp_install(FILES interface/IGeomagic.h                             DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
yarp_install(FILES ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake DESTINATION ${GEOMAGIC_DYNAMIC_PLUGINS_INSTALL_DIR})

