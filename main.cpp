#include "PNGImage.h"
#include <iostream>
#include "Stegano.h"

void usage(){
    std::cout << "Usage: " << "ImaEasyCrypt" << " <inputPathText> <inputPathImage> <outputPath> -m <mode>" << std::endl;
    std::cout << "Modes: encode, decode, generate" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string mode;
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg=="-m"){
            mode = argv[i+1];
            i++;
        }
    }
    if (mode.empty()){
        std::cout << "No mode specified" << std::endl;
        usage();
        return 1;
    }
    if (mode == "encode") {
        if (argc < 5){
            std::cout << "Usage encode: " << "ImaEasyCrypt" << " <inputPathText> <inputPathImage> <outputPath> -m encode" << std::endl;
            return 1;
        } else{
            std::string textPath = argv[1];
            std::string imagePath = argv[2];
            std::string outputPath = argv[3];
            Stegano stegano;
            std::cout << "Reading text file..." << std::endl;
            std::string message = stegano.readTextFile(textPath);
            std::cout << "Reading image..." << std::endl;
            PNGImage image(imagePath);
            image.displayImageInformation();
            std::cout << "Encoding message..." << std::endl;
            std::vector<std::vector<Pixel>> pixelsDat = image.getPixelData();
            stegano.leastSignificantBitEncode(pixelsDat, message);
            image.setPixelData(pixelsDat);
            image.saveImage(outputPath);
            std::cout << "Image saved to " << outputPath << std::endl;
        }
    } else if (mode == "decode") {
        if (argc < 4){
            std::cout << "Usage decode: " << "ImaEasyCrypt" << " <inputPathImage> <outputPath> -m decode" << std::endl;
            return 1;
        } else{
            std::string imagePath = argv[1];
            std::string outputPath = argv[2];
            Stegano stegano;
            std::cout << "Reading image..." << std::endl;
            PNGImage image(imagePath);
            image.displayImageInformation();
            std::cout << "Decoding message..." << std::endl;
            std::vector<std::vector<Pixel>> pixelsDat = image.getPixelData();
            std::string message = stegano.leastSignificantBitDecode(pixelsDat);
            std::cout << "Writing message to file..." << std::endl;
            stegano.writeTextFile(outputPath, message);
            std::cout << "Message saved to " << outputPath << std::endl;
        }
    } else if (mode == "generate") {
        if (argc < 4){
            std::cout << "Usage generate: " << "ImaEasyCrypt" << " <outputPath> -m generate -w <width> -h <height>" << std::endl;
            return 1;
        }else{
            std::string outputPath = argv[1];
            int width = 100;
            int height = 100;
            for(int i = 2; i < argc; i++){
                std::string arg = argv[i];
                if (arg == "-w"){
                    width = (int) std::stoi(argv[i+1]);
                    std::cout << "width:" << width <<std::endl;
                } else if (arg == "-h"){
                    height = (int) std::stoi(argv[i+1]);
                    std::cout << "height:" << height <<std::endl;
                }
            }
            std::cout << "Generating random image..." << std::endl;
            PNGImage::createRandomImage(width,height,outputPath);
            std::cout << "Image saved to " << outputPath << std::endl;
        }
    } else {
        usage();
        return 1;
    }

    return 0;
}