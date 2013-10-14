
include_directories(${IRNU_SOURCE_DIR})
link_directories(${IRNU_BUILD_DIR})

#add_executable(IRNU socket/udpsocket.cpp rcp/irnu_rcp_util.cpp rcp/irnu_rcp_req.cpp IRNU.cpp)

#target_link_libraries( IRNU ${CMAKE_THREAD_LIBS_INIT} )

add_subdirectory(socket)
add_subdirectory(rcp)
add_subdirectory(test)
<<<<<<< HEAD
add_subdirectory(enc)
=======
add_subdirectory(crypt)
>>>>>>> ff9e8d2f6ddbee85e20a7ea1b1d9805957750056
