//
// Created by janek on 14.06.23.
//

#include "Chunk.h"
#include <zlib.h>
#include <cstring>

void Chunk::calculateCRC(){
    unsigned char dataArray[type.length()];
    // Convert string to unsigned char array
    std::memcpy(dataArray, type.c_str(), type.length());
    // Calculate CRC
    crc = crc32(0L, dataArray, 4);
    crc = crc32(crc, data.data(), data.size());
}


