/*
 * RHP.h
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
 * authors - Skyler Cleland, Justin Benson
 * last revision: 2022-01-24
 */

#ifndef PROJECT2_RHP_H
#define PROJECT2_RHP_H

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <malloc.h>

#define RHP_COMMID 0x312    //general commID used for default messages
#define RHP_HEADERLEN 7 //1 for type, 2 for commID, 2 for length, 2 for checksum

enum RHP_TYPE{RESERVED = 0, COMMAND = 1, RHMP = 2};     //defines the different available types for the RHP protocol

//struct used to store the different variables needed for the RHP protocol
struct RHP_payloadStruct {
    uint8_t type;
    uint16_t commID;
    uint16_t length;
    char* payload;
    uint16_t checkSum;
};

/*
 * used to initialize control messages - does not calculate checksum as that should be done when packing
 *
 * rhpmsg - the struct to init
 * msg - the payload to load in
 */
extern void RHPstructFill(RHP_payloadStruct* rhpmsg, char* msg);

/*
 * unpacks a byte array into the RHP struct
 *
 * recvPacket - struct to unload into
 * udprecv - byte array to parse from
 *
 * return - returns whether the checksum is valid - 0 is valid
 */
extern int RHPunpack(RHP_payloadStruct* recvPacket, uint8_t* udprecv);

/*
 * packs a RHP struct into a byte array.
 * this will calculate the checksum for you
 *
 * sendPacket - struct to use to get data from
 * udpSend - the byte array to pack - mack sure this is pre-allocated with enough space (payload plus buffer plus header)
 *
 * return - returns the length of the message as strlen will no longer work (0x00 values before end of message)
 */
extern int RHPpack(RHP_payloadStruct* sendPacket, uint8_t* udpSend);

/*
 * calculates and validates checksums
 * calculate a checksum by sending data and actual datalength (should be even) and will output checksum to calcsum
 * validate a checksum by sending data and actual datalength plus 2, this makes it evaluate with the checksum
 *
 * data - data to calculate/validate from
 * dataLen - how much data to calculate/validate, raw lenth to calculate, add 2 to validate
 * calcsum - calculated checksum is placed here, when validating this should be 0 if valid
 */
extern void calcChecksum(const uint8_t* data, uint16_t dataLen, uint16_t* calcsum);

#endif //PROJECT2_RHP_H