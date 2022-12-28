cmake_minimum_required(VERSION 3.25.1)

set(LIB_NAME ${PROJECT_NAME}-core)

file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
set(CORE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/core)

find_package(SQLite3)

if(DEFINED BUILD_SHARED_LIBS)
  # shared lib
  add_library(${LIB_NAME} SHARED ${CORE_SOURCES})
else()
  # static lib
  add_library(${LIB_NAME} STATIC ${CORE_SOURCES})
endif()

target_include_directories(${LIB_NAME} PRIVATE ${CORE_DIRS})
target_include_directories(${LIB_NAME} SYSTEM PUBLIC ../third_party/sqlite_orm/include/)
target_link_libraries(${LIB_NAME} PRIVATE sqlite3)

install(TARGETS ${LIB_NAME}
        LIBRARY DESTINATION pkg
        ARCHIVE DESTINATION pkg)
