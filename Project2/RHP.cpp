//
// Created by skillet on 21/01/2022.
//

#include "RHP.h"

void RHPstructFill(RHP_payloadStruct* rhpmsg, char* msg){
    rhpmsg->type = COMMAND;
    rhpmsg->commID = RHP_COMMID;
    rhpmsg->length = strlen(msg);
    memcpy(rhpmsg->payload, msg, strlen(msg));
    rhpmsg->checkSum = 0;
    printf("RHP: structure filled\n");
}

int RHPunpack(RHP_payloadStruct* recvPacket, uint8_t* udprecv) {
    printf("RHP: unpacking udp message\n");
    recvPacket->type = udprecv[0];
    recvPacket->commID = (udprecv[2] << 8) | udprecv[1];
    recvPacket->length = (udprecv[4] << 8) | udprecv[3];
    recvPacket->payload = (char *) (malloc(recvPacket->length));
    memcpy(recvPacket->payload, &udprecv[5], recvPacket->length);
    recvPacket->checkSum = (udprecv[recvPacket->length+6] << 8) | udprecv[recvPacket->length+5];
    uint16_t checksum = 0;
    calcChecksum(udprecv, (recvPacket->length+RHP_HEADERLEN+2), &checksum);
    printf("RHP: msg: %s\n", recvPacket->payload);
    printf("RHP: type: %d\n", recvPacket->type);
    printf("RHP: CommID: %d\n", recvPacket->commID);
    printf("RHP: length: %d\n", recvPacket->length);
    printf("RHP: chucksum received: %d\n", recvPacket->checkSum);
    printf("RHP: chucksum calculated: %d\n", checksum);
    if(~checksum == 0){
        printf("RHP: checksum valid\n");
        return 0;
    }
    else{
        printf("RHP: ERROR: checksum not valid!\n");
        return -1;
    }
}

void RHPpack(RHP_payloadStruct* sendPacket, uint8_t* udpSend) {
    uint16_t relPayLen = (sendPacket->length % 2) ? sendPacket->length : 1+sendPacket->length;
    int msgLen = relPayLen+RHP_HEADERLEN;
    udpSend = (uint8_t*) malloc(msgLen);
    uint16_t checksum = 0;

    udpSend[0] = sendPacket->type;
    udpSend[1] = sendPacket->commID & 0x00ff;
    udpSend[2] = (sendPacket->commID & 0xff00) >> 8;
    udpSend[3] = sendPacket->length & 0x00ff;
    udpSend[4] = (sendPacket->length & 0xff00) >> 8;
    memcpy(&udpSend[5], sendPacket->payload, sendPacket->length);
    calcChecksum(udpSend, msgLen, &checksum);
    sendPacket->checkSum = checksum;
    udpSend[msgLen-2] = checksum & 0x00ff;
    udpSend[msgLen-1] = (checksum & 0xff00) >> 8;

    printf("RHP: udp data packed as: ");
    for(int temp = 0; temp < msgLen; temp++){
        printf("%x ",udpSend[temp]);
    }
    printf("\n");
}

void calcChecksum(const uint8_t* data, uint16_t dataLen, uint16_t* calcsum) {
    uint32_t checksum = 0;
    for(int i = 0; i < dataLen-2; i+=2){
        checksum += (data[i+1]<<8) | data[i];
        if(checksum > 0xffff){
            checksum = (checksum & 0xffff) + ((checksum & 0xffff0000) >> 16);
        }
    }
    *calcsum = ~(checksum & 0xffff);
}