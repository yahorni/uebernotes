cmake_minimum_required(VERSION 3.25.1)

set(LINUX_NAME ${PROJECT_NAME}-cli)

file(GLOB_RECURSE LINUX_SOURCES "src/*.cpp")
set(LINUX_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${LINUX_NAME} main.cpp ${LINUX_SOURCES})
target_include_directories(${LINUX_NAME} PRIVATE ${LINUX_DIRS} ../core/include/)
target_link_directories(${LINUX_NAME} PRIVATE pkg)
target_link_libraries(${LINUX_NAME} PRIVATE uebernotes-core)
install(TARGETS ${LINUX_NAME} RUNTIME DESTINATION pkg)
