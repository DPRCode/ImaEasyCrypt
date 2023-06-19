#include "PNGImage.h"
#include <iostream>
#include "Stegano.h"
int main() {
    //PNGImage::demo();
    Stegano::demo3();
    PNGImage image;
    image.loadImage("/home/janek/SynologyDrive/HS_Mainz/SS23/EFFProg/ImaEasyCrypt/image3_encoded.png");
    std::vector<std::vector<Pixel>> imageDat = image.getPixelData();
    return 0;
}