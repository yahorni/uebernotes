cmake_minimum_required(VERSION 3.25.1)

set(CORE_LIB ${PROJECT_NAME}-core)

file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
set(CORE_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CORE_PRIVATE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src)

# TODO: get rid of system lib dependency if possible
find_package(SQLite3)

if(DEFINED BUILD_SHARED_LIBS)
  # shared lib
  message(STATUS "Building shared library")
  add_library(${CORE_LIB} SHARED ${CORE_SOURCES})
else()
  # static lib
  message(STATUS "Building static library")
  add_library(${CORE_LIB} STATIC ${CORE_SOURCES})
endif()

target_include_directories(${CORE_LIB}
  PUBLIC ${CORE_PUBLIC_HEADERS}
  PRIVATE ${CORE_PRIVATE_HEADERS})

# SQLite3 ---------------------------------------------------------------------
target_include_directories(${CORE_LIB} SYSTEM PUBLIC ${PROJECT_ROOT}/extras/sqlite_orm/include/)
target_link_libraries(${CORE_LIB} PUBLIC sqlite3)
set_target_properties(${CORE_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_ROOT}/pkg)
set_target_properties(${CORE_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_ROOT}/pkg)
# -----------------------------------------------------------------------------

# tests
# TODO include only if building tests
set(CORE_TESTS ${PROJECT_NAME}-core-tests)
file(GLOB_RECURSE CORE_SOURCES_TESTS "tests/*.cpp")

add_executable(${CORE_TESTS} ${CORE_SOURCES_TESTS})
target_include_directories(${CORE_TESTS} PRIVATE ${CORE_PUBLIC_HEADERS} ${CORE_PRIVATE_HEADERS})
target_compile_options(${CORE_TESTS} PRIVATE -ggdb)
target_link_libraries(${CORE_TESTS} PRIVATE ${CORE_LIB})
set_target_properties(${CORE_TESTS} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT}/pkg)

# Catch2 ----------------------------------------------------------------------
target_include_directories(${CORE_TESTS} PRIVATE ../pkg/catch2/include)
target_link_libraries(${CORE_TESTS} PRIVATE Catch2::Catch2WithMain)
# -----------------------------------------------------------------------------
