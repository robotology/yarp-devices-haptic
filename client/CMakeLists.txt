# Copyright: (C) 2015 iCub Facility
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_prepare_device(geomagicclient TYPE GeomagicClient INCLUDE geomagicClient.h WRAPPER geomagicclient)

if(ENABLE_geomagicmod_geomagicclient)
    include_directories(${YARP_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${PROJECT_SOURCE_DIR}/interface)
    include_directories(${PROJECT_SOURCE_DIR}/common)

    yarp_add_plugin(geomagicclient geomagicClient.h geomagicClient.cpp
                    ${PROJECT_SOURCE_DIR}/interface/IGeomagic.h
                    ${PROJECT_SOURCE_DIR}/common/common.h)
    target_link_libraries(geomagicclient ${YARP_LIBRARIES})
    yarp_install(TARGETS geomagicclient
                 COMPONENT Runtime
                 LIBRARY DESTINATION ${GEOMAGIC_DYNAMIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES geomagicclient.ini DESTINATION ${GEOMAGIC_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

