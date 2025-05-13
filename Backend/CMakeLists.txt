cmake_minimum_required(VERSION 3.5)
project(SIMOP CXX)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cc)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 



# 1. Find libmodbus (installed via apt-get)
find_path(MODBUS_INCLUDE_DIR modbus/modbus.h
    PATHS /usr/include /usr/local/include
    REQUIRED
)

find_library(MODBUS_LIBRARY
    NAMES modbus
    PATHS /usr/lib /usr/local/lib
    REQUIRED
)

if(NOT MODBUS_INCLUDE_DIR OR NOT MODBUS_LIBRARY)
    message(FATAL_ERROR "libmodbus not found! Install with: sudo apt-get install libmodbus-dev")
endif()
message(STATUS "Found libmodbus: ${MODBUS_LIBRARY}")
message(STATUS "Modbus includes: ${MODBUS_INCLUDE_DIR}")


add_subdirectory(Depends/SubModules/drogon) 
target_link_libraries(${PROJECT_NAME} PRIVATE drogon ${MODBUS_LIBRARY})


# and comment out the following lines
# find_package(Drogon CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "c++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(ReActSystem AI_SRC)
aux_source_directory(ReActSystem/AITools AI_TOOLS)
drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models  ${MODBUS_INCLUDE_DIR} )
target_sources(${PROJECT_NAME}
               PRIVATE
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC}
               ${AI_SRC}
               ${AI_TOOLS})
# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

# add_subdirectory(test)
