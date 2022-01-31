//
// Created by skillet on 21/01/2022.
//

#include <cstdio>
#include "RHMP.h"

#define RHMP_HEADERLEN 3

extern void RHMP_messageFill(RHMP_payloadStruct* rhmpMsg, char* msg) {
    if(rhmpMsg->type != MESSAGE_RESPONSE) {
        //throw error
        printf("RHMP: ERROR: Attempting to attach payload to non-message type.\n");
    }

    // Set payload length to length of the input string plus 1 for the EOL character
//    rhmpMsg->length = strlen(msg)+1;
//
//    // Allocate space for the payload
//    rhmpMsg->payload = static_cast<char *>(malloc(sizeof(msg));

    // Copy message plus EOL into the payload
    memcpy(rhmpMsg->payload, msg, strlen(msg)+1);

    printf("RHMP: Structure Filled.\n");
}


extern int RHMP_unpack(RHMP_payloadStruct* packetToRecv, uint8_t* rhpRecv) {
    printf("RHMP: Unpacking RHP message.\n");

    //!!!!!How does enum type store bits?
    packetToRecv->type = static_cast<RHMP_TYPE>(rhpRecv[0] & (0b11110000) >> 4);

    packetToRecv->dstPort = (((rhpRecv[0] & 0b00001111) << 16) 
                            | ((rhpRecv[1] & 0b11111111 ) << 8)
                            | ((rhpRecv[2] & 0b11000000))) >> 6;

    packetToRecv->srcPort = ((rhpRecv[2] & 0b00111111) << 8) | (rhpRecv[1] & 0b11111111);

    switch(packetToRecv->type) {

        case MESSAGE_REQUEST:
            break;
        case MESSAGE_RESPONSE:
            break;
        case ID_REQUEST:
            break;
        case ID_RESPONSE:
            break;
    }
}



extern int RHMP_pack(RHMP_payloadStruct* structToSend, uint8_t* rhpSend) {
    //figure out the actual payload length
    uint16_t relPayLen = 0;
    if (structToSend->type == 2) {
        relPayLen = static_cast<uint16_t>(strlen((char *) (structToSend->payload)));
    } else if (structToSend->type == 4) {
        relPayLen = 4;
    } 

    //figure out total message length
    int msgLen = relPayLen+RHMP_HEADERLEN;

    //allocate some temp space for setting up the message


    //copy the payload into temp
    //copy temp into the provided send
    //free temp space
    //return the msg length so udp knows how long the message is (can't really strlen as some values are 0x00)
}