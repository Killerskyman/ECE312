//
// Created by skillet on 21/01/2022.
//

#ifndef PROJECT2_RHMP_H
#define PROJECT2_RHMP_H

#include <cstdint>
#include <cstring>
#include <malloc.h>

enum RHMP_TYPE{RESERVED = 0, MESSAGE_REQUEST = 1, MESSAGE_RESPONSE = 2,
                ID_REQUEST = 3, ID_RESPONSE = 4}; 

//!!!!! How do we determine the source and destination port? Don't those come from the level below us?

struct RHMP_payloadStruct {
    RHMP_TYPE type;   //4-bit-wide field
    int dstPort;
    int srcPort;
    uint8_t* length;
    uint8_t * payload;
};

/*
 * used to initialize RHMP message packets - will throw errors if struct is of type other than MESSAGE_RESPONSE
 *
 * rhmpMsg - the struct to init
 * msg - the payload to load in
 */
extern void RHMP_messageFill(RHMP_payloadStruct* rhmpMsg, char* msg);


/*
 * unpacks a byte array payload into the RHMP struct
 *
 * packetToRecv - struct to unload into
 * rhpRecv - byte array to parse from
 *
 * return - returns whether the checksum is valid - 0 is valid
 */
extern int RHMP_unpack(RHMP_payloadStruct* packetToRecv, uint8_t* rhpRecv);



/*
 * packs a RHMP struct into a byte array.
 * this will calculate the checksum for you
 *
 * structToSend - struct to use to get data from
 * rhpSend - the byte array to pack 
 *
 * return - returns the length of the message as strlen will no longer work (0x00 values before end of message)
 */
extern int RHMP_pack(RHMP_payloadStruct* structToSend, uint8_t* rhpSend);

#endif //PROJECT2_RHMP_H