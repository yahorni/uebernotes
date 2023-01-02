cmake_minimum_required(VERSION 3.25.1)
project(uebernotes CXX)

# TODO: move build type to Makefile
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Catch2 ----------------------------------------------------------------------
# TODO include only if building tests
add_library(Catch2::Catch2 STATIC IMPORTED)
set_property(TARGET Catch2::Catch2 PROPERTY
  IMPORTED_LOCATION ${PROJECT_ROOT}/pkg/catch2/lib/libCatch2.a)

add_library(Catch2::Catch2WithMain STATIC IMPORTED)
set_property(TARGET Catch2::Catch2WithMain PROPERTY
  IMPORTED_LOCATION ${PROJECT_ROOT}/pkg/catch2/lib/libCatch2Main.a)
target_link_libraries(Catch2::Catch2WithMain INTERFACE Catch2::Catch2)
# -----------------------------------------------------------------------------

add_subdirectory(core)

if(NOT DEFINED BUILD_SHARED_LIBS)
  # FTXUI --------------------------------------------------------------------
  add_library(ftxui::dom STATIC IMPORTED)
  set_property(TARGET ftxui::dom PROPERTY
    IMPORTED_LOCATION ${PROJECT_ROOT}/pkg/ftxui/lib/libftxui-dom.a)

  add_library(ftxui::screen STATIC IMPORTED)
  set_property(TARGET ftxui::screen PROPERTY
    IMPORTED_LOCATION ${PROJECT_ROOT}/pkg/ftxui/lib/libftxui-screen.a)

  add_library(ftxui::component STATIC IMPORTED)
  set_property(TARGET ftxui::component PROPERTY
    IMPORTED_LOCATION ${PROJECT_ROOT}/pkg/ftxui/lib/libftxui-component.a)
  # -----------------------------------------------------------------------------

  add_subdirectory(linux)
endif()
