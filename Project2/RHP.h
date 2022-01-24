//
// Created by skillet on 21/01/2022.
//

#ifndef PROJECT2_RHP_H
#define PROJECT2_RHP_H

#endif //PROJECT2_RHP_H


/*
 *
 !!!!! This struct has a fatal flaw
 !!!!! I don't know the best way to include the 
 !!!!! buffer if it's needed and exclude when not
 * 
*/
struct RHP_payloadStruct {
    __UINT8_TYPE__ type;
    __UINT16_TYPE__ commID;
    __UINT16_TYPE__ length;
    char* payload; 
    __UINT8_TYPE__* buffer;
    __UINT16_TYPE__ checkSum; 
};

/* function: unpackPayload
 *
 * arguments: RHP_payloadStruct givenPacket
 * 
 * This function extracts the payload 
 * inside a UDP packet the program has recieved
 * 
 !!!!! I don't know what the best way to return the value is
 !!!!! Should I pass the payload by reference and store it in the function
 !!!!! or should I have it be the return value?
 !!!!! and what type will the payload be? String?
 * 
*/
void unpackPayload(RHP_payloadStruct givenPacket);

void packPayload(RHP_payloadStruct givenPayload);


/* function: internetChecksum_Check
 * 
 * arguments: RHP_payloadStruct givenPacket
 * 
 * This function performs an internet checksum on a
 * recieved packet. If the function returns 0, 
 * then the internet checksum passes.
*/
int internetChecksum_Check(RHP_payloadStruct givenPacket);