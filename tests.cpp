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
#include "render.h"
#include "shaders.h"
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
    
    line(Vec2i(50, 50), Vec2i(90, 70), image, red); // base case
    line(Vec2i(50, 50), Vec2i(70, 90), image, blue); // steep
    line(Vec2i(50, 50), Vec2i(10, 30), image, green); // x0 > x1
    line(Vec2i(50, 50), Vec2i(90, 30), image, red); // negative slope
    line(Vec2i(50, 50), Vec2i(30, 10), image, green); // x0 > x1, steep
    line(Vec2i(50, 50), Vec2i(10, 70), image, red); // x0 > x1, negative
    line(Vec2i(50, 50), Vec2i(30, 90), image, white); // x0 > x1, negative, steep
    line(Vec2i(50, 50), Vec2i(70, 10), image, white); // negative, steep
    line(Vec2i(50, 50), Vec2i(90, 50), image, green); // horizontal line
    line(Vec2i(50, 50), Vec2i(50, 90), image, green); //vertical line
    line(Vec2i(60, 90), Vec2i(60, 90), image, blue); //single point
    line(Vec2i(90, 90), Vec2i(110, 150), image, white); //point outside image
    
    image.write_tga_file("output/line_test.tga");
    
    return;
}

void small_test() {
    
    Vec3f v1(0, 0, 0);
    Vec3f v2(0, 0, 0);
    
//    std::cout << (v1 == v2) << endl;
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
            line(Vec2i(x0, y0), Vec2i(x1, y1), image, rand_color());
        }
    }
    
    // Flip image vertically as it is drawn upside-down
    image.flip_vertically();
    image.write_tga_file("output/wireframe_test.tga");
    
    return;
}

void triangle_model_test() {
    
    Matrix o2w = Matrix::identity();
    Vec3f o2w_trans(1, 1, 2);
    o2w.set_col(3, o2w_trans);
    
    const char* filename = "tinyrenderer-files/obj/african_head/african_head.obj";
    Model model(filename);
    
    // image dimensions
    float asp_ratio = 1.77; //aspect ratio
    
    int width = 2000;
    int height = width / asp_ratio;
    
    TGAImage image(width, height, TGAImage::RGB);
    
    // initialize z-buffer array
    vector<vector<float>> z_buffer(width, vector<float>(height, FLT_MAX));
    
    /* light direction vector is negative of direction light is actually coming
     from*/
    Vec3f light_vec(0, 0, 1);
    light_vec.normalize();
    
    // view matrix
    Vec3f to(0.f, 0.f, 0.f);
    Vec3f from(5.f, 3.f, 5.f);
    Vec3f up(0.f, 1.f, 0.f);
    
    Matrix viewmat = view_matrix(from, to, up);
    
    // view direction vector for back face culling
    // should not change since it is in camera reference frame, which doesn't change
    Vec3f view_vec(0.f, 0.f, 1.f);
    
    // perspective matrix
    float fov_x = 60; //degrees
    float fov_y = fov_x / asp_ratio;
    float n = 1;
    float f = 6;
    
    Matrix persp = perspective_matrix(fov_x, fov_y, n, f);
    
    // viewport matrix
    Matrix viewport_mat = viewport_matrix(0, width, 0, height);
    
    // transform light vector
    light_vec = proj<3>(viewmat * embed<4>(light_vec, 0.f));
    
    // loop through all faces of model
    for (int i = 0; i < model.nfaces(); i++) {
        vector<int> face = model.face(i);
        
        Vec3f verts[3];
        Vec3f verts_world[3];
        
        for (int j = 0;j < 3; j++) {
            verts_world[j] = model.vert(face[j]);
//            verts_world[j] = transform_vertex(verts_world[j], viewmat);
            
//            verts[j] = transform_vertex(verts_world[j], persp, viewport_mat);
        }
        
        Vec3f norm = cross((verts_world[1] - verts_world[0]), (verts_world[2] - verts_world[0]));
        norm.normalize();
        
        // dot product of face normal and light vector to get light intensity
        float intensity = norm * light_vec;
        
        if ((norm * view_vec) > 0) {
//            triangle_flat(verts, z_buffer, model, i, image, intensity);
//            triangle_gouraud(verts, z_buffer, model, i, image, light_vec, viewmat);
//            triangle_phong(verts, z_buffer, model, i, image, light_vec, viewmat);
            triangle_darboux(verts, z_buffer, model, i, image, light_vec, viewmat);
        }
        
    }
    
    // Flip image vertically as it is drawn upside-down
    image.flip_vertically();
    
    image.write_tga_file("output/object_world_transform.tga");
    
    return;
}

void render_test() {
    
    int width = 2000;
    int height = width / 1.77;
    
    const char* modelfile = "tinyrenderer-files/obj/african_head/african_head.obj";
    ModelMatrix mod_mat;
    
    std::vector<Model> model_vec;
    std::vector<ModelMatrix> modmat_vec;
    
    TGAImage fbuffer(width, height, TGAImage::RGB);
    
    Render render(fbuffer);
    
    for (int i = 0; i < model_vec.size(); i++) {
        Model curr_model = model_vec[i];
        ModelMatrix curr_modmat = modmat_vec[i];
        render_model(curr_model, curr_modmat, render);
    }
    
}

//void shader_test() {
//    
//    // SHADER
//    Shader shader;
//    
//    // MODEL
//    Matrix o2w = Matrix::identity();
//    Vec3f o2w_trans(1, 1, 2);
//    o2w.set_col(3, o2w_trans);
//    
//    const char* filename = "tinyrenderer-files/obj/african_head/african_head.obj";
//    Model model(filename);
//    
//    // IMAGE
//    float asp_ratio = 1.77; //aspect ratio
//    
//    int width = 2000;
//    int height = width / asp_ratio;
//    
//    TGAImage image(width, height, TGAImage::RGB);
//    
//    shader.viewport(0, width, 0, height);
//    
//    // Z BUFFER
//    vector<vector<float>> z_buffer(width, vector<float>(height, FLT_MAX));
//    
//    // CAMERA
//    Vec3f to(0.f, 0.f, 0.f);
//    Vec3f from(5.f, 3.f, 5.f);
//    Vec3f up(0.f, 1.f, 0.f);
//    
//    shader.view(from, to, up);
//    
//    // perspective matrix
//    float fov_x = 60; //degrees
//    float fov_y = fov_x / asp_ratio;
//    float n = 1;
//    float f = 6;
//    
//    shader.projection(fov_x, fov_y, n, f);
//    
//    // LIGHT
//    Vec3f light_vec(0, 0, 1);
//    light_vec.normalize();
//    
//    shader.world2clip_vec(light_vec);
//    
//    // view direction vector for back face culling
//    // should not change since it is in camera reference frame, which doesn't change
//    Vec3f view_vec(0.f, 0.f, 1.f);
//    
//    for (int i = 0; i < model.nfaces(); i++) {
//        for (int j = 0; j < 3; j++) {
//            shader.vertex(model, i, j);
//        }
//        shader.rasterize(light_vec, z_buffer, image);
//        
//    }
//    
//}