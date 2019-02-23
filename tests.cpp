/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <algorithm>
#include <climits>
#include <iostream>
#include <random>
#include <vector>
#include "drawing.h"
#include "model.h"
#include "tgaimage.h"
#include "tests.h"

using namespace std;

void vec3f_print(Vec3f vec) {
    
    for (int j = 0; j <= 2; j++) {
        cout << vec[j] << " ";
    }
    
    cout << endl;
    
    return;
}

TGAColor rand_color() {
    
    TGAColor color = TGAColor(std::rand()%256, std::rand()%256, std::rand()%256, 255);
    
    return color;
}

void line_test() {
    
    TGAImage image(100, 100, TGAImage::RGB);
    const TGAColor white(255, 255, 255, 255);
    const TGAColor red(255, 0, 0, 255);
    const TGAColor green(0, 255, 0, 255);
    const TGAColor blue(0, 0, 255, 255);
    
    line(50, 50, 90, 70, image, red); // base case
    line(50, 50, 70, 90, image, blue); // steep
    line(50, 50, 10, 30, image, green); // x0 > x1
    line(50, 50, 90, 30, image, red); // negative slope
    line(50, 50, 30, 10, image, green); // x0 > x1, steep
    line(50, 50, 10, 70, image, red); // x0 > x1, negative
    line(50, 50, 30, 90, image, white); // x0 > x1, negative, steep
    line(50, 50, 70, 10, image, white); // negative, steep
    line(50, 50, 90, 50, image, green); // horizontal line
    line(50, 50, 50, 90, image, green); //vertical line
    line(60, 90, 60, 90, image, blue); //single point
    line(90, 90, 110, 150, image, white); //point outside image
    
    image.write_tga_file("output/line_test.tga");
    
    return;
}

void small_test() {
    
    int zbuff[100][100] = {INT_MIN};
    
    std::cout << zbuff[0][0] << std::endl;
    
}

void wireframe_test() {
    
    Model model("tinyrenderer-files/obj/african_head/african_head.obj");
    
    const int width = 700;
    const int height = 700;
    
    TGAImage image(width, height, TGAImage::RGB);
    const TGAColor white(255, 255, 255, 255);
    const TGAColor red(255, 0, 0, 255);
    const TGAColor green(0, 255, 0, 255);
    const TGAColor blue(0, 0, 255, 255);
    
    vector<TGAColor> color_vec;
    color_vec.push_back(white);
    color_vec.push_back(red);
    color_vec.push_back(green);
    color_vec.push_back(blue);
    
    // Initialize RNG seed for random color generator
    srand(time(NULL));
    
    for (int i = 0; i < model.nfaces(); i++) {
        vector<int> face = model.face(i);
        // Draw lines between all vertices of face
        for (int j = 0;j < 3; j++) {
            Vec3f v0 = model.vert(face[j]);
            Vec3f v1 = model.vert(face[(j + 1) % 3]);
            
            /* Scale coordinate to size of image by adding 1(-1:1 -> 0:2) and 
             * multiplying by width/height and dividing by 2*/
            int x0 = (v0[0] + 1) * width / 2;
            int y0 = (v0[1] + 1) * height / 2;
            int x1 = (v1[0] + 1) * width / 2;
            int y1 = (v1[1] + 1) * height / 2;
//            line(x0, y0, x1, y1, image, color_vec[(i % 4)]);
            line(x0, y0, x1, y1, image, rand_color());
        }
    }
    
    // Flip image vertically as it is drawn upside-down
    image.flip_vertically();
    image.write_tga_file("output/wireframe_test.tga");
    
    return;
}

void triangle_test() {
    
    TGAImage image(1000, 1000, TGAImage::RGB);
    const TGAColor white(255, 255, 255, 255);
    const TGAColor red(255, 0, 0, 255);
    const TGAColor green(0, 255, 0, 255);
    const TGAColor blue(0, 0, 255, 255);
    
    image.set(1, 1, white);
    image.set(1, 999, white);
    image.set(999, 1, white);
    image.set(999, 999, white);
    
    Vec2I A(760, 500);
    Vec2I B(80, 900);
    Vec2I C(440, 10);
    
    Vec2I D(750, 890);
    
    triangle(A, B, C, image, red);
    triangle(A, D, B, image, green);
    
    image.write_tga_file("output/triangle_test.tga");
    
    return;
}

void triangle_model_test() {
    
    Model model("tinyrenderer-files/obj/african_head/african_head.obj");
    
    const int width = 750;
    const int height = 750;
    
    TGAImage image(width, height, TGAImage::RGB);
    const TGAColor white(255, 255, 255, 255);
    const TGAColor red(255, 0, 0, 255);
    const TGAColor green(0, 255, 0, 255);
    const TGAColor blue(0, 0, 255, 255);
    
    vector<TGAColor> color_vec;
    color_vec.push_back(white);
    color_vec.push_back(red);
    color_vec.push_back(green);
    color_vec.push_back(blue);
    
    // initialize z-buffer array
    int z_buffer[width][height];
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++){
            z_buffer[i][j] = INT_MIN;
        }
    }
    
    // light direction vector
    Vec3f light_vec(0.f, 0.f, -1.f);
    
    // Initialize RNG seed for random color generator
    srand(time(NULL));
    
    for (int i = 0; i < model.nfaces(); i++) {
        vector<int> face = model.face(i);
        
        Vec2I pts[3];
        
        Vec3f verts[3];
        
        for (int j = 0;j < 3; j++) {
            verts[j] = model.vert(face[j]);

            /* Scale coordinate to size of image by adding 1(-1:1 -> 0:2) and 
             * multiplying by width/height and dividing by 2*/
            int x = (verts[j][0] + 1) * width / 2;
            int y = (verts[j][1] + 1) * height / 2;
            
            Vec2I P(x, y);
            pts[j] = P;

//            line(x0, y0, x1, y1, image, rand_color());
        }
        
        /* need to cross p0p2 x p0p1, not the reverse(p0p1 x p0p2) to get proper
         * normal, otherwise vector will be reversed*/
        Vec3f norm = cross(verts[2] - verts[0], verts[1] - verts[0]);
        norm.normalize();
        
        float intensity = norm*light_vec;
        
//        triangle(pts[0], pts[1], pts[2], image, rand_color());
        if (intensity >= 0) {
            triangle(pts, image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
        
    }
    
    // Flip image vertically as it is drawn upside-down
    image.flip_vertically();
    image.write_tga_file("output/triangle_model_test.tga");
    
    return;
}