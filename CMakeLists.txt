cmake_minimum_required(VERSION 3.20)
include(CMakePrintHelpers)
if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW) 
endif()
project(aslm 
    VERSION 0.1 
    DESCRIPTION "ASLM Plugin for Creo" 
    LANGUAGES CXX
)

file(TO_CMAKE_PATH $ENV{CREO_TOOLKIT} CREO_TOOLKIT)
set(CREO_TOOLKIT_INCLUDE ${CREO_TOOLKIT}/includes)
set(CREO_TOOLKIT_LIB ${CREO_TOOLKIT}/x86e_win64/obj)

cmake_print_variables(
    CREO_TOOLKIT_INCLUDE
    CREO_TOOLKIT_LIB
)

# pt_utils
add_library(pt_utils STATIC
    src/utils/pt_utils/pt_utils.cpp
)
target_include_directories(pt_utils PUBLIC includes ${CREO_TOOLKIT_INCLUDE})
set_property(TARGET pt_utils PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded")

# pt_section
add_library(pt_section STATIC
    src/utils/pt_section/pt_section.cpp
)
target_include_directories(pt_section PUBLIC includes ${CREO_TOOLKIT_INCLUDE})
set_property(TARGET pt_section PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded")

# pt_select        
add_library(pt_select STATIC
    src/utils/pt_select/pt_select.cpp
)
target_include_directories(pt_select PUBLIC includes ${CREO_TOOLKIT_INCLUDE})
set_property(TARGET pt_select PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_library(aslm SHARED
    src/main.cpp
)
target_include_directories(aslm PUBLIC includes ${CREO_TOOLKIT_INCLUDE})
set_property(TARGET aslm PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded")

target_link_libraries(aslm PUBLIC 
    pt_section
    pt_utils
    pt_select
    ${CREO_TOOLKIT_LIB}\\protk_dll_NU.lib
    ${CREO_TOOLKIT_LIB}\\ucore.lib 
    ${CREO_TOOLKIT_LIB}\\udata.lib
    kernel32.lib 
    user32.lib 
    wsock32.lib 
    advapi32.lib 
    mpr.lib 
    winspool.lib 
    netapi32.lib 
    psapi.lib 
    gdi32.lib 
    shell32.lib 
    comdlg32.lib 
    ole32.lib 
    ws2_32.lib
)
