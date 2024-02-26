cmake_minimum_required(VERSION 3.13)
project("sshlirp"
    DESCRIPTION "instant VDE VPN based on libslirp"
    HOMEPAGE_URL "https://github.com/virtualsquare/sshlirp"
    VERSION 0.0.1
    LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2 -O2 -pedantic -Wall -Wextra")

include(GNUInstallDirs)
include(CheckIncludeFile)
include(CheckSymbolExists)

set(LIBS_REQUIRED slirp)
set(HEADERS_REQUIRED slirp/libslirp.h)
set(CMAKE_REQUIRED_QUIET TRUE)
set(CMAKE_REQUIRED_LIBRARIES slirp)

check_symbol_exists("slirp_add_exec" "slirp/libslirp.h" HAS_ADD_EXEC)
check_symbol_exists("slirp_add_unix" "slirp/libslirp.h" HAS_ADD_UNIX)
check_symbol_exists("slirp_remove_guestfwd" "slirp/libslirp.h" HAS_REMOVE_GUESTFWD)

foreach(THISLIB IN LISTS LIBS_REQUIRED)
  find_library(LIB${THISLIB}_OK ${THISLIB})
  if(NOT LIB${THISLIB}_OK)
    message(FATAL_ERROR "library lib${THISLIB} not found")
  endif()
endforeach(THISLIB)

foreach(HEADER IN LISTS HEADERS_REQUIRED)
  check_include_file(${HEADER} ${HEADER}_OK)
  if(NOT ${HEADER}_OK)
    message(FATAL_ERROR "header file ${HEADER} not found")
  endif()
endforeach(HEADER)

add_definitions(-D_GNU_SOURCE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(VDEVPN_STATIC sshlirp-${CMAKE_SYSTEM_PROCESSOR})

add_executable(${VDEVPN_STATIC} sshlirp.c libvdeslirp.c libvdestream.c autoopt.c fake.c)
target_link_libraries(${VDEVPN_STATIC} slirp glib-2.0 -static)

add_executable(sshlirp sshlirp.c libvdeslirp.c libvdestream.c autoopt.c)
target_link_libraries(sshlirp slirp glib-2.0)

configure_file(config.h.in config.h)

install(TARGETS ${VDEVPN_STATIC} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS sshlirp DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory(man)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${PROJECT_SOURCE_DIR}/Uninstall.cmake")
