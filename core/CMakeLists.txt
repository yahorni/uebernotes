cmake_minimum_required(VERSION 3.25.1)

set(LIB_NAME ${PROJECT_NAME}-core)

file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
set(CORE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/core)

if(DEFINED BUILD_SHARED_LIBS)
  # shared lib
  add_library(${LIB_NAME} SHARED ${CORE_SOURCES})
  target_include_directories(${LIB_NAME} PRIVATE ${CORE_DIRS})
  install(TARGETS ${LIB_NAME} LIBRARY DESTINATION pkg)

else()
  # static lib
  add_library(${LIB_NAME} STATIC ${CORE_SOURCES})
  target_include_directories(${LIB_NAME} PRIVATE ${CORE_DIRS})
  install(TARGETS ${LIB_NAME} ARCHIVE DESTINATION pkg)

endif()
