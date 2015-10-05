# Copyright: (C) 2015 iCub Facility
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

file(WRITE  ${PROJECT_SOURCE_DIR}/cmake/generated/${PROJECT_NAME}Config.cmake "# Copyright: (C) 2015 iCub Facility\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/${PROJECT_NAME}Config.cmake "# Authors: Ugo Pattacini <ugo.pattacini@iit.it>\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/${PROJECT_NAME}Config.cmake "# CopyPolicy: Released under the terms of the GNU GPL v2.0.\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/${PROJECT_NAME}Config.cmake "\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/${PROJECT_NAME}Config.cmake "if(NOT geomagic_FOUND)\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/${PROJECT_NAME}Config.cmake "set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/${PROJECT_NAME}Config.cmake "    set(geomagic_FOUND TRUE)\n")
file(APPEND ${PROJECT_SOURCE_DIR}/cmake/generated/${PROJECT_NAME}Config.cmake "endif()\n")
