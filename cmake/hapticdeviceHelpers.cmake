# Copyright: (C) 2015 iCub Facility
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

file(WRITE  ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake "# Copyright: (C) 2015 iCub Facility\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake "# Authors: Ugo Pattacini <ugo.pattacini@iit.it>\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake "# CopyPolicy: Released under the terms of the GNU GPL v2.0.\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake "\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake "if(NOT hapticdevice_FOUND)\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake "set(hapticdevice_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake "    set(hapticdevice_FOUND TRUE)\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/hapticdeviceConfig.cmake "endif()\n")
