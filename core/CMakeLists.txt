cmake_minimum_required(VERSION 3.25.1)

set(CORE_LIB ${PROJECT_NAME}-core)

file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
set(CORE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(SQLite3)

if(DEFINED BUILD_SHARED_LIBS)
  # shared lib
  message("Building shared library")
  add_library(${CORE_LIB} SHARED ${CORE_SOURCES})
else()
  # static lib
  message("Building static library")
  add_library(${CORE_LIB} STATIC ${CORE_SOURCES})
endif()

target_include_directories(${CORE_LIB} PRIVATE ${CORE_DIRS})
target_include_directories(${CORE_LIB} SYSTEM PUBLIC ${PROJECT_ROOT}/third_party/sqlite_orm/include/)
target_link_libraries(${CORE_LIB} PUBLIC sqlite3)
set_target_properties(${CORE_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_ROOT}/pkg)
set_target_properties(${CORE_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_ROOT}/pkg)

# tests
set (CORE_TESTS ${PROJECT_NAME}-core-tests)
file(GLOB_RECURSE CORE_SOURCES_TESTS "tests/*.cpp")

add_executable(${CORE_TESTS} ${CORE_SOURCES_TESTS})
target_include_directories(${CORE_TESTS} PRIVATE ${CORE_DIRS} ../pkg/catch2/include)
target_link_libraries(${CORE_TESTS} PRIVATE Catch2::Catch2WithMain ${CORE_LIB})
target_compile_options(${CORE_TESTS} PRIVATE -ggdb)
set_target_properties(${CORE_TESTS} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT}/pkg)
