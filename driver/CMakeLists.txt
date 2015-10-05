# Copyright: (C) 2015 iCub Facility
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_prepare_device(geomagicdriver TYPE GeomagicDriver INCLUDE geomagicDriver.h WRAPPER geomagicwrapper)

if(ENABLE_geomagicmod_geomagicdriver)
    if(UNIX)
        set(GEOMAGIC_INCLUDE_DIRS)
        set(GEOMAGIC_LIBRARIES HDU HD)
    else()
        set(GEOMAGIC_DIR $ENV{OH_SDK_BASE} CACHE PATH "Path to Geomagic SDK directory")
        option(GEOMAGIC_WINx64 "Link against x64 libraries" OFF)

        set(GEOMAGIC_INCLUDE_DIRS ${GEOMAGIC_DIR}/include
                                  ${GEOMAGIC_DIR}/utilities/include)
        if(GEOMAGIC_WINx64)
            set(GEOMAGIC_LIBRARIES optimized ${GEOMAGIC_DIR}/lib/x64/Release/hd.lib
                                   optimized ${GEOMAGIC_DIR}/lib/x64/Release/hl.lib
                                   optimized ${GEOMAGIC_DIR}/utilities/lib/x64/Release/hdu.lib
                                   optimized ${GEOMAGIC_DIR}/utilities/lib/x64/Release/hlu.lib
                                       debug ${GEOMAGIC_DIR}/lib/x64/Debug/hd.lib
                                       debug ${GEOMAGIC_DIR}/lib/x64/Debug/hl.lib
                                       debug ${GEOMAGIC_DIR}/utilities/lib/x64/Debug/hdu.lib
                                       debug ${GEOMAGIC_DIR}/utilities/lib/x64/Debug/hlu.lib)
        else()
            set(GEOMAGIC_LIBRARIES optimized ${GEOMAGIC_DIR}/lib/Win32/Release/hd.lib
                                   optimized ${GEOMAGIC_DIR}/lib/Win32/Release/hl.lib
                                   optimized ${GEOMAGIC_DIR}/utilities/lib/Win32/Release/hdu.lib
                                   optimized ${GEOMAGIC_DIR}/utilities/lib/Win32/Release/hlu.lib
                                       debug ${GEOMAGIC_DIR}/lib/Win32/Debug/hd.lib
                                       debug ${GEOMAGIC_DIR}/lib/Win32/Debug/hl.lib
                                       debug ${GEOMAGIC_DIR}/utilities/lib/Win32/Debug/hdu.lib
                                       debug ${GEOMAGIC_DIR}/utilities/lib/Win32/Debug/hlu.lib)
        endif()
    endif()
    
	include_directories(${YARP_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories(${GEOMAGIC_INCLUDE_DIRS})
    include_directories(${PROJECT_SOURCE_DIR}/interface)	

    yarp_add_plugin(geomagicdriver geomagicDriver.h geomagicDriver.cpp
                    ${PROJECT_SOURCE_DIR}/interface/IGeomagic.h)
 
    target_link_libraries(geomagicdriver ${YARP_LIBRARIES} ${GEOMAGIC_LIBRARIES})
    yarp_install(TARGETS geomagicdriver
                 COMPONENT Runtime
                 LIBRARY DESTINATION ${GEOMAGIC_DYNAMIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES geomagicdriver.ini DESTINATION ${GEOMAGIC_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
