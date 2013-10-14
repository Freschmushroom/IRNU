
#add_library(libcryptopp STATIC IMPORTED)
#set_property(TARGET libcryptopp PROPERTY IMPORTED LOCATION /home/felix/projects/IRNU.git/src/crypt/libcryptopp.a)

add_executable(crypt_test crypt_test.cpp ../ccp/irnu_ccp_util.cpp ../encsocket.cpp ../../socket/udpsocket.cpp ../../IRNU.cpp ../../rcp/irnu_rcp_req.cpp ../../rcp/irnu_rcp_util.cpp)

target_link_libraries(crypt_test cryptopp pthread)

