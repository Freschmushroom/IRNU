
add_executable(server server.cpp ../socket/udpsocket.cpp ../rcp/irnu_rcp_req.cpp ../rcp/irnu_rcp_util.cpp ../IRNU.cpp )

target_link_libraries( server ${CMAKE_THREAD_LIBS_INIT} )

add_executable(client client.cpp ../socket/udpsocket.cpp ../rcp/irnu_rcp_req.cpp ../rcp/irnu_rcp_util.cpp ../IRNU.cpp )

target_link_libraries( client ${CMAKE_THREAD_LIBS_INIT} )