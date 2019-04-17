#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include "drawing.h"
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

Vec3f barycentric(const Vec3f* pts, const Vec2f& P) {
    /* Returns barycentric coordinates if point P is inside triangle of points 
     * in pts.  Otherwise returns negative values(which are rejected by shader.
     */
    
    // pre-compute values that are used multiple times
    float x0_x2 = pts[0].x - pts[2].x;
    float x2_x1 = pts[2].x - pts[1].x;
    float y0_y2 = pts[0].y - pts[2].y;
    float y1_y2 = pts[1].y - pts[2].y;
    
    float inv_det = 1.f / ((y1_y2 * x0_x2) + (x2_x1 * y0_y2));
    
    Vec3f lambda;
    
    /* Since any negative barycentric coordinate rejects the point, we can check
     if the point is positive after each calculation, and skip the remaining
     calculations if it is negative. */
    lambda[0] = ((y1_y2 * (P.x - pts[2].x)) + (x2_x1 * (P.y - pts[2].y))) * inv_det;
    if (lambda[0] >= 0) {
        lambda[1]  = ((-y0_y2 * (P.x - pts[2].x)) + (x0_x2 * (P.y - pts[2].y))) * inv_det ;
        if (lambda[1] >= 0) {
            lambda[2] = 1.f - lambda[1] - lambda[0];
            if (lambda[2] >= 0) {
                return lambda;
            }   
        }
    }
    
    return Vec3f(-1, -1, -1);
}

//Matrix view_matrix(const Vec3f& from, const Vec3f& to, Vec3f& up) {
//    
//    Vec3f forward = from - to;
//    forward.normalize();
//    Vec3f right = cross(up.normalize(), forward);
//    right.normalize();
//    Vec3f up_ortho = cross(forward, right);
//    
//    Vec3f vec_arr[] = {right, up_ortho, forward};
//    
//    Matrix view_matrix = Matrix::identity();
//    
//    for (int i = 0; i < 3; i++) {
//        for (int j = 0; j < 4; j++) {
//            view_matrix[i][j] = (j!=3) ? vec_arr[i][j]: -(vec_arr[i] * from);
//        }
//    }
//    
//    return view_matrix;
//}
//
//Matrix perspective_matrix(float fov_x, float fov_y, float n, float f) {
//    /* Maps 3D points from eye coordinates(viewing frustum) to normalized device
//     coordinates (NDC space). This matrix uses reversed-Z convention, mapping to
//     zero on the far clipping plane and positive 1 on the near clipping plane, 
//     to take advantage of increased floating-point precision near zero to 
//     attempt to cancel out the decreased depth value distribution as Z moves 
//     away from the near plane.  See this blog post:
//     * 
//     https://developer.nvidia.com/content/depth-precision-visualized
//     * 
//     */
//    
//    float l = -n * std::tan((fov_x / 2) * (M_PI / 180));
//    float r = -l;
//    float b = -n * std::tan((fov_y / 2) * (M_PI / 180));
//    float t = -b;
//    
//    Matrix perspective_matrix;
//    
//    perspective_matrix[0][0] = (2 * n) / (r -l);
//    perspective_matrix[0][1] = 0;
//    perspective_matrix[0][2] = (r + l) / (r - l);
//    perspective_matrix[0][3] = 0;
//    
//    perspective_matrix[1][0] = 0;
//    perspective_matrix[1][1] = (2 * n) / (t - b);
//    perspective_matrix[1][2] = (t + b) / (t - b);
//    perspective_matrix[1][3] = 0;
//    
//    perspective_matrix[2][0] = 0;
//    perspective_matrix[2][1] = 0;
//    perspective_matrix[2][2] = n / (f - n);
//    perspective_matrix[2][3] = (n * f) / (f - n);
//    
//    perspective_matrix[3][0] = 0;
//    perspective_matrix[3][1] = 0;
//    perspective_matrix[3][2] = -1;
//    perspective_matrix[3][3] = 0;
//    
//    return perspective_matrix;
//}
//
//Matrix viewport_matrix(int l, int r, int b, int t) {
//    
//    Matrix viewport_matrix = Matrix::identity();
//    
//    viewport_matrix[0][0] = (r - l) / 2.f;
//    viewport_matrix[0][3] = (r + l) / 2.f;
//    viewport_matrix[1][1] = (t - b) / 2.f;
//    viewport_matrix[1][3] = (t + b) / 2.f;
//    viewport_matrix[2][2] = -1;
//    viewport_matrix[2][3] = 1;
//    
//    return viewport_matrix;
//}

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    
    // set steep bool if slope > 1 and swap x/y if so
    bool steep = (std::abs(p1.y - p0.y) > std::abs(p1.x - p0.x)) ? true : false;
    if (steep) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
    }
    
    // swap points if x0 > x1
    if (p0.x > p1.x) {
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    }
    
    const int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    int y = p0.y;
    int yincr = 1;
    int err = 0;
    
    // set y to decrement if slope is negative
    if (dy < 0) {
        dy = -dy;
        yincr = -1;
    }
    
    /* transpose line (image.set(y, x, color) instead of (x, y, color) if steep 
     * since we swapped x/y if steep.
     */
    if (!steep) {
        for (int x = p0.x; x <= p1.x; x++) {

            image.set(x, y, color);

            if (2 * (err + dy) < dx) {
                err += dy;
            } else {
                y += yincr;
                err += dy-dx;
            }

        }
    } else {
        for (int x = p0.x; x <= p1.x; x++) {

            image.set(y, x, color);

            if (2 * (err + dy) < dx) {
                err += dy;
            } else {
                y += yincr;
                err += dy-dx;
            }

        }
    }
    return;
}

void line(Vec3f p0, Vec3f p1, TGAImage &image, TGAColor color) {
    Vec2i p0_2i(p0.x, p0.y);
    Vec2i p1_2i(p1.x, p1.y);
    line(p0_2i, p1_2i, image, color);
}

void triangle_flat (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, const int& i_face, \
        TGAImage& image, float intensity) {
    
    Vec2f uv_diff[3];
    
    for (int j = 0;j < 3; j++) {
            uv_diff[j] = model.uv(i_face, j);
        }

    BoundingBox bbox(pts, image);
    
    Vec2i P;
    
    for (P.x = bbox.x_lower; P.x < bbox.x_upper; P.x++) {
        for (P.y = bbox.y_lower; P.y < bbox.y_upper; P.y++) {
            
            Vec3f bc = barycentric(pts, P);
            
            Vec3f xyz = interpolate(bc, pts);
            
            if ((bc[2] >= 0) && (xyz[2] < (z_buffer[P.x][P.y]))) {
                z_buffer[P.x][P.y] = xyz[2];
                Vec2f uv = interpolate(bc, uv_diff);
                TGAColor color_uv = model.diffuse(uv);
                image.set(P.x, P.y, color_uv * intensity);
            }
        }
    }
    
    return;
}

void triangle_gouraud (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, const int& i_face, \
        TGAImage& image, const Vec3f& light_vec, \
        Matrix& viewmat) {
    
    Vec2f uv_diff[3];
    Vec3f normals[3];
    
    TGAColor vert_colors[3];
    
    for (int j = 0;j < 3; j++) {
            uv_diff[j] = model.uv(i_face, j);
            normals[j] = model.normal(i_face, j);
            
            normals[j] = proj<3>(viewmat.invert_transpose() * embed<4>(normals[j], 0.f));
            
            float intensity = normals[j] * light_vec;
            
            vert_colors[j] = model.diffuse(uv_diff[j]) * intensity;
        }

    BoundingBox bbox(pts, image);
    
    Vec2i P;
    
    for (P.x = bbox.x_lower; P.x < bbox.x_upper; P.x++) {
        for (P.y = bbox.y_lower; P.y < bbox.y_upper; P.y++) {
            
            Vec3f bc = barycentric(pts, P);
            
            Vec3f xyz = interpolate(bc, pts);
            
            if ((bc[2] >= 0) && (xyz[2] < (z_buffer[P.x][P.y]))) {
                z_buffer[P.x][P.y] = xyz[2];
                TGAColor diff = interpolate(bc, vert_colors);
                image.set(P.x, P.y, diff);
            }
        }
    }
    
    return;
}

void triangle_phong (Vec3f* verts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, const int& i_face, \
        TGAImage& image, const Vec3f& light_vec, \
        Matrix& viewmat) {
    
    Vec2f uv_diffs[3];
    Vec3f normals[3];
    
    for (int j = 0;j < 3; j++) {
            uv_diffs[j] = model.uv(i_face, j);
            normals[j] = model.normal(i_face, j);
            normals[j] = proj<3>(viewmat.invert_transpose() * embed<4>(normals[j], 0.f));
        }

    BoundingBox bbox(verts, image);
    
    Vec2i P;
    
    for (P.x = bbox.x_lower; P.x < bbox.x_upper; P.x++) {
        for (P.y = bbox.y_lower; P.y < bbox.y_upper; P.y++) {
            
            Vec3f bc = barycentric(verts, P);
            
            Vec3f xyz = interpolate(bc, verts);
            
            if ((bc[2] >= 0) && (xyz[2] < (z_buffer[P.x][P.y]))) {
                z_buffer[P.x][P.y] = xyz[2];
                Vec3f normal = interpolate(bc, normals);
                float intensity = std::max(0.f, normal * light_vec);
                Vec2f uv_diff = interpolate(bc, uv_diffs);
                TGAColor pix_color = model.diffuse(uv_diff) * intensity;
                image.set(P.x, P.y, pix_color);
            }
        }
    }
    
    return;
}

void triangle_darboux (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, const int& i_face, \
        TGAImage& image, const Vec3f& light_vec, \
        Matrix& viewmat) {
    
    Vec2f uvs[3];
    Vec3f normals[3];
    Vec3f tans[3];
    Vec3f bitans[3];
    
    for (int j = 0;j < 3; j++) {
            uvs[j] = model.uv(i_face, j);
            normals[j] = model.normal(i_face, j);
            normals[j] = proj<3>(viewmat.invert_transpose() * embed<4>(normals[j], 0.f));
            
            tans[j] = model.tangent(i_face, j);
            tans[j] = proj<3>(viewmat * embed<4>(tans[j], 0.f));
            
            bitans[j] = model.bitangent(i_face, j);
            bitans[j] = proj<3>(viewmat * embed<4>(bitans[j], 0.f));
        }

    BoundingBox bbox(pts, image);
    
    Vec2i P;
    
    for (P.x = bbox.x_lower; P.x < bbox.x_upper; P.x++) {
        for (P.y = bbox.y_lower; P.y < bbox.y_upper; P.y++) {
            
            Vec3f bc = barycentric(pts, P);
            Vec3f xyz = interpolate(bc, pts);
            
            if ((bc[2] >= 0) && (xyz[2] < (z_buffer[P.x][P.y]))) {
                z_buffer[P.x][P.y] = xyz[2];
                Vec3f normal = interpolate(bc, normals);
                float intensity = std::max(0.f, normal * light_vec);
                Vec2f uv = interpolate(bc, uvs);
                Vec3f norm_ts = model.normal(uv);
                Vec3f tangent_vec = interpolate(bc, tans);
                Vec3f bitangent_vec = interpolate(bc, bitans);
                
                Vec3f darboux_normal;
                
                darboux_normal = tangent_vec * norm_ts[0] + \
                        bitangent_vec * norm_ts[1] + \
                        normal * norm_ts[2];
                
                intensity = std::max(0.f, darboux_normal * light_vec);
                
                TGAColor pix_color = model.diffuse(uv) * intensity;
                image.set(P.x, P.y, pix_color);
            }
        }
    }
    
    return;
}