
#add_library(libcryptopp STATIC IMPORTED)
#set_property(TARGET libcryptopp PROPERTY IMPORTED LOCATION /home/felix/projects/IRNU.git/src/crypt/libcryptopp.a)

add_executable(crypt_test crypt_test.cpp )

target_link_libraries(crypt_test cryptopp pthread)

