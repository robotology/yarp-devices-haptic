# Copyright: (C) 2015 iCub Facility
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_prepare_device(geomagicdriver TYPE GeomagicDriver INCLUDE geomagicDriver.h WRAPPER geomagicwrapper)

if(ENABLE_geomagicmod_geomagicdriver)
    include_directories(${YARP_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${PROJECT_SOURCE_DIR}/interface)

    yarp_add_plugin(geomagicdriver geomagicDriver.h geomagicDriver.cpp)
    target_link_libraries(geomagicdriver ${YARP_LIBRARIES})
    yarp_install(TARGETS geomagicdriver
                 COMPONENT Runtime
                 LIBRARY DESTINATION ${GEOMAGIC_DYNAMIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES geomagicdriver.ini DESTINATION ${GEOMAGIC_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()