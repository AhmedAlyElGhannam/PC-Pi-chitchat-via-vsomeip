cmake_minimum_required(VERSION 3.22)

project(SERVICE_PI)

file(GLOB_RECURSE SERVICE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_executable(service ${SERVICE_SRC} ${EXAMPLE_CONFIG_FILES})

target_link_libraries(service vsomeip3 ${VSOMEIP_NAME} ${Boost_LIBRARIES} ${DL_LIBRARY})


