//
// Created by skillet on 21/01/2022.
//

#include <cstdio>
#include "RHMP.h"

#define RHMPHEADERLEN 4

void RHMP_messageFill(RHMP_TYPE msgType, RHMP_payloadStruct* rhmpMsg, char* msg) {
    rhmpMsg->type = msgType;
    rhmpMsg->srcPort = SRCPORT;
    rhmpMsg->dstPort = DESTPORT;
    rhmpMsg->uid = DEVICE_UID;

    if(msgType == MESSAGE_RESPONSE) {

        // Set payload length to length of the input string plus 1 for the EOL character
        rhmpMsg->length = strlen(msg) + 1;

        // Allocate space for the payload
        rhmpMsg->payload = static_cast<uint8_t *>(malloc(rhmpMsg->length));

        // Copy message plus EOL into the payload
        memcpy(rhmpMsg->payload, msg, rhmpMsg->length);
    }

    printf("RHMP: Structure Filled.\n");
}


int RHMP_unpack(RHMP_payloadStruct* packetToRecv, uint8_t* rhpRecv) {
    printf("RHMP: Unpacking RHP message.\n");

    // Unpack entire header
    uint32_t rhmpHeader = rhpRecv[0]<<24 | rhpRecv[1] << 16 | rhpRecv[2] << 8 | rhpRecv[3];

    // Shift bits back and mask to set value of type
    packetToRecv->type = static_cast<RHMP_TYPE>((rhmpHeader>>28) & (0x0F));

    // Shift bits back and mask to set values of dstPort and srcPort
    packetToRecv->dstPort = (((rhmpHeader >> 20) & (0x0FF)) | ((rhmpHeader>>6) & (0x03F00)));
    packetToRecv->srcPort = (((rhmpHeader & 0x03F) << 8) | ((rhmpHeader >> 6) & (0x0FF)));

    // Since each packet has a different type, the prorgam must handle the 
    // cases for each message type differently
    switch(packetToRecv->type) {

        // MESSAGE_REQUEST: Store source port and destination port 
        //  since there is no payload we don't do anything with the 
        //  length nor the payload 
        case MESSAGE_REQUEST:
            printf("RHMP: type: MESSAGE_REQUEST\n");
            printf("RHMP: source port: %d\n", packetToRecv->srcPort);
            printf("RHMP: dest port: %d\n", packetToRecv->dstPort);
            return 1;

        // MESSAGE_RESPONSE: This is the only type to have a variable
        //  payload length. The message unpacks the length and then 
        //  uses that length value to allocate memory space for the 
        //  payload and then unpacks the payload into this allocated space.
        case MESSAGE_RESPONSE:
            packetToRecv->length = rhpRecv[4];
            packetToRecv->payload = static_cast<uint8_t *>(malloc(packetToRecv->length));
            memcpy(packetToRecv->payload, &(rhpRecv[5]), packetToRecv->length);
            printf("RHMP: type: MESSAGE_RESPONSE\n");
            printf("RHMP: source port: %d\n", packetToRecv->srcPort);
            printf("RHMP: dest port: %d\n", packetToRecv->dstPort);
            printf("RHMP: length: %d\n", packetToRecv->length);
            printf("RHMP: msg: %s\n", packetToRecv->payload);
            break;

        // ID_REQUEST: This type also does not have a payload so 
        //  don't try to unpack a length or a payload. 
        case ID_REQUEST:
            printf("RHMP: type: ID_REQUEST\n");
            printf("RHMP: source port: %d\n", packetToRecv->srcPort);
            printf("RHMP: dest port: %d\n", packetToRecv->dstPort);
            return 1;

        // ID_RESPONSE: This type has a fixed payload and because of 
        //  that the length byte is useless and as such is just written to zero.
        case ID_RESPONSE:
            packetToRecv->length = 0;
            packetToRecv->uid = ((rhpRecv[4]) | (rhpRecv[5] << 8) | (rhpRecv[6] << 16) | (rhpRecv[7] << 24));
            printf("RHMP: type: ID_RESPONSE\n");
            printf("RHMP: source port: %d\n", packetToRecv->srcPort);
            printf("RHMP: dest port: %d\n", packetToRecv->dstPort);
            printf("RHMP: id: 0x%x\n", packetToRecv->uid);
            printf("RHMP: id: %d\n", packetToRecv->uid);
            break;
        default:
            break;
    }
    return 0;
}

int RHMP_pack(RHMP_payloadStruct* structToSend, uint8_t* rhpSend) {
    uint32_t rhmpHeader = 0;

    // Shift bits over and apply masks to properly pack 
    //  the type, dstPort, and srcPort into the header
    rhmpHeader |= (structToSend->type << 28);
    rhmpHeader |= (structToSend->dstPort & 0x0FF) << 20;
    rhmpHeader |= (structToSend->dstPort & 0x03F00) << 12;
    rhmpHeader |= (structToSend->srcPort & 0xFF) << 6;
    rhmpHeader |= (structToSend->srcPort & 0x3F00) >> 8;
    
    // Pack each value of the header into its respective place in 
    //  the byte sequence.
    rhpSend[0] = (rhmpHeader>>24) & 0x0FF;
    rhpSend[1] = (rhmpHeader>>16) & 0x0FF;
    rhpSend[2] = (rhmpHeader>>8) & 0x0FF;
    rhpSend[3] = (rhmpHeader) & 0xFF;

    // Determine how to handle the payload for each of the given types. 
    switch (structToSend->type) {

        // Since there is no payload, the whole packet size is just 
        //  going to be the header 
        case MESSAGE_REQUEST:
            return RHMPHEADERLEN;

        // ID_REQUEST: No Payload
        case ID_REQUEST:
            return RHMPHEADERLEN + 4;

        // MESSAGE_RESPONSE: Variable Payload. Load the value of the length 
        //  and then use that value to allocate memory for the payload. 
        //  Then load the payload. 
        case MESSAGE_RESPONSE:
            rhpSend[4] = structToSend->length;
            memcpy(&(rhpSend[5]), structToSend->payload, structToSend->length);
            return RHMPHEADERLEN + 1 + structToSend->length;
        
        // ID_RESPONSE: Fixed 32-bit payload. 
        //  Pack the UID value onto the back of the header. 
        case ID_RESPONSE:
            rhpSend[4] = (structToSend->uid) & 0x0FF;
            rhpSend[5] = (structToSend->uid >> 8) & 0xFF;
            rhpSend[6] = (structToSend->uid >> 16) & 0xFF;
            rhpSend[7] = (structToSend->uid >> 24) & 0xFF;
            return RHMPHEADERLEN + 4;
        default:
            return 0;
    }
}