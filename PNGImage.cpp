//
// Created by janek on 14.06.23.
//

#include <fstream>
#include <iostream>
#include <zlib.h>
#include "PNGImage.h"

std::vector<Chunk> PNGImage::extractChunks(const unsigned char* dataArray, long long fileSize) {
    std::vector<Chunk> chunks;
    // 8 because of the PNG signature
    long long arrayIndex = 8;
    while (arrayIndex < fileSize) {
        Chunk chunk;
        // Get length of chunk through bit shifting and or operation
        chunk.length = dataArray[arrayIndex] << 24 | dataArray[arrayIndex + 1] << 16 | dataArray[arrayIndex + 2] << 8 | dataArray[arrayIndex + 3];
        // Move forward to chunk type
        arrayIndex += 4;
        chunk.type = "";
        for (int i = 0; i < 4; ++i) {
            chunk.type += dataArray[arrayIndex + i];
        }
        // Move forward to chunk data
        arrayIndex += 4;
        // Get chunk data
        for (int i = 0; i < chunk.length; ++i) {
            chunk.data.push_back(dataArray[arrayIndex + i]);
        }
        // Move forward to chunk CRC
        arrayIndex += chunk.length;
        // Get chunk CRC
        chunk.crc = dataArray[arrayIndex] << 24 | dataArray[arrayIndex + 1] << 16 | dataArray[arrayIndex + 2] << 8 | dataArray[arrayIndex + 3];
        // Skip CRC
        arrayIndex += 4;
        chunks.push_back(chunk);
    }
    return chunks;
}

void PNGImage::loadImage(std::string path){
    std::ifstream image(path, std::ios::binary);
    if (!image) {
        std::cerr << "Error opening image" << std::endl;
    }else{
        image.seekg(0, std::ios::end);
        std::streampos fileSize = image.tellg();
        image.seekg(0, std::ios::beg);

        // Array with the size of the file
        unsigned char* dataArray = new unsigned char[fileSize];

        // Fill array with data from file
        image.read(reinterpret_cast<char*>(dataArray), fileSize);

        image.close();

        if (dataArray[0] != 137 || dataArray[1] != 80 || dataArray[2] != 78 || dataArray[3] != 71 || dataArray[4] != 13 || dataArray[5] != 10 || dataArray[6] != 26 || dataArray[7] != 10) {
            std::cerr << "Error: File is not a PNG image" << std::endl;
        } else{
            chunks = extractChunks(dataArray, fileSize);
            extractImageInformation();
        }
    }
}

void PNGImage::saveImage(std::string path){
    std::ofstream image(path, std::ios::binary);
    if (!image) {
        std::cerr << "Error opening image" << std::endl;
    }else{
        // Write PNG signature with the magic number
        unsigned char* PNGMagicNumber = new unsigned char[8];
        PNGMagicNumber[0] = 137;
        PNGMagicNumber[1] = 80;
        PNGMagicNumber[2] = 78;
        PNGMagicNumber[3] = 71;
        PNGMagicNumber[4] = 13;
        PNGMagicNumber[5] = 10;
        PNGMagicNumber[6] = 26;
        PNGMagicNumber[7] = 10;
        image.write(reinterpret_cast<char*>(PNGMagicNumber), 8);
        for(Chunk chunk:chunks){
            // Write chunk length
            unsigned char* length = new unsigned char[4];
            length[0] = (chunk.length >> 24) & 0xFF;
            length[1] = (chunk.length >> 16) & 0xFF;
            length[2] = (chunk.length >> 8) & 0xFF;
            length[3] = chunk.length & 0xFF;
            image.write(reinterpret_cast<char*>(length), 4);
            // Write chunk type
            unsigned char* type = new unsigned char[4];
            type[0] = chunk.type[0];
            type[1] = chunk.type[1];
            type[2] = chunk.type[2];
            type[3] = chunk.type[3];
            image.write(reinterpret_cast<char*>(type), 4);
            // Write chunk data
            image.write(reinterpret_cast<char*>(chunk.data.data()), chunk.length);
            // Write chunk CRC
            unsigned char* CRC = new unsigned char[4];
            CRC[0] = (chunk.crc >> 24) & 0xFF;
            CRC[1] = (chunk.crc >> 16) & 0xFF;
            CRC[2] = (chunk.crc >> 8) & 0xFF;
            CRC[3] = chunk.crc & 0xFF;
            image.write(reinterpret_cast<char*>(CRC), 4);
        }
    }
}

void PNGImage::extractImageInformation(){
    for (Chunk chunk:chunks){
        if (chunk.type=="IHDR") {
            width = chunk.data[0] << 24 | chunk.data[1] << 16 | chunk.data[2] << 8 | chunk.data[3];
            height = chunk.data[4] << 24 | chunk.data[5] << 16 | chunk.data[6] << 8 | chunk.data[7];
            bitDepth= chunk.data[8];
            colorType = chunk.data[9];
            if (colorType==0){
                colorTypeString = "Grayscale";
            }else if (colorType==2) {
                colorTypeString = "Truecolor";
            }else if (colorType==3) {
                colorTypeString = "Indexed-color";
            }else if (colorType==4) {
                colorTypeString = "Grayscale with alpha";
            }else if (colorType==6) {
                colorTypeString = "Truecolor with alpha";
            }
            compressionMethod = chunk.data[10];
            filterMethod = chunk.data[11];
            interlaceMethod = chunk.data[12];
        }
    }
}

void PNGImage::displayImageInformation(){
    std::cout << "Analyzing IHDR chunk:" << std::endl;
    std::cout << "Width: " << width << std::endl;
    std::cout << "Height: " << height << std::endl;
    std::cout << "Bit depth: " << bitDepth << std::endl;
    std::cout << "Color type: " << colorTypeString << std::endl;
    //This should always be 0
    std::cout << "Compression method: " << compressionMethod << std::endl;
    // This should always be 0
    std::cout << "Filter method: " << filterMethod << std::endl;
    std::cout << "Interlace method: " << interlaceMethod << std::endl;
}

std::vector<unsigned char> PNGImage::inflatePixelData(std::vector<unsigned char> compressedData){
    std::vector<unsigned char> decompressedData;
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;
    if (int error = inflateInit(&stream) != Z_OK) {
        std::cerr << "Error initializing zlib" << error << std::endl;
    }
    // Set the starting point and the length of the compressed data
    stream.avail_in = compressedData.size();
    stream.next_in = compressedData.data();

    while (stream.avail_in > 0) {
        unsigned char buffer[1024];
        buffer[1023] = '\0';
        stream.next_out = buffer;
        stream.avail_out = 1024;

        int error = inflate(&stream, Z_NO_FLUSH);
        if (error == Z_NEED_DICT || error == Z_DATA_ERROR || error == Z_MEM_ERROR) {
            inflateEnd(&stream);
            std::cerr << "Error inflating data: " << error << std::endl;
        } else if (error == Z_STREAM_END) {
            decompressedData.insert(decompressedData.end(), buffer, buffer + 1024 - stream.avail_out);
            break;
        } else if(error == Z_OK) {
            decompressedData.insert(decompressedData.end(), buffer, buffer + 1024 - stream.avail_out);
        } else{
            std::cerr << "Unknown Error inflating data: " << error << std::endl;
            break;
        }
    }
    return decompressedData;
}

std::vector<std::vector<Pixel>> PNGImage::extractPixelData(std::vector<unsigned char> inflatedPixelData){
    std::vector<std::vector<Pixel>> pixelData(height, std::vector<Pixel>(width));
    filterMethod = inflatedPixelData[0];
    if(colorType!=2){
        std::cerr << "Only truecolor images are supported" << std::endl;
        return pixelData;
    } else{
        int byteCounter = 1;
        int pixelCounter = 0;
        while(byteCounter<inflatedPixelData.size()){
            // Skip the filter byte
            if (byteCounter%(width*3+1)==0){
                byteCounter++;
            } else{
                if (bitDepth==8){
                    int column = pixelCounter/width;
                    int row = pixelCounter%width;
                    pixelData[column][row] = Pixel(inflatedPixelData[byteCounter],inflatedPixelData[byteCounter+1],inflatedPixelData[byteCounter+2]);
                    byteCounter+=3;
                    pixelCounter++;
                } else if (bitDepth==16){
                    pixelData[byteCounter%width][byteCounter/width] = Pixel(inflatedPixelData[byteCounter]<<8|inflatedPixelData[byteCounter+1],inflatedPixelData[byteCounter+2]<<8|inflatedPixelData[byteCounter+3],inflatedPixelData[byteCounter+4]<<8|inflatedPixelData[byteCounter+5]);
                    byteCounter+=6;
                } else {
                    std::cout << "Bit depth not supported" << std::endl;
                }
            }
        }
        if (filterMethod==1) {
            pixelData = reverseSubFilter(pixelData);
        } else {
            std::cout<< "Filter method not supported" << std::endl;
        }
        return pixelData;
    }
}

std::vector<std::vector<Pixel>> PNGImage::reverseSubFilter(std::vector<std::vector<Pixel>> unfilterdPixelData){
    for (int i = 0; i < unfilterdPixelData.size(); ++i) {
        for (int j = 0; j < unfilterdPixelData[i].size(); ++j) {
            if (j == 0) {
                unfilterdPixelData[i][j].red = unfilterdPixelData[i][j].red;
                unfilterdPixelData[i][j].green = unfilterdPixelData[i][j].green;
                unfilterdPixelData[i][j].blue = unfilterdPixelData[i][j].blue;
            } else {
                unfilterdPixelData[i][j].red = (unfilterdPixelData[i][j].red + unfilterdPixelData[i][j - 1].red)%256;
                unfilterdPixelData[i][j].green =
                        (unfilterdPixelData[i][j].green + unfilterdPixelData[i][j - 1].green)%256;
                unfilterdPixelData[i][j].blue = (unfilterdPixelData[i][j].blue + unfilterdPixelData[i][j - 1].blue)%256;
            }
        }
    }
    return unfilterdPixelData;
}

std::vector<unsigned char> PNGImage::combineCompressedData(){
    std::vector<unsigned char> compressedData;
    for (Chunk chunk:chunks){
        if (chunk.type=="IDAT") {
            for (unsigned char byte:chunk.data) {
                compressedData.push_back(byte);
            }
        }
    }
    return compressedData;
}

void PNGImage::demo(){
    PNGImage image;
    // Test image loading
    image.loadImage("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/image.png");
    // Test image information extraction
    image.displayImageInformation();
    // Test combine compressed data
    std::cout << "\nCompressed Data:" <<std::endl;
    std::vector<unsigned char> compressedData = image.combineCompressedData();
    for (unsigned char byte:compressedData){
        std::cout << (int) byte << " ";
    }
    // Test inflate
    std::vector<unsigned char> inflatedpixelData = image.inflatePixelData(image.combineCompressedData());
    std::cout << "\nPixel data size: " << inflatedpixelData.size() << std::endl;
    std::cout << "Filtered Pixel Data:" <<std::endl;
    for(unsigned char byte:inflatedpixelData){
        std::cout << (int) byte << " ";
    }
    // Test deflate
    std::cout << "\n\nRecompressed Pixel Data:" <<std::endl;
    std::vector<unsigned char> deflatedPixelData = image.deflatePixelData(inflatedpixelData);
    for(unsigned char byte:deflatedPixelData){
        std::cout << (int) byte << " ";
    }
    // Test pixel data extraction
    std::cout << "\n\nUnfiltered Pixel Data" <<std::endl;
    std::vector<std::vector<Pixel>> pixels = image.extractPixelData(inflatedpixelData);
    for (int i = 0; i < pixels.size(); ++i) {
        std::cout << "Line Nr." << i+1 << std::endl;
        for (int j = 0; j < pixels[i].size(); ++j) {
            std::cout
                    << "pixel Nr." << j+1
                    << " red:" << pixels[i][j].red
                    << " green:" << pixels[i][j].green
                    << " blue:" << pixels[i][j].blue << "\n";
        }
        std::cout << std::endl;
    }

    // Test pixel data insertion
    std::cout << "\nPixel Data Insertion" <<std::endl;
    std::vector<unsigned char> inflatedPixelData = image.insertPixelData(pixels);
    for(unsigned char byte:inflatedPixelData){
        std::cout << (int) byte << " ";
    }
    // Test CRC
    std::cout << "\n\nCRC Test for IDAT Chunks" <<std::endl;
    for (Chunk chunk:image.chunks){
        if (chunk.type=="IDAT"){
            std::cout << "CRC: " << chunk.crc << std::endl;
            chunk.calculateCRC();
            std::cout << "CRC calculated: " << chunk.crc << std::endl;
        }
    }
    // Test if image is valid after using own deflate function
    image.setPixelData(pixels);

    // Save image
    image.saveImage("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/image2.png");
}

PNGImage::PNGImage(std::string path) {
    loadImage(path);
}

std::vector<std::vector<Pixel>> PNGImage::getPixelData() {
    if (this->chunks.size()>0){
        std::vector<unsigned char> inflatedPixelData = inflatePixelData(combineCompressedData());
        return extractPixelData(inflatedPixelData);
    } else {
        std::cout << "No image loaded" << std::endl;
        return std::vector<std::vector<Pixel>>();
    }
}

std::vector<unsigned char> PNGImage::deflatePixelData(std::vector<unsigned char> decompressedData) {
    std::vector<unsigned char> compressedData;
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;
    if (int error = deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK){
        std::cerr << "Error while initializing deflate stream Error: " << error << std::endl;
    }

    stream.avail_in = decompressedData.size();
    stream.next_in = (Bytef *) decompressedData.data();
    //TODO: data could only grow to 1024 bytes --> error in compression
    while (true){ //bit hacky but stream.avail_in > 0 did not work
        unsigned char buffer[1024];
        stream.next_out = buffer;
        stream.avail_out = sizeof buffer;

        int error = deflate(&stream, Z_FINISH);
        if (error == Z_OK){
            size_t compressedSize = sizeof(buffer) - stream.avail_out;
            compressedData.insert(compressedData.end(), buffer, buffer + compressedSize);
        } else if (error == Z_STREAM_END) {
            size_t compressedSize = sizeof(buffer) - stream.avail_out;
            compressedData.insert(compressedData.end(), buffer, buffer + compressedSize);
        } else{
            std::cerr << "Error while deflating data Error:" << error << std::endl;
            break;
        }
        if (error == Z_STREAM_END){
            break;
        }
    }
    deflateEnd(&stream);
    return compressedData;
}

std::vector<unsigned char> PNGImage::insertPixelData(std::vector<std::vector<Pixel>> pixelData) {
    // TODO Use something else than the extracted filter method
    filterMethod = 1;
    if (filterMethod==1){
        pixelData = applySubFilter(pixelData);
    } else {
        std::cerr << "Filter method not supported" << std::endl;
        return std::vector<unsigned char>();
    }
    // Only Filter Method 0 is supported
    filterMethod = 0;
    std::vector<unsigned char> pixelDataVector;
    if(colorType!=2){
        std::cerr << "Only color type 2 is supported" << std::endl;
        return pixelDataVector;
    }else{
        if (bitDepth==8){
            for (std::vector<Pixel> line:pixelData){
                pixelDataVector.push_back(1);
                for (Pixel pixel:line){
                    pixelDataVector.push_back(pixel.red);
                    pixelDataVector.push_back(pixel.green);
                    pixelDataVector.push_back(pixel.blue);
                }
            }
        } else if (bitDepth==16){
            for (std::vector<Pixel> line:pixelData){
                for (Pixel pixel:line){
                    pixelDataVector.push_back(pixel.red/256);
                    pixelDataVector.push_back(pixel.red%256);
                    pixelDataVector.push_back(pixel.green/256);
                    pixelDataVector.push_back(pixel.green%256);
                    pixelDataVector.push_back(pixel.blue/256);
                    pixelDataVector.push_back(pixel.blue%256);
                }
            }
        } else {
            std::cerr << "Bit depth not supported" << std::endl;
            return std::vector<unsigned char>();
        }
    }
    return pixelDataVector;
}

std::vector<std::vector<Pixel>> PNGImage::applySubFilter(std::vector<std::vector<Pixel>> unfilterdPixelData) {
    std::vector<std::vector<Pixel>> filteredPixelData;
    for (int i = 0; i < unfilterdPixelData.size(); ++i) {
        std::vector<Pixel> filteredLine;
        for (int j = 0; j < unfilterdPixelData[i].size(); ++j) {
            Pixel pixel;
            if (j==0){
                pixel.red = unfilterdPixelData[i][j].red;
                pixel.green = unfilterdPixelData[i][j].green;
                pixel.blue = unfilterdPixelData[i][j].blue;
            } else {
                pixel.red = unfilterdPixelData[i][j].red - unfilterdPixelData[i][j-1].red;
                pixel.green = unfilterdPixelData[i][j].green - unfilterdPixelData[i][j-1].green;
                pixel.blue = unfilterdPixelData[i][j].blue - unfilterdPixelData[i][j-1].blue;
            }
            filteredLine.push_back(pixel);
        }
        filteredPixelData.push_back(filteredLine);
    }
    return filteredPixelData;
}

void PNGImage::setPixelData(std::vector<std::vector<Pixel>> pixelData) {
    if (this->chunks.size()==0){
        std::cout << "No image loaded" << std::endl;
        return;
    } else {
        std::vector<unsigned char> inflatedPixelData = insertPixelData(pixelData);
        std::vector<unsigned char> compressedPixelData = deflatePixelData(inflatedPixelData);
        std::vector<Chunk> newChunks;
        for (Chunk chunk:chunks){
            if (chunk.type=="IDAT"){
                chunk.data = compressedPixelData;
                chunk.length = compressedPixelData.size();
                chunk.calculateCRC();
            }
            if (chunk.type=="IHDR"){
                chunk=(createIHDRChunk(pixelData.size(),pixelData[0].size(),8,2,0,0,0));
            }
            newChunks.push_back(chunk);
        }
        this->chunks = newChunks;
    }
}

void PNGImage::createRandomImage(int width, int height, std::string path) {
    std::vector<std::vector<Pixel>> pixelData;
    for (int i = 0; i < height; ++i) {
        std::vector<Pixel> line;
        for (int j = 0; j < width; ++j) {
            Pixel pixel;
            // Not really random but good enough
            pixel.red = rand() % 256;
            pixel.green = rand() % 256;
            pixel.blue = rand() % 256;
            line.push_back(pixel);
        }
        pixelData.push_back(line);
    }
    PNGImage image;
    image.chunks[0] = image.createIHDRChunk(width,height,8,2,0,0,0);
    image.extractImageInformation();
    image.setPixelData(pixelData);
    image.saveImage(path);
}

PNGImage::PNGImage() {
    this->chunks = std::vector<Chunk>();
    chunks.push_back(createIHDRChunk(0,0,8,2,0,0,0));
    chunks.push_back(createIDATChunk());
    chunks.push_back(createIENDChunk());
    this->extractImageInformation();
}

Chunk PNGImage::createIHDRChunk(int width, int height, int bitDepth, int colorType, int compressionMethod, int filterMethod, int interlaceMethod) {
    Chunk chunk;
    chunk.type= "IHDR";
    std::vector<unsigned char> data(13);
    // Width
    data[0] = (width >> 24) & 0xFF;
    data[1] = (width >> 16) & 0xFF;
    data[2] = (width >> 8) & 0xFF;
    data[3] = width & 0xFF;
    // Height
    data[4] = (height >> 24) & 0xFF;
    data[5] = (height >> 16) & 0xFF;
    data[6] = (height >> 8) & 0xFF;
    data[7] = height & 0xFF;
    // Bit depth
    data[8] = static_cast<unsigned char>(bitDepth);
    // Color type
    data[9] = static_cast<unsigned char>(colorType);
    // Compression method
    data[10] = static_cast<unsigned char>(compressionMethod);
    // Filter method
    data[11] = static_cast<unsigned char>(filterMethod);
    // Interlace method
    data[12] = static_cast<unsigned char>(interlaceMethod);
    chunk.data = data;
    chunk.length = 13;
    chunk.calculateCRC();
    return chunk;
}

Chunk PNGImage::createIDATChunk() {
    Chunk chunk;
    chunk.type = "IDAT";
    chunk.data = std::vector<unsigned char>();
    chunk.length = 0;
    chunk.calculateCRC();
    return chunk;
}

Chunk PNGImage::createIENDChunk() {
    Chunk chunk;
    chunk.type = "IEND";
    chunk.data = std::vector<unsigned char>(0);
    chunk.length = 0;
    chunk.crc= 2923585666;
    return chunk;
}

void PNGImage::demo2() {
    PNGImage image;
    image.displayImageInformation();
    PNGImage::createRandomImage(2000,2000,"/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/test.png");
    PNGImage image2("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/test.png");
    image2.displayImageInformation();
}

