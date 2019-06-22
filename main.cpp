/* 
 * File:   main.cpp
 * Author: tylerw
 *
 * Created on February 18, 2019, 11:14 AM
 * 
 * 
 * 
 */


//#include "tests.h"

#include <algorithm>
#include <cfloat>
#include <iostream>
#include <vector>
#include "geometry.h"
#include "model.h"
#include "render.h"
#include "tgaimage.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    // INPUT VARIABLES
    /* Adjustable inputs below.  Any that are not self-explanatory are 
     * commented.*/
    
    // render model
    const char* modelfile = "tinyrenderer-files/obj/diablo3_pose/diablo3_pose.obj";
    
    // output filename
    const char* output = "output/render_output.tga";
    
    // frame size
    int width = 2000;
    float asp_ratio = 1.77; // aspect ratio
    int height = width / asp_ratio;
    
    // view matrix
    Vec3f to(0.f, 0.f, 0.f); // point the camera is looking TOWARDS
    Vec3f from(3, 1, 5); // point the camera is looking FROM
    Vec3f up(0.f, 1.f, 0.f); // Camera's UP direction
    
    // projection matrix
    float fovx = 60; // Field of view angle (horizontal)
    float fovy = fovx / asp_ratio; // FOV angle(vertical)
    float n = 1; // near clipping plane
    float f = 6;
    
    // lighting coefficients(should sum to 1)
    float ambient = 0.2;
    float diffuse = 0.6;
    float specular = 0.2;
    
    // light vector
    Vec3f light_vector(-1, 0, 3);
    
    // END INPUT VARIABLES
    
    Model model(modelfile);
    ModelMatrix mod_mat;
    
    std::vector<Model> model_vec;
    std::vector<ModelMatrix> modmat_vec;
    
    model_vec.push_back(model);
    modmat_vec.push_back(mod_mat);
    
    TGAImage fbuffer(width, height, TGAImage::RGB);
    
    Render render(fbuffer);
    
    render.ambient = ambient;
    render.diffuse = diffuse;
    render.specular = specular;
    
    render.view = view_matrix(from, to, up);
    render.proj = projection_matrix(fovx, fovy, n, f);
    
    render.light_vec = light_vector;
    render.light_vec.normalize();
    
    for (int i = 0; i < model_vec.size(); i++) {
        Model curr_model = model_vec[i];
        ModelMatrix curr_modmat = modmat_vec[i];
        Matrix model_matrix = curr_modmat.model_mat();
        render_model(curr_model, model_matrix, render);
    }
    
    render.framebuffer.write_tga_file(output);
    
    return 0;
}