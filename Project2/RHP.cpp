/*
 * RHP.cpp
 * provides functionality for creating and parsing strings to the RHP protocol
 *
 * RHPstructFill() - fills in the struct with default values (assumes control message) for the given string
 *
 * RHPunpack() - given a "string" it unpacks the string into a RHP struct and validates the checksum
 *
 * RHPpack() - given an RHP struct it packs the given string following the RHP protocol
 *
 * calcChecksum() - used for validating and generating 16-bit checksums
 *
 * NOTE: function descriptions in header file
 *
 * authors - Skyler Cleland, Justin Benson
 * last revision: 2022-01-24
 */

#include "RHP.h"

void RHPstructFill(RHP_payloadStruct* rhpmsg, char* msg){
    rhpmsg->type = COMMAND;                                             //assumes control message
    rhpmsg->commID = RHP_COMMID;                                        //default commID
    rhpmsg->length = strlen(msg)+1;                                 //sets payload length to length of the input string plus 1 for the EOL character
    rhpmsg->payload = static_cast<char *>(malloc(sizeof(msg)));     //allocate space for the payload
    memcpy(rhpmsg->payload, msg, strlen(msg)+1);        //copy message plus EOL into the payload
    rhpmsg->checkSum = 0;                                               //checksum is figured out later, set to zero for now
    printf("RHP: structure filled\n");
}

int RHPunpack(RHP_payloadStruct* recvPacket, uint8_t* udprecv) {
    printf("RHP: unpacking udp message\n");
    recvPacket->type = udprecv[0];                                                                      //set struct values based on appropriate header bytes
    recvPacket->commID = (udprecv[2] << 8) | udprecv[1];                                                //little-endian byte order
    recvPacket->length = (udprecv[4] << 8) | udprecv[3];
    recvPacket->payload = (char *) (malloc(recvPacket->length));                                    //allocate space for the payload to get copied to
    memcpy(recvPacket->payload, &udprecv[5], recvPacket->length);                           //copy payload into the space allocated
    recvPacket->checkSum = (udprecv[recvPacket->length+6] << 8) | udprecv[recvPacket->length+5];        //get the checksum (little-endian)
    uint16_t checksum = 0;                                                                              //setup temp value for checksum calculation
    calcChecksum(udprecv, (recvPacket->length+RHP_HEADERLEN+2), &checksum);     //validate checksum (plus 2 on length is for validation as this includes the checksum value)
    printf("RHP: msg: %s\n", recvPacket->payload);              //print everything out
    printf("RHP: type: %d\n", recvPacket->type);
    printf("RHP: CommID: %d\n", recvPacket->commID);
    printf("RHP: length: %d\n", recvPacket->length);
    printf("RHP: chucksum received: %x\n", recvPacket->checkSum);
    printf("RHP: chucksum calculated: %x\n", checksum);
    if(checksum == 0){                          //if the checksum is valid return 0
        printf("RHP: checksum valid\n");
        return 0;
    }
    else{                                       //if checksum not valid return -1
        printf("RHP: ERROR: checksum not valid!\n");
        return -1;
    }
}

int RHPpack(RHP_payloadStruct* sendPacket, uint8_t* udpSend) {
    uint16_t relPayLen = (sendPacket->length % 2) ? sendPacket->length : 1+sendPacket->length;  //figure out the actual payload length (this includes the buffer
    int msgLen = relPayLen+RHP_HEADERLEN;           //figure out total message length
    uint8_t* udpSendTemp = (uint8_t*) malloc(msgLen);   //allocate some temp space for setting up the message
    uint16_t checksum = 0;          //init the checksum

    udpSendTemp[0] = sendPacket->type;                          //set proper bytes based on struct
    udpSendTemp[1] = sendPacket->commID & 0x00ff;               //little-endian
    udpSendTemp[2] = (sendPacket->commID & 0xff00) >> 8;
    udpSendTemp[3] = sendPacket->length & 0x00ff;
    udpSendTemp[4] = (sendPacket->length & 0xff00) >> 8;
    memcpy(&udpSendTemp[5], sendPacket->payload, sendPacket->length);   //copy the payload into temp
    calcChecksum(udpSendTemp, msgLen, &checksum);           //figure out the checksum (using msgLen calculates a checksum)
    sendPacket->checkSum = checksum;                                                //set the checksum of the struct now that we know what it is
    udpSendTemp[msgLen - 2] = checksum & 0x00ff;                                    //set checksum in temp (little-endian)
    udpSendTemp[msgLen - 1] = (checksum & 0xff00) >> 8;                             //no need to set buffer as checksum is based on the calculated total message length with the buffer

    memcpy(udpSend, udpSendTemp, msgLen);                               //copy temp into the provided send

    printf("RHP: udp data packed as: ");                //print raw for validation
    for(int temp = 0; temp < msgLen; temp++){
        printf("%x ", udpSendTemp[temp]);
    }
    printf("\n");
    free(udpSendTemp);      //free temp space
    return msgLen;              //return the msg length so udp knows how long the message is (can't really strlen as some values are 0x00)
}

void calcChecksum(const uint8_t* data, uint16_t dataLen, uint16_t* calcsum) {
    uint32_t checksum = 0;                              //init temp checksum
    for(int i = 0; i < dataLen-2; i+=2){                //evaluate at every word except the end word
        checksum += (data[i+1]<<8) | data[i];           //add to the checksum
        if(checksum > 0xffff){                          //deal with overflow
            checksum = (checksum & 0xffff) + ((checksum & 0xffff0000) >> 16);
        }
    }
    *calcsum = ~(checksum & 0xffff);        //mask and invert the checksum then output to calcsum
}