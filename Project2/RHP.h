//
// Created by skillet on 21/01/2022.
//


#ifndef PROJECT2_RHP_H
#define PROJECT2_RHP_H

#include <cstdint>
#include <cstring>
#include <malloc.h>

#define RHP_COMMID 0x312
#define RHP_HEADERLEN 7 //1 for type, 2 for commID, 2 for length, 2 for checksum


enum RHP_TYPE{RESERVED = 0, COMMAND = 1, RHMP = 2};

struct RHP_payloadStruct {
    uint8_t type;
    uint16_t commID;
    uint16_t length;
    char* payload;
    uint16_t checkSum;
};

extern void RHPstructFill(RHP_payloadStruct* rhpmsg, char* msg);

extern int RHPunpack(RHP_payloadStruct* recvPacket, uint8_t* udprecv);

extern int RHPpack(RHP_payloadStruct* sendPacket, uint8_t* udpSendTemp);

extern void calcChecksum(const uint8_t* data, uint16_t dataLen, uint16_t* calcChecksum);

#endif //PROJECT2_RHP_H