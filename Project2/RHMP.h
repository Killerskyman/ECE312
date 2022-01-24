//
// Created by skillet on 21/01/2022.
//

#ifndef PROJECT2_RHMP_H
#define PROJECT2_RHMP_H

#endif //PROJECT2_RHMP_H

struct RHMP_payloadStruct {
    int type : 4;   //4-bit-wide field
    int dstport : 14;
    int srcPort : 14;
    __UINT8_TYPE__* length;
    __UINT8_TYPE__* payload;
}