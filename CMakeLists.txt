cmake_minimum_required(VERSION 3.22)

project(EMBEDDED_LINUX_MAGNUM_OPUS)

find_package(Boost)

set (VSOMEIP_INCLUDE_DIRS "/home/nemesis/Playground/vsomeip/vsomeip-3.5.5/interface/vsomeip/")

include_directories(
    ${Boost_INCLUDE_DIR}
    ${VSOMEIP_INCLUDE_DIRS}
    ${VSOMEIP_INCLUDE_DIRS}../
)

link_directories(
    "/home/nemesis/Playground/vsomeip/vsomeip-3.5.5/build"
)

set(NETWORK_COMPONENTS
	CLIENT_PC
	SERVICE_PI
)

foreach(network_component ${NETWORK_COMPONENTS})
	add_subdirectory(
	    "${CMAKE_CURRENT_SOURCE_DIR}/${network_component}/"
	    "${CMAKE_BINARY_DIR}/${network_component}/"
	)
endforeach()

