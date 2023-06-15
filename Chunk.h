//
// Created by janek on 14.06.23.
//

#ifndef IMAEASYCRYPT_CHUNK_H
#define IMAEASYCRYPT_CHUNK_H


#include <string>
#include <vector>

class Chunk {
public:
    std::string type;
    std::vector<unsigned char> data;
    unsigned int length;
    unsigned int crc;

    Chunk() = default;
    void calculateCRC();
};


#endif //IMAEASYCRYPT_CHUNK_H
