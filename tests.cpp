/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <algorithm>
#include <cfloat>
#include <iostream>
#include <vector>
#include "drawing.h"
#include "geometry.h"
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
    
    Vec3f testv(1.4, 2.5, 3.5);
    
    Matrix mat = Matrix::identity();
    
    Vec3f testres = Cartesian(mat * Homogeneous(testv));
    
    std::cout << testres << endl;
    
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
//
//void triangle_test() {
//    
//    TGAImage image(1000, 1000, TGAImage::RGB);
//    const TGAColor white(255, 255, 255, 255);
//    const TGAColor red(255, 0, 0, 255);
//    const TGAColor green(0, 255, 0, 255);
//    const TGAColor blue(0, 0, 255, 255);
//    
//    image.set(1, 1, white);
//    image.set(1, 999, white);
//    image.set(999, 1, white);
//    image.set(999, 999, white);
//    
//    Vec2i A(760, 500);
//    Vec2i B(80, 900);
//    Vec2i C(440, 10);
//    
//    Vec2i D(750, 890);
//    
//    triangle(A, B, C, image, red);
//    triangle(A, D, B, image, green);
//    
//    image.write_tga_file("output/triangle_test.tga");
//    
//    return;
//}

void triangle_model_test() {
    
    Model model("tinyrenderer-files/obj/african_head/african_head.obj");
    
    // image dimensions
    
    float asp_ratio = 1; //aspect ratio
    
    const int width = 1000;
    const int height = width / asp_ratio;
    
    TGAImage image(width, height, TGAImage::RGB);
    
    // initialize z-buffer array
    vector<vector<float>> z_buffer(height, vector<float>(width, -FLT_MAX));
    
    // light direction vector
    Vec3f light_vec(0.f, 0.f, -1.f);
    
    // view matrix
    Vec3f to(0, 0, 0);
    Vec3f from(0, 0, 3);
    Vec3f up(0, 1, 0);
    
    Matrix viewmat = view_matrix(from, to, up);
    
    // perspective matrix
    float fov_x = 60; //degrees
    float fov_y = fov_x / asp_ratio;
    float n = .5;
    float f = 6;
    
    Matrix persp = perspective_matrix(fov_x, fov_y, n, f);
    
    // viewport matrix
    Matrix viewport = viewport_matrix(0, width, 0, height);
    
    
    // loop through all faces of model
    for (int i = 0; i < model.nfaces(); i++) {
        vector<int> face = model.face(i);
        
        Vec2i pts[3];
        Vec3f z_vals;
        
        Vec3f verts[3];
        Vec3f verts_world[3];
        
        // texture u,v vertices
        Vec2f tex_uv[3];
        
        for (int j = 0;j < 3; j++) {
            verts_world[j] = model.vert(face[j]);
            
            verts_world[j] = Cartesian(viewmat * Homogeneous(verts_world[j]));
            
            verts[j] = Cartesian(viewport * persp * Homogeneous(verts_world[j]));
            
            tex_uv[j] = model.uv(i, j);
        }
        
        /* need to cross p0p2 x p0p1, not the reverse(p0p1 x p0p2) to get proper
         * normal, otherwise vector will be reversed*/
        Vec3f norm = cross((verts_world[2] - verts_world[0]), (verts_world[1] - verts_world[0]));
        norm.normalize();
        
        // dot product of face normal and light vector to get light intensity
        float intensity = norm * light_vec;
        
        if (i%100==0) {
            cout << norm << endl;
        }
        
        if (intensity >= 0) {
            triangle_fp(verts, z_buffer, model, tex_uv, image, intensity);
        }
        
    }
    
    // Flip image vertically as it is drawn upside-down
    image.flip_vertically();
    image.write_tga_file("output/triangle_model_persp_test.tga");
    
    return;
}