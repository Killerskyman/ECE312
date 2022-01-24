//
// Created by skillet on 21/01/2022.
//

#include "RHP.h"

void RHPstructFill(RHP_payloadStruct* rhpmsg, char* msg){
    rhpmsg->type = COMMAND;
    rhpmsg->commID = RHP_COMMID;
    rhpmsg->length = strlen(msg)+1;
    rhpmsg->payload = static_cast<char *>(malloc(sizeof(msg)));
    memcpy(rhpmsg->payload, msg, strlen(msg)+1);
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
    printf("RHP: chucksum received: %x\n", recvPacket->checkSum);
    printf("RHP: chucksum calculated: %x\n", checksum);
    if(checksum == 0){
        printf("RHP: checksum valid\n");
        return 0;
    }
    else{
        printf("RHP: ERROR: checksum not valid!\n");
        return -1;
    }
}

int RHPpack(RHP_payloadStruct* sendPacket, uint8_t* udpSend) {
    uint16_t relPayLen = (sendPacket->length % 2) ? sendPacket->length : 1+sendPacket->length;
    int msgLen = relPayLen+RHP_HEADERLEN;
    uint8_t* udpSendTemp = (uint8_t*) malloc(msgLen);
    uint16_t checksum = 0;

    udpSendTemp[0] = sendPacket->type;
    udpSendTemp[1] = sendPacket->commID & 0x00ff;
    udpSendTemp[2] = (sendPacket->commID & 0xff00) >> 8;
    udpSendTemp[3] = sendPacket->length & 0x00ff;
    udpSendTemp[4] = (sendPacket->length & 0xff00) >> 8;
    memcpy(&udpSendTemp[5], sendPacket->payload, sendPacket->length);
    calcChecksum(udpSendTemp, msgLen, &checksum);
    sendPacket->checkSum = checksum;
    udpSendTemp[msgLen - 2] = checksum & 0x00ff;
    udpSendTemp[msgLen - 1] = (checksum & 0xff00) >> 8;

    memcpy(udpSend, udpSendTemp, msgLen);

    printf("RHP: udp data packed as: ");
    for(int temp = 0; temp < msgLen; temp++){
        printf("%x ", udpSendTemp[temp]);
    }
    printf("\n");
    return msgLen;
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