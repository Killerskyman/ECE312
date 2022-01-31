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

    // Set payload length to length of the input string plus 1 for the EOL character
    rhpmsg->length = strlen(msg)+1; 

    // Allocate space for the payload
    rhpmsg->payload = static_cast<char *>(malloc(sizeof(msg)));     

    // Copy message plus EOL into the payload
    memcpy(rhpmsg->payload, msg, strlen(msg)+1);        
    rhpmsg->checkSum = 0;                               //checksum is figured out later, set to zero for now
    printf("RHP: structure filled\n");
}

int RHPunpack(RHP_payloadStruct* recvPacket, uint8_t* udprecv) {
    printf("RHP: unpacking udp message\n");

    // Set struct values based on appropriate header bytes
    recvPacket->type = udprecv[0];                                                           
    recvPacket->commID = (udprecv[2] << 8) | udprecv[1];    //little-endian byte order
    recvPacket->length = (udprecv[4] << 8) | udprecv[3];

    // Allocate space for the payload to get copied to
    recvPacket->payload = (char *) (malloc(recvPacket->length));      

    // Copy payload into the space allocated                       
    memcpy(recvPacket->payload, &udprecv[5], recvPacket->length);     

    // Get the checksum (little-endian)                      
    recvPacket->checkSum = (udprecv[recvPacket->length+6] << 8) | udprecv[recvPacket->length+5];   

    // Setup temp value for checksum calculation     
    uint16_t checksum = 0;                    

    // Validate checksum (plus 2 on length is for validation as this includes the checksum value)                                                         
    calcChecksum(udprecv, (recvPacket->length+RHP_HEADERLEN+2), &checksum);     

    // Print everything out
    printf("RHP: msg: %s\n", recvPacket->payload);              
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

    //figure out the actual payload length (this includes the buffer
    uint16_t relPayLen = (sendPacket->length % 2) ? sendPacket->length : 1+sendPacket->length;  

    //figure out total message length
    int msgLen = relPayLen+RHP_HEADERLEN;      

    //allocate some temp space for setting up the message
    uint8_t* udpSendTemp = (uint8_t*) malloc(msgLen);  

    //init the checksum
    uint16_t checksum = 0;          

    //set proper bytes based on struct
    udpSendTemp[0] = sendPacket->type;                          
    udpSendTemp[1] = sendPacket->commID & 0x00ff;               //little-endian
    udpSendTemp[2] = (sendPacket->commID & 0xff00) >> 8;
    udpSendTemp[3] = sendPacket->length & 0x00ff;
    udpSendTemp[4] = (sendPacket->length & 0xff00) >> 8;

    //copy the payload into temp
    memcpy(&udpSendTemp[5], sendPacket->payload, sendPacket->length);   

    //figure out the checksum (using msgLen calculates a checksum)
    calcChecksum(udpSendTemp, msgLen, &checksum); 

    //set the checksum of the struct now that we know what it is          
    sendPacket->checkSum = checksum;           

    //set checksum in temp (little-endian)                                     
    udpSendTemp[msgLen - 2] = checksum & 0x00ff;            //no need to set buffer as checksum is based on the 
    udpSendTemp[msgLen - 1] = (checksum & 0xff00) >> 8;     //calculated total message length with the buffer

    //copy temp into the provided send
    memcpy(udpSend, udpSendTemp, msgLen);                               

    //print raw for validation
    printf("RHP: udp data packed as: ");                
    for(int temp = 0; temp < msgLen; temp++){
        printf("%x ", udpSendTemp[temp]);
    }
    printf("\n");

    //free temp space
    free(udpSendTemp);   

    //return the msg length so udp knows how long the message is (can't really strlen as some values are 0x00)   
    return msgLen;              
}

void calcChecksum(const uint8_t* data, uint16_t dataLen, uint16_t* calcsum) {
    //init temp checksum
    uint32_t checksum = 0;

    //evaluate at every word except the end word
    for(int i = 0; i < dataLen-2; i+=2){    

        //add to the checksum            
        checksum += (data[i+1]<<8) | data[i];  

        //deal with overflow         
        if(checksum > 0xffff){                          
            checksum = (checksum & 0xffff) + ((checksum & 0xffff0000) >> 16);
        }
    }

    //mask and invert the checksum then output to calcsum
    *calcsum = ~(checksum & 0xffff);        
}