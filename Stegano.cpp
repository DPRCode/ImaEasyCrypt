//
// Created by janek on 17.06.23.
//

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "Stegano.h"
#include "Pixel.h"
#include "PNGImage.h"

std::string Stegano::readTextFile(std::string path){
    std::ifstream file;
    file.open(path);
    std::string line;
    std::string content = "";
    if(file.is_open()){
        while(getline(file, line)){
            content += line + "\n";
        }
        file.close();
    } else {
        std::cout << "Unable to open Text file" << std::endl;
    }
    return content;
}

void Stegano::writeTextFile(std::string path, std::string content){
    std::ofstream file;
    file.open(path);
    if(file.is_open()){
        file << content;
        file.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
}

void Stegano::leastSignificantBitEncode(std::vector<std::vector<Pixel>>& image,std::string& message){
    if (checkSizes(image, message)){
        int messageIndex = 0;
        unsigned char currentChar;
        int bitIndex = 0;
        for (int i = 0; i < image.size(); ++i) {
            for (int j = 0; j < image[i].size(); ++j) {
                for (int k = 0; k < 3; ++k) {
                    if (messageIndex>=message.size()){
                        currentChar = static_cast<unsigned char>(26);
                    }else {
                        currentChar = message[messageIndex];
                    }
                    //Get the bit at the current position
                    bool bit = (currentChar >> bitIndex) & 0x01;
                    //Modify the LSB of the current pixel
                    if (k == 0) {
                        modifyLSB(image[i][j].red, bit);
                    } else if (k == 1) {
                        modifyLSB(image[i][j].green, bit);
                    } else {
                        modifyLSB(image[i][j].blue, bit);
                    }
                    bitIndex++;
                    if (bitIndex == 8) {
                        //If the message is longer than the image and the eof char is set return
                        if (messageIndex >= message.size()) {
                            return;
                        }
                        bitIndex = 0;
                        messageIndex++;
                    }
                }
            }
        }

    }else {
        std::cout << "Message is too long for the image" << std::endl;
    }
}

bool Stegano::checkSizes(std::vector<std::vector<Pixel>>& image, std::string& message){
    int imageSize = image.size() * image[0].size() * 3;
    int messageSize = message.size() * 8;
    return imageSize >= messageSize;
}

bool Stegano::getLSB(unsigned char value) {
    // Das LSB wird mit einem Bitmaske-Operator (&) und 0x01 extrahiert
    return (value & 0x01);
}

std::string Stegano::leastSignificantBitDecode(std::vector<std::vector<Pixel>>& image){
    std::string message;
    unsigned char currentChar = 0;
    int bitIndex = 0;

    for (int i = 0; i < image.size(); ++i) {
        for (int j = 0; j < image[i].size(); ++j) {
            for (int k = 0; k < 3; ++k) {
                // Get the LSB of the current pixel
                bool bit;
                if (k == 0) {
                    bit = getLSB(image[i][j].red);
                } else if (k == 1) {
                    bit = getLSB(image[i][j].green);
                } else {
                    bit = getLSB(image[i][j].blue);
                }

                // Set the bit at the current position in the currentChar
                currentChar |= (bit << bitIndex);
                bitIndex++;

                if (bitIndex == 8) {
                    // Check if the currentChar represents the EOF character
                    if (currentChar == 26) {
                        return message;
                    }
                    // Add the currentChar to the message
                    message += currentChar;

                    // Reset the currentChar and bitIndex
                    currentChar = 0;
                    bitIndex = 0;
                }
            }
        }
    }
    return message;
}

void Stegano::demo3(){
    Stegano stegano;
    std::string message = stegano.readTextFile("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/message.txt");
    PNGImage image = PNGImage("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/demo.png");
    std::vector<std::vector<Pixel>> imageDat = image.getPixelData();
    stegano.leastSignificantBitEncode(imageDat, message);
    PNGImage image2 = PNGImage();
    image2.setPixelData(imageDat);
    image2.saveImage("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/image3_encoded.png");
    PNGImage image3;
    image.loadImage("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/image3_encoded.png");
    std::vector<std::vector<Pixel>> imageDat2 = image.getPixelData();
    std::string decodedMessage = stegano.leastSignificantBitDecode(imageDat2);
    stegano.writeTextFile("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/decodedMessage.txt", decodedMessage);
}
    void Stegano::modifyLSB(uint16_t& color, bool bit) {
        if (bit) {
            color |= 0x0001; // Setze das LSB auf 1
        } else {
            color &= 0xFFFE; // Setze das LSB auf 0
        }
    }

