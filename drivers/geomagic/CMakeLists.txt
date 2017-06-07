# Copyright: (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_prepare_plugin(geomagicdriver CATEGORY device
                                   TYPE GeomagicDriver
                                   INCLUDE geomagicDriver.h
                                   EXTRA_CONFIG WRAPPER=hapticdevicewrapper)

if(ENABLE_hapticdevicemod_geomagicdriver)
    if(UNIX)
        set(GEOMAGIC_INCLUDE_DIRS)
        find_library(GEOMAGIC_LIB_HD  HD)
        find_library(GEOMAGIC_LIB_HDU HDU)
        find_library(GEOMAGIC_LIB_HL  HL)
        find_library(GEOMAGIC_LIB_HLU HLU)
        set(GEOMAGIC_LIBRARIES ${GEOMAGIC_LIB_HD} ${GEOMAGIC_LIB_HDU}
                               ${GEOMAGIC_LIB_HL} ${GEOMAGIC_LIB_HLU})

        if(NOT GEOMAGIC_LIBRARIES)
            message(FATAL_ERROR "Unable to locate Geomagic libraries")
        endif()

        mark_as_advanced(GEOMAGIC_LIB_HD)
        mark_as_advanced(GEOMAGIC_LIB_HDU)
        mark_as_advanced(GEOMAGIC_LIB_HL)
        mark_as_advanced(GEOMAGIC_LIB_HLU)
    else()
        set(GEOMAGIC_DIR $ENV{OH_SDK_BASE} CACHE PATH "Path to Geomagic SDK directory")
        option(GEOMAGIC_WINx64 "Link against x64 libraries" OFF)

        set(GEOMAGIC_INCLUDE_DIRS ${GEOMAGIC_DIR}/include
                                  ${GEOMAGIC_DIR}/utilities/include)
        if(GEOMAGIC_WINx64)
            set(SUBDIR x64)
        else()
            set(SUBDIR Win32)
        endif()

        find_library(GEOMAGIC_LIB_HD_R  hd  PATH ${GEOMAGIC_DIR}/lib/${SUBDIR}/Release           NO_DEFAULT_PATH)
        find_library(GEOMAGIC_LIB_HDU_R hdu PATH ${GEOMAGIC_DIR}/utilities/lib/${SUBDIR}/Release NO_DEFAULT_PATH)
        find_library(GEOMAGIC_LIB_HL_R  hl  PATH ${GEOMAGIC_DIR}/lib/${SUBDIR}/Release           NO_DEFAULT_PATH)
        find_library(GEOMAGIC_LIB_HLU_R hlu PATH ${GEOMAGIC_DIR}/utilities/lib/${SUBDIR}/Release NO_DEFAULT_PATH)

        find_library(GEOMAGIC_LIB_HD_D  hd  PATH ${GEOMAGIC_DIR}/lib/${SUBDIR}/Debug             NO_DEFAULT_PATH)
        find_library(GEOMAGIC_LIB_HDU_D hdu PATH ${GEOMAGIC_DIR}/utilities/lib/${SUBDIR}/Debug   NO_DEFAULT_PATH)
        find_library(GEOMAGIC_LIB_HL_D  hl  PATH ${GEOMAGIC_DIR}/lib/${SUBDIR}/Debug             NO_DEFAULT_PATH)
        find_library(GEOMAGIC_LIB_HLU_D hlu PATH ${GEOMAGIC_DIR}/utilities/lib/${SUBDIR}/Debug   NO_DEFAULT_PATH)

        if(GEOMAGIC_LIB_HD_R-NOTFOUND OR GEOMAGIC_LIB_HDU_R-NOTFOUND OR
           GEOMAGIC_LIB_HL_R-NOTFOUND OR GEOMAGIC_LIB_HLU_R-NOTFOUND OR
           GEOMAGIC_LIB_HD_D-NOTFOUND OR GEOMAGIC_LIB_HDU_D-NOTFOUND OR
           GEOMAGIC_LIB_HL_D-NOTFOUND OR GEOMAGIC_LIB_HLU_D-NOTFOUND)
            message(FATAL_ERROR "Unable to locate Geomagic libraries")
        endif()

        set(GEOMAGIC_LIBRARIES optimized ${GEOMAGIC_LIB_HD_R}
                               optimized ${GEOMAGIC_LIB_HDU_R}
                               optimized ${GEOMAGIC_LIB_HL_R}
                               optimized ${GEOMAGIC_LIB_HLU_R}
                                   debug ${GEOMAGIC_LIB_HD_D}
                                   debug ${GEOMAGIC_LIB_HDU_D}
                                   debug ${GEOMAGIC_LIB_HL_D}
                                   debug ${GEOMAGIC_LIB_HLU_D})

        mark_as_advanced(GEOMAGIC_LIB_HD_R)
        mark_as_advanced(GEOMAGIC_LIB_HDU_R)
        mark_as_advanced(GEOMAGIC_LIB_HL_R)
        mark_as_advanced(GEOMAGIC_LIB_HLU_R)
        mark_as_advanced(GEOMAGIC_LIB_HD_D)
        mark_as_advanced(GEOMAGIC_LIB_HDU_D)
        mark_as_advanced(GEOMAGIC_LIB_HL_D)
        mark_as_advanced(GEOMAGIC_LIB_HLU_D)
    endif()
    
    include_directories(${YARP_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${GEOMAGIC_INCLUDE_DIRS})
    include_directories(${PROJECT_SOURCE_DIR}/interface)

    yarp_add_plugin(geomagicdriver geomagicDriver.h geomagicDriver.cpp)
 
    target_link_libraries(geomagicdriver ${YARP_LIBRARIES} ${GEOMAGIC_LIBRARIES})
    yarp_install(TARGETS geomagicdriver
                 COMPONENT Runtime
                 LIBRARY DESTINATION ${HAPTICDEVICE_DYNAMIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES geomagicdriver.ini DESTINATION ${HAPTICDEVICE_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
