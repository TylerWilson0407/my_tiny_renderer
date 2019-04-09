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

#ifndef SHADER_H
#define SHADER_H

class VertexShader {
    Matrix mat_world2cam;
    Matrix mat_projection;
    Matrix mat_viewport;
    
public:
    void world_camera(Vec3f& from, Vec3f& to, Vec3f& up);
    void projection(float fov_x, float fov_y, float n, float f);
    void viewport(int l, int r, int b, int t);
};

#endif /* SHADER_H */

