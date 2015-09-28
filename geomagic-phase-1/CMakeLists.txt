# Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
# Author: Ugo Pattacini
# email:  ugo.pattacini@iit.it

cmake_minimum_required(VERSION 2.8.9)
project(geomagic)

find_package(YARP)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

if(LINUX)
    set(GEOMAGIC_INCLUDE_DIRS)
    set(GEOMAGIC_LIBRARIES HDU HD)
else()
    set(GEOMAGIC_DIR $ENV{OH_SDK_BASE} CACHE PATH "Path to Geomagic SDK directory")
    option(GEOMAGIC_WINx64 "Link against x64 libraries" OFF)

    set(GEOMAGIC_INCLUDE_DIRS ${GEOMAGIC_DIR}/include
                              ${GEOMAGIC_DIR}/utilities/include)
    if(GEOMAGIC_WINx64)
        set(GEOMAGIC_LIBRARIES_RELEASE ${GEOMAGIC_DIR}/lib/x64/Release/hd.lib
                                       ${GEOMAGIC_DIR}/lib/x64/Release/hl.lib
                                       ${GEOMAGIC_DIR}/utilities/lib/x64/Release/hdu.lib
                                       ${GEOMAGIC_DIR}/utilities/lib/x64/Release/hlu.lib)
        set(GEOMAGIC_LIBRARIES_DEBUG   ${GEOMAGIC_DIR}/lib/x64/Debug/hd.lib
                                       ${GEOMAGIC_DIR}/lib/x64/Debug/hl.lib
                                       ${GEOMAGIC_DIR}/utilities/lib/x64/Debug/hdu.lib
                                       ${GEOMAGIC_DIR}/utilities/lib/x64/Debug/hlu.lib)
    else()
        set(GEOMAGIC_LIBRARIES_RELEASE ${GEOMAGIC_DIR}/lib/Win32/Release/hd.lib
                                       ${GEOMAGIC_DIR}/lib/Win32/Release/hl.lib
                                       ${GEOMAGIC_DIR}/utilities/lib/Win32/Release/hdu.lib
                                       ${GEOMAGIC_DIR}/utilities/lib/Win32/Release/hlu.lib)
        set(GEOMAGIC_LIBRARIES_DEBUG   ${GEOMAGIC_DIR}/lib/Win32/Debug/hd.lib
                                       ${GEOMAGIC_DIR}/lib/Win32/Debug/hl.lib
                                       ${GEOMAGIC_DIR}/utilities/lib/Win32/Debug/hdu.lib
                                       ${GEOMAGIC_DIR}/utilities/lib/Win32/Debug/hlu.lib)
    endif()
endif()

set(headers include/${PROJECT_NAME}/geomagic.h)
set(sources src/geomagic.cpp src/driver.cpp)

source_group("Header Files" FILES ${headers})
source_group("Source Files" FILES ${sources})

include_directories(${YARP_INCLUDE_DIRS} ${GEOMAGIC_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} ${headers} ${sources})
if(LINUX)
    target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES} ${GEOMAGIC_LIBRARIES})
else()
    target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES})

    foreach(LIB IN ITEMS ${GEOMAGIC_LIBRARIES_RELEASE})
        target_link_libraries(${PROJECT_NAME} optimized ${LIB})
    endforeach()

    foreach(LIB IN ITEMS ${GEOMAGIC_LIBRARIES_DEBUG})
        target_link_libraries(${PROJECT_NAME} debug ${LIB})
    endforeach()
endif()
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
