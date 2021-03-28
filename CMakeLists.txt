cmake_minimum_required(VERSION 3.17)
project(LudoGraph)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

set(QT_DEP_PLUGINS platforms/qwindows)


add_executable(LudoGraph)
target_sources(LudoGraph
        PRIVATE
        src/qt_main.cpp
    )
target_link_libraries(LudoGraph
        PUBLIC
            Qt6::Core
            Qt6::Widgets
    )
target_include_directories(LudoGraph PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}>
    )
set_target_properties(LudoGraph PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )


get_filename_component(Qt6_BIN "${Qt6_DIR}/../../../bin" ABSOLUTE)
get_filename_component(Qt6_PLUGINS "${Qt6_DIR}/../../../plugins" ABSOLUTE)

if(MINGW)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

list(TRANSFORM QT_DEP_PLUGINS REPLACE "/.*" ""
        OUTPUT_VARIABLE QT_DEP_PLUGINS_DIRS)
list(TRANSFORM QT_DEP_PLUGINS REPLACE "/" "/${CMAKE_SHARED_LIBRARY_PREFIX}")
list(TRANSFORM QT_DEP_PLUGINS APPEND ${CMAKE_SHARED_LIBRARY_SUFFIX})
list(TRANSFORM QT_DEP_PLUGINS PREPEND ${Qt6_PLUGINS}/)

install(CODE "set(RUN_DEP_PATHS \"${Qt6_BIN}\")")
install(CODE "set(DEP_PLUGIN_PATHS \"${QT_DEP_PLUGINS}\")")
install(CODE "set(DEP_PLUGIN_DIRS \"${QT_DEP_PLUGINS_DIRS}\")")

install(CODE [[
    file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES $<TARGET_FILE:LudoGraph>
        RESOLVED_DEPENDENCIES_VAR _r_deps
        UNRESOLVED_DEPENDENCIES_VAR _u_deps
        POST_EXCLUDE_REGEXES ".*system32.*"
        DIRECTORIES ${RUN_DEP_PATHS}
    )
    foreach(_file ${_r_deps})
        message(${_file})
        file(INSTALL
            DESTINATION "${CMAKE_BINARY_DIR}/bin"
            TYPE SHARED_LIBRARY
            FOLLOW_SYMLINK_CHAIN
            FILES "${_file}"
        )
    endforeach()
    foreach(_file _dir IN ZIP_LISTS DEP_PLUGIN_PATHS DEP_PLUGIN_DIRS)
        message("outputting ${_file} in ${_dir}")
        file(INSTALL
            DESTINATION "${CMAKE_BINARY_DIR}/bin/plugins/${_dir}"
            TYPE SHARED_LIBRARY
            FOLLOW_SYMLINK_CHAIN
            FILES "${_file}"
        )
    endforeach()
    foreach(_missed ${_u_deps})
        message(WARNING "Unresolved Dependency - ${_missed}")
    endforeach()
]])

