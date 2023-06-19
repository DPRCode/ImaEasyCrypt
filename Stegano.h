//
// Created by janek on 17.06.23.
//

#ifndef IMAEASYCRYPT_STEGANO_H
#define IMAEASYCRYPT_STEGANO_H


#include "Pixel.h"

class Stegano {
public:
    std::string readTextFile(std::string path);
    void writeTextFile(std::string path, std::string content);
    void leastSignificantBitEncode(std::vector<std::vector<Pixel>>& image,std::string& message);
    std::string leastSignificantBitDecode(std::vector<std::vector<Pixel>>& image);
    bool checkSizes(std::vector<std::vector<Pixel>>& image, std::string& message);
    static void demo3();
private:
    void modifyLSB(uint16_t& red, bool bit);
    bool getLSB(unsigned char value);
};


#endif //IMAEASYCRYPT_STEGANO_H
