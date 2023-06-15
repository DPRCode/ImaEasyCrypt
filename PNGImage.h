//
// Created by janek on 14.06.23.
//

#ifndef IMAEASYCRYPT_PNGIMAGE_H
#define IMAEASYCRYPT_PNGIMAGE_H


#include <vector>
#include <string>
#include "Chunk.h"
#include "Pixel.h"

class PNGImage {
private:
    std::vector<Chunk> chunks;
    int width;
    int height;
    int colorType;
    std::string colorTypeString;
    int bitDepth;
    int compressionMethod;
    int filterMethod;
    int interlaceMethod;

    std::vector<Chunk> extractChunks(const unsigned char* dataArray, long long fileSize);
    void extractImageInformation();
    std::vector<unsigned char> inflatePixelData(std::vector<unsigned char> compressedData);
    std::vector<std::vector<Pixel>> extractPixelData(std::vector<unsigned char> inflatedPixelData);
    std::vector<std::vector<Pixel>> subFilter(std::vector<std::vector<Pixel>> unfilterdPixelData);
    std::vector<unsigned char> combineCompressedData();

public:
    PNGImage() = default;
    PNGImage(std::string path);
    void loadImage(std::string path);
    void saveImage(std::string path);
    void displayImageInformation();
    std::vector<std::vector<Pixel>> getPixelData();
    static void demo();
};


#endif //IMAEASYCRYPT_PNGIMAGE_H
