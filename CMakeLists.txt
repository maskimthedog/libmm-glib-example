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
