
#include "../../IRNU.hpp"
#include "../encsocket.h"
#include "irnu_ccp.hpp"
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

using namespace CryptoPP;

void encrypt(ccp_package * pack, byte * key) {
    byte iv[AES::BLOCKSIZE];
    unsigned char tmp[16];
    int i, j;
    AES::Encryption aesEncryption(key, AES::DEFAULT_KEYLENGTH);
    CFB_Mode_ExternalCipher::Encryption cfbEncryption(aesEncryption, iv);
    for(i = 0; i < 14; ++i) {
        cfbEncryption.ProcessData(tmp, pack->blocks[i], 16);
        for(j = 0; j < 16; ++j) {
            pack->blocks[i][j] = tmp[j];
        }
    }
}

void decrypt(ccp_package * pack, byte * key) {
    byte iv[AES::BLOCKSIZE];
    int i, j;
    unsigned char tmp[16];
    CFB_Mode<AES>::Decryption cfbDecryption(key, 16, iv);
    for(i = 0; i < 14; ++i) {
        cfbDecryption.ProcessData(tmp, pack->blocks[i], 16);
        for(j = 0; j < 16; ++j) {
            pack->blocks[i][j] = tmp[j];
        }
    }
}

byte * get_checksum(unsigned char * msg) {
    CryptoPP::MD5 hash;
    byte * digest = (byte *) malloc(CryptoPP::MD5::DIGESTSIZE * sizeof(byte));
    hash.CalculateDigest(digest, msg, 208);
    return digest;
}

void handle_ccp_ack(ccp_package ccp, void * arg) {
    encsocket * enc = (encsocket *) arg;
    std::cout << "Handling ACK Package" << std::endl;
    if(ccp.protocol == PROTOCOL_CCP && ccp.package == PACKAGE_CCP_ACK) {
        ccp_ack_package * ack = (ccp_ack_package *) &ccp;
        enc->check_sent_packs[ack->ack_session][ack->ack_id] = false;

    }
}

void handle_ccp_pack(ccp_package ccp, void * arg) {
    encsocket * enc = (encsocket *) arg;
    std::cout << "Handling not ACK Package" << std::endl;
    if(ccp.protocol == PROTOCOL_CCP && ccp.package != PACKAGE_CCP_ACK) {
        ccp_ack_package * ack = new ccp_ack_package;
        ack->ack_session = ccp.session;
        ack->ack_id = ccp.id;
        ack->remote_addr = ccp.remote_addr;
        ack->package = PACKAGE_CCP_ACK;
        ack->protocol = PROTOCOL_CCP;
        std::cout << "Sending to " << inet_ntoa(ccp.remote_addr->sin_addr) << std::endl;
        enc->send_pack(*((ccp_package *)(ack)));
    }
}

void add_ccp_handlers(void * arg) {
    std::cout << "Adding CCP Handlers" << std::endl;
    encsocket * enc = (encsocket *) arg;
    enc->add_enc_handler( handle_ccp_ack);
    enc->add_enc_handler( handle_ccp_pack);
}
