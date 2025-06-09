cmake_minimum_required(VERSION 3.22)

project(CLIENT_PC)

file(GLOB_RECURSE CLIENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_executable(client ${CLIENT_SRC} ${EXAMPLE_CONFIG_FILES})
target_link_libraries(client vsomeip3 ${VSOMEIP_NAME} ${Boost_LIBRARIES} ${DL_LIBRARY})
