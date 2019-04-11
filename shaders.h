/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   shader.h
 * Author: tylerw
 *
 * Created on April 3, 2019, 10:40 PM
 */

#ifndef SHADERS_H
#define SHADERS_H

class Shader {
    
    Matrix mat_view;
    Matrix mat_proj;
    Matrix mat_viewport;
    
    Matrix mat_view_it;
    Matrix mat_proj_it;
    
    Vec3f ndc[3];
    Vec3f norm[3];
    Vec3f screen[3];
    Vec2f uv[3];
    Vec3f tan[3];
    Vec3f bitan[3];
    
public:
    void view(Vec3f& from, Vec3f& to, Vec3f& up);
    void projection(float fov_x, float fov_y, float n, float f);
    void viewport(int l, int r, int b, int t);
    void vertex(Model& model, int face_i, int vert_j);
    void world2clip_vert(Vec3f vert);
    void world2clip_vec(Vec3f vec);
    void triangle(Vec3f light_vec, \
        std::vector<std::vector<float>>& z_buffer, \
        TGAImage image);
};

#endif /* SHADERS_H */

