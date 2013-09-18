
include_directories(${IRNU_SOURCE_DIR})
link_directories(${IRNU_BUILD_DIR})

add_executable(IRNU socket/udpsocket.cpp IRNU.cpp)

target_link_libraries( IRNU ${CMAKE_THREAD_LIBS_INIT} )

add_subdirectory(socket)