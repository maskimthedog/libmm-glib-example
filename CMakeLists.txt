#
# Copyright (c) 2025 Rob Krakora
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# urnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.10)
project(ModemApiExample VERSION 1.0 LANGUAGES C)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(MM REQUIRED mm-glib)

# Build library
add_library(modem_api STATIC modem_api.c)
target_include_directories(modem_api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${GLIB_INCLUDE_DIRS} ${MM_INCLUDE_DIRS})
target_link_libraries(modem_api ${GLIB_LIBRARIES} ${MM_LIBRARIES})

# Build example application
add_executable(modem_example example_app.c)
target_link_libraries(modem_example modem_api ${GLIB_LIBRARIES} ${MM_LIBRARIES})

# Enable Doxygen
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()

# Installation
install(TARGETS modem_api modem_example
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib)
install(FILES modem_api.h DESTINATION include)
