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

# cxxopts ---------------------------------------------------------------------
# TODO: make variable for cxxopts
target_include_directories(${LINUX_CLIENT} PRIVATE ${PROJECT_ROOT}/third_party/cxxopts/include)
# -----------------------------------------------------------------------------

# FTXUI -----------------------------------------------------------------------
target_include_directories(${LINUX_CLIENT} PRIVATE ${PROJECT_ROOT}/pkg/ftxui/include)
target_link_libraries(${LINUX_CLIENT} PRIVATE ftxui::screen ftxui::dom ftxui::component)
# -----------------------------------------------------------------------------
