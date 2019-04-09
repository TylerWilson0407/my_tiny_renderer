/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "geometry.h"

class VertexShader {
    
    Matrix mat_world2cam;
    Matrix mat_projection;
    Matrix mat_viewport;
    
public:
    void world_camera(Vec3f& from, Vec3f& to, Vec3f& up) {
        Vec3f forward = from - to;
        forward.normalize();
        Vec3f right = cross(up.normalize(), forward);
        right.normalize();
        Vec3f up_ortho = cross(forward, right);

        Vec3f vec_arr[] = {right, up_ortho, forward};

        mat_world2cam = Matrix::identity();

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                mat_world2cam[i][j] = (j!=3) ? vec_arr[i][j]: -(vec_arr[i] * from);
            }
        }
    }
    
    void projection(float fov_x, float fov_y, float n, float f) {
        
        float l = -n * std::tan((fov_x / 2) * (M_PI / 180));
        float r = -l;
        float b = -n * std::tan((fov_y / 2) * (M_PI / 180));
        float t = -b;

        mat_projection = Matrix::identity();

        mat_projection[0][0] = (2 * n) / (r -l);
        mat_projection[0][2] = (r + l) / (r - l);

        mat_projection[1][1] = (2 * n) / (t - b);
        mat_projection[1][2] = (t + b) / (t - b);

        mat_projection[2][2] = n / (f - n);
        mat_projection[2][3] = (n * f) / (f - n);

        mat_projection[3][2] = -1;
        
    }
    
    void viewport(int l, int r, int b, int t) {
    
        mat_viewport = Matrix::identity();

        mat_viewport[0][0] = (r - l) / 2.f;
        mat_viewport[0][3] = (r + l) / 2.f;
        mat_viewport[1][1] = (t - b) / 2.f;
        mat_viewport[1][3] = (t + b) / 2.f;
        mat_viewport[2][2] = -1;
        mat_viewport[2][3] = 1;

    }
    
};