//
// Created by skillet on 21/01/2022.
//

#include "RHP.h"

void unpackPayload(RHP_payloadStruct givenPacket) {

}

char* packPayload(char* givenPayload, RHP_payloadStruct& sendPacket) {
    sendPacket.type = 1;
    sendPacket.commID = 312;
    sendPacket.length = sizeof(*givenPayload);
    sendPacket.buffer = nullptr;
    if(sendPacket.length % 2 == 0) {
        //Set buffer value to 0
        *(sendPacket.buffer) &= 0;
    }
    sendPacket.payload = givenPayload;
    char* encodedMessage;
    *encodedMessage = sendPacket.type;
    *(encodedMessage+2) = sendPacket.commID;

}



/* function: internetChecksum_Check
 * 
 * arguments: RHP_payloadStruct givenPacket
 * 
 * This function performs an internet checksum on a
 * recieved packet. If the function returns 0, 
 * then the internet checksum passes.
*/

int internetChecksum_Check(RHP_payloadStruct givenPacket) {
    //Packet Length in bits:
    int packetLen = 8 + 16 + 16 
                    + givenPacket.length*8  
                    + (givenPacket.buffer != nullptr)*8
                    + 16;
    
    /*TODO: Iterate through each 4-byte segment of 
            the packet and add it to the running total
            If there is a carry-over 1, add 1 to the 
            running total and continue to the 
            next segment*/
    
}