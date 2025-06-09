cmake_minimum_required(VERSION 3.22)

project(SERVICE_PI)

file(GLOB_RECURSE SERVICE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SERVICE_SRC} ${EXAMPLE_CONFIG_FILES})

target_link_libraries(${PROJECT_NAME} vsomeip3 ${VSOMEIP_NAME} ${Boost_LIBRARIES} ${DL_LIBRARY})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
