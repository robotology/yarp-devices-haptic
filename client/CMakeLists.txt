# Copyright: (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_prepare_plugin(hapticdeviceclient CATEGORY device
                                       TYPE HapticDeviceClient
                                       INCLUDE hapticdeviceClient.h
                                       EXTRA_CONFIG WRAPPER=hapticdeviceclient)

if(ENABLE_hapticdevicemod_hapticdeviceclient)
    include_directories(${YARP_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${PROJECT_SOURCE_DIR}/interface)
    include_directories(${PROJECT_SOURCE_DIR}/common)

    yarp_add_plugin(hapticdeviceclient hapticdeviceClient.h hapticdeviceClient.cpp
                    ${PROJECT_SOURCE_DIR}/common/common.h)
    target_link_libraries(hapticdeviceclient ${YARP_LIBRARIES})
    yarp_install(TARGETS hapticdeviceclient
                 COMPONENT Runtime
                 LIBRARY DESTINATION ${HAPTICDEVICE_DYNAMIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES hapticdeviceclient.ini DESTINATION ${HAPTICDEVICE_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

