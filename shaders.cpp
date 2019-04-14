/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "drawing.h"
#include "geometry.h"
#include "model.h"
#include "shaders.h"
#include "tgaimage.h"


void Shader::view(Vec3f& from, Vec3f& to, Vec3f& up) {
    Vec3f forward = from - to;
    forward.normalize();
    Vec3f right = cross(up.normalize(), forward);
    right.normalize();
    Vec3f up_ortho = cross(forward, right);

    Vec3f vec_arr[] = {right, up_ortho, forward};

    mat_view = Matrix::identity();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            mat_view[i][j] = (j!=3) ? vec_arr[i][j]: -(vec_arr[i] * from);
        }
    }

    mat_view_it = mat_view.invert_transpose();

}

void Shader::projection(float fov_x, float fov_y, float n, float f) {
    /* Maps 3D points from eye coordinates(viewing frustum) to normalized device
     coordinates (NDC space). This matrix uses reversed-Z convention, mapping to
     zero on the far clipping plane and positive 1 on the near clipping plane, 
     to take advantage of increased floating-point precision near zero to 
     attempt to cancel out the decreased depth value distribution as Z moves 
     away from the near plane.  See this blog post:
     * 
     https://developer.nvidia.com/content/depth-precision-visualized
     * 
     */

    float l = -n * std::tan((fov_x / 2) * (M_PI / 180));
    float r = -l;
    float b = -n * std::tan((fov_y / 2) * (M_PI / 180));
    float t = -b;

    mat_proj = Matrix::identity();

    mat_proj[0][0] = (2 * n) / (r -l);
    mat_proj[0][2] = (r + l) / (r - l);

    mat_proj[1][1] = (2 * n) / (t - b);
    mat_proj[1][2] = (t + b) / (t - b);

    mat_proj[2][2] = n / (f - n);
    mat_proj[2][3] = (n * f) / (f - n);

    mat_proj[3][2] = -1;

    mat_proj_it = mat_proj.invert_transpose();

}

void Shader::viewport(int l, int r, int b, int t) {

    mat_viewport = Matrix::identity();

    mat_viewport[0][0] = (r - l) / 2.f;
    mat_viewport[0][3] = (r + l) / 2.f;
    mat_viewport[1][1] = (t - b) / 2.f;
    mat_viewport[1][3] = (t + b) / 2.f;
    mat_viewport[2][2] = -1;
    mat_viewport[2][3] = 1;

}

//void Shader::vertex(Model& model, int face_i, int vert_j) {
//    ndc[vert_j] = transform_vertex(model.vert(face_i, vert_j), mat_view, mat_proj);
//    
//    screen[vert_j] = transform_vertex(ndc[vert_j], mat_viewport);
//
//    // normals must be transformed by inverse-transpose of matrices
//    norm[vert_j] = transform_vector(model.normal(face_i, vert_j), mat_view_it, mat_proj_it);
//
//    tan[vert_j] = transform_vector(model.tangent(face_i, vert_j), mat_view, mat_proj);
//    bitan[vert_j] = transform_vector(model.bitangent(face_i, vert_j), mat_view, mat_proj);
//
//    uv[vert_j] = model.uv(face_i, vert_j);
//
//}

void Shader::rasterize(Vec3f light_vec, \
        std::vector<std::vector<float>>& z_buffer, \
        TGAImage image) {
    
    BoundingBox bbox(screen, image);
    Vec2i P;
    
    for (P.x = bbox.x_lower; P.x < bbox.x_upper; P.x++) {
        for (P.y = bbox.y_lower; P.y < bbox.y_upper; P.y++) {
            
            Vec3f bc = barycentric(screen, P);
            
            Vec3f P_coords = interpolate(bc, screen);
            
            if ((bc[2] >= 0) && (P_coords[2] < (z_buffer[P.x][P.y]))) {
                z_buffer[P.x][P.y] = P_coords[2];
//                Vec3f P_norm = interpolate(bc, norm);
//                Vec2f P_uv = interpolate(bc, uv);
//                Vec3f P_tan = interpolate(bc, tan);
//                Vec3f P_bitan = interpolate(bc, bitan);
                
                /// PICK UP HERE, ONLY CALCULATE WHATS NEEDED FOR FRAGMENT
            }
            
        }
    }
    
}

void Shader::fragment(Vec3f bc, TGAImage& image) {
    
    
    
}

//void Shader::world2clip_vert(Vec3f vert) {
//    vert = transform_vertex(vert, mat_view, mat_proj);
//}
//
//void Shader::world2clip_vec(Vec3f vec) {
//    vec = transform_vector(vec, mat_view, mat_proj);
//}