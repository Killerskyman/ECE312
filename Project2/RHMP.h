//
// Created by skillet on 21/01/2022.
//

#ifndef PROJECT2_RHMP_H
#define PROJECT2_RHMP_H

#include <cstdint>

struct RHMP_payloadStruct {
    int type : 4;   //4-bit-wide field
    int dstport : 14;
    int srcPort : 14;
    uint8_t* length;
    uint8_t * payload;
};

#endif //PROJECT2_RHMP_H