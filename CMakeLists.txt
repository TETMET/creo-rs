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

set(CREO_TOOLKIT_INCLUDE $ENV{CREO_TOOLKIT}\\includes)
set(CREO_TOOLKIT_LIB $ENV{CREO_TOOLKIT}\\x86e_win64\\obj)

cmake_print_variables(
    CREO_TOOLKIT_INCLUDE
    CREO_TOOLKIT_LIB
)

include_directories(
    include
    src
)

add_library(aslm SHARED src/main.cpp)
set_property(TARGET aslm PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded")
target_include_directories(aslm PUBLIC ${CREO_TOOLKIT_INCLUDE})

target_link_libraries(aslm PUBLIC 
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
