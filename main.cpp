/* 
 * File:   main.cpp
 * Author: tylerw
 *
 * Created on February 18, 2019, 11:14 AM
 */

#include <iostream>
#include "line.h"
#include "tgaimage.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    TGAImage image(100, 100, TGAImage::RGB);
    const TGAColor white(255, 255, 255, 255);
    const TGAColor red(255, 0, 0, 255);
    const TGAColor green(0, 255, 0, 255);
    const TGAColor blue(0, 0, 255, 255);
    
    image.set(1, 1, white);
    image.set(1, 98, white);
    image.set(98, 1, white);
    image.set(98, 98, white);
    
    line(50, 50, 90, 70, image, red); // base case
    line(50, 50, 70, 90, image, blue); // steep
    line(50, 50, 10, 30, image, green); // x0 > x1
    line(50, 50, 90, 30, image, red); // negative slope
    line(50, 50, 30, 10, image, green); // x0 > x1, steep
    line(50, 50, 10, 70, image, red); // x0 > x1, negative
    line(50, 50, 30, 90, image, white); // x0 > x1, negative, steep
    
    
    image.write_tga_file("output.tga");
    
    return 0;
}