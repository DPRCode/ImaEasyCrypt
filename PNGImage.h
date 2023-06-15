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
    std::vector<unsigned char> deflatePixelData(std::vector<unsigned char> decompressedData);
    std::vector<std::vector<Pixel>> extractPixelData(std::vector<unsigned char> inflatedPixelData);
    std::vector<unsigned char> insertPixelData(std::vector<std::vector<Pixel>> pixelData);
    std::vector<std::vector<Pixel>> reverseSubFilter(std::vector<std::vector<Pixel>> unfilterdPixelData);
    std::vector<std::vector<Pixel>> applySubFilter(std::vector<std::vector<Pixel>> unfilterdPixelData);
    std::vector<unsigned char> combineCompressedData();
    Chunk createIHDRChunk(int width, int height, int bitDepth, int colorType, int compressionMethod, int filterMethod, int interlaceMethod);
    Chunk createIDATChunk();
    Chunk createIENDChunk();

public:
    PNGImage();
    PNGImage(std::string path);
    void loadImage(std::string path);
    void saveImage(std::string path);
    void displayImageInformation();
    std::vector<std::vector<Pixel>> getPixelData();
    void setPixelData(std::vector<std::vector<Pixel>> pixelData);
    static void createRandomImage(int width, int height,std::string path);
    static void demo();
    static void demo2();
};


#endif //IMAEASYCRYPT_PNGIMAGE_H
