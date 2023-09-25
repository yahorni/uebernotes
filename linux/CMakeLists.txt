cmake_minimum_required(VERSION 3.25.1)

set(LINUX_CLIENT ${PROJECT_NAME}-cli)

file(GLOB_RECURSE LINUX_SOURCES "src/*.cpp")
set(LINUX_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${LINUX_CLIENT} main.cpp ${LINUX_SOURCES})
target_include_directories(${LINUX_CLIENT} PRIVATE ${LINUX_DIRS})
target_link_directories(${LINUX_CLIENT} PRIVATE ${PROJECT_ROOT}/pkg)
set_target_properties(${LINUX_CLIENT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT}/pkg)

# uebernotes-core -------------------------------------------------------------
# TODO: change core/include to ${CORE_DIRS}
target_include_directories(${LINUX_CLIENT} PRIVATE ${PROJECT_ROOT}/core/include)
# TODO: change to ${CORE_LIB}
target_link_libraries(${LINUX_CLIENT} PRIVATE uebernotes-core)
# TODO: maybe change dir to pkg/core
target_link_directories(${LINUX_CLIENT} PRIVATE ${PROJECT_ROOT}/pkg)
# -----------------------------------------------------------------------------

# easyloggingpp ---------------------------------------------------------------
add_library(elpp STATIC IMPORTED)
set_property(TARGET elpp PROPERTY
  IMPORTED_LOCATION ${PROJECT_ROOT}/pkg/elpp/lib/libeasyloggingpp.a)

target_include_directories(
  ${LINUX_CLIENT} SYSTEM PUBLIC ${PROJECT_ROOT}/pkg/elpp/include)
target_link_libraries(${LINUX_CLIENT} PUBLIC elpp)
# -----------------------------------------------------------------------------

# cxxopts ---------------------------------------------------------------------
# TODO: make variable for cxxopts
target_include_directories(${LINUX_CLIENT} PRIVATE ${PROJECT_ROOT}/third_party/cxxopts/include)
# -----------------------------------------------------------------------------

# FTXUI -----------------------------------------------------------------------
target_include_directories(${LINUX_CLIENT} PRIVATE ${PROJECT_ROOT}/pkg/ftxui/include)
target_link_libraries(${LINUX_CLIENT} PRIVATE ftxui::screen ftxui::dom ftxui::component)
# -----------------------------------------------------------------------------

# tests
# TODO include only if building tests
set (LINUX_TESTS ${PROJECT_NAME}-cli-tests)
file(GLOB_RECURSE LINUX_SOURCES_TESTS "tests/*.cpp")

add_executable(${LINUX_TESTS} ${LINUX_SOURCES_TESTS} ${LINUX_SOURCES})
target_include_directories(${LINUX_TESTS} PRIVATE ${LINUX_DIRS})
target_compile_options(${LINUX_TESTS} PRIVATE -ggdb)
set_target_properties(${LINUX_TESTS} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT}/pkg)

# tests deps -- TODO: redo includes -------------------------------------------
# core
target_include_directories(${LINUX_TESTS} PRIVATE ${PROJECT_ROOT}/core/include)
target_link_libraries(${LINUX_TESTS} PRIVATE uebernotes-core)
# cxxopts
target_include_directories(${LINUX_TESTS} PRIVATE ${PROJECT_ROOT}/third_party/cxxopts/include)
# Catch2
target_include_directories(${LINUX_TESTS} PRIVATE ../pkg/catch2/include)
target_link_libraries(${LINUX_TESTS} PRIVATE Catch2::Catch2WithMain)
# FTXUI
target_include_directories(${LINUX_TESTS} PRIVATE ${PROJECT_ROOT}/pkg/ftxui/include)
target_link_libraries(${LINUX_TESTS} PRIVATE ftxui::screen ftxui::dom ftxui::component)
# easyloggingpp
target_include_directories(${LINUX_TESTS} PRIVATE ${PROJECT_ROOT}/pkg/elpp/include)
target_link_libraries(${LINUX_TESTS} PRIVATE elpp)
# -----------------------------------------------------------------------------
