#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include "drawing.h"
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

// structures

// utility functions

int sign(const int& x) {
    /* Returns sign of int.  (x>=0) returns 1.  (x<0) returns -1.*/
    return (x >= 0) - (x < 0);
}

bool inside_triangle(const Vec2i* pts, const Vec2i& P) {
    /* Returns true if points P is inside triangle of points in pts*/
    
    // pre-compute values that are used multiple times
    int y1_y2 = pts[1].y - pts[2].y;
    int x2_x1 = pts[2].x - pts[1].x;
    int x0_x2 = pts[0].x - pts[2].x;
    int y0_y2 = pts[0].y - pts[2].y;
    int y2_y0 = pts[2].y - pts[0].y;
    
    int det = (y1_y2 * x0_x2) + (x2_x1 * y0_y2);
    int sign_det = sign(det);
    
    int lambda_1 = sign_det * ((y1_y2 * (P.x - pts[2].x)) + (x2_x1 * (P.y - pts[2].y)));
    if (lambda_1 >= 0) {
        int lambda_2  = sign_det * ((y2_y0 * (P.x - pts[2].x)) + (x0_x2 * (P.y - pts[2].y)));
        if (lambda_2 >= 0) {
            int lambda_3 = det - sign_det * (lambda_1 + lambda_2);
            if ((sign_det>0) ? (lambda_3 > 0) : (lambda_3 < 0)) {
                return true;
            }   
        }
    }
    
    return false;
}

Vec3f barycentric(const Vec2i* pts, const Vec2i& P) {
    /* Returns true if points P is inside triangle of points in pts*/
    
    // pre-compute values that are used multiple times
    int y1_y2 = pts[1].y - pts[2].y;
    int x2_x1 = pts[2].x - pts[1].x;
    int x0_x2 = pts[0].x - pts[2].x;
    int y0_y2 = pts[0].y - pts[2].y;
    int y2_y0 = pts[2].y - pts[0].y;
    
    int det = (y1_y2 * x0_x2) + (x2_x1 * y0_y2);
    
    Vec3f lambda;
    
    /* Since any negative barycentric coordinate rejects the point, we can check
     if the point is positive after each calculation, and skip the remaining
     calculations if it is negative. */
    lambda[0] = ((y1_y2 * (P.x - pts[2].x)) + (x2_x1 * (P.y - pts[2].y))) / (float)det;
    if (lambda[0] >= 0) {
        lambda[1]  = ((y2_y0 * (P.x - pts[2].x)) + (x0_x2 * (P.y - pts[2].y))) / (float)det ;
        if (lambda[1] >= 0) {
            lambda[2] = 1 - lambda[1] - lambda[0];
            if (lambda[2] >= 0) {
                return lambda;
            }   
        }
    }
    
    return Vec3f(-1, -1, -1);
}

Vec3f barycentric_fp(const Vec3f* pts, const Vec2f& P) {
    /* Returns true if points P is inside triangle of points in pts*/
    
    // pre-compute values that are used multiple times
    float y1_y2 = pts[1].y - pts[2].y;
    float x2_x1 = pts[2].x - pts[1].x;
    float x0_x2 = pts[0].x - pts[2].x;
    float y0_y2 = pts[0].y - pts[2].y;
    float y2_y0 = pts[2].y - pts[0].y;
    
    float inv_det = 1.f / ((y1_y2 * x0_x2) + (x2_x1 * y0_y2));
    
    Vec3f lambda;
    
    /* Since any negative barycentric coordinate rejects the point, we can check
     if the point is positive after each calculation, and skip the remaining
     calculations if it is negative. */
    lambda[0] = ((y1_y2 * (P.x - pts[2].x)) + (x2_x1 * (P.y - pts[2].y))) * inv_det;
    if (lambda[0] >= 0) {
        lambda[1]  = ((y2_y0 * (P.x - pts[2].x)) + (x0_x2 * (P.y - pts[2].y))) * inv_det ;
        if (lambda[1] >= 0) {
            lambda[2] = 1.f - lambda[1] - lambda[0];
            if (lambda[2] >= 0) {
                return lambda;
            }   
        }
    }
    
    return Vec3f(-1, -1, -1);
}

// expand this function to interpolate any types
float z_interpolate(Vec3f bary, Vec3f* pts) {
    
//    return (bary * z_vals);
    
    float z;
    
    for (int i=0; i<3;i++) {
        z += bary[i] * pts[i][2];
    }

    return z;
}

// merge w/ z_interpolate later as template
Vec2f uv_interpolate(Vec3f bary, Vec2f* tex_uv) {
    
    Vec2f uv;
    
    for (int i=0; i<3;i++) {
        for (int j = 0; j < 2; j++) {
            uv[j] += bary[i] * tex_uv[i][j];
        }
    }

    return uv;
}

Matrix view_matrix(const Vec3f& from, const Vec3f& to, Vec3f& up) {
    
    Vec3f forward = from - to;
    Vec3f right = cross(up.normalize(), forward.normalize());
    Vec3f up_ortho = cross(forward, right);
    
    Vec3f vec_arr[] = {right, up_ortho, forward};
    
    Matrix view_matrix = Matrix::identity();
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            view_matrix[i][j] = (j!=3) ? vec_arr[i][j]: -(vec_arr[i] * from);
        }
    }
    
    return view_matrix;
}

// change parameters later to accept structure

Matrix perspective_matrix(float fov_x, float fov_y, float n, float f) {
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
    
    float l = -n * std::tan((fov_x / 2) * (180 / M_PI));
    float r = -l;
    float b = -n * std::tan((fov_y / 2) * (180 / M_PI));
    float t = -b;
    
    Matrix perspective_matrix;
    
    perspective_matrix[0][0] = (2 * n) / (r -l);
    perspective_matrix[0][1] = 0;
    perspective_matrix[0][2] = (r + l) / (r - l);
    perspective_matrix[0][3] = 0;
    
    perspective_matrix[1][0] = 0;
    perspective_matrix[1][1] = (2 * n) / (t - b);
    perspective_matrix[1][2] = (t + b) / (t - b);
    perspective_matrix[1][3] = 0;
    
    perspective_matrix[2][0] = 0;
    perspective_matrix[2][1] = 0;
    perspective_matrix[2][2] = n / (f - n);
    perspective_matrix[2][3] = (n * f) / (f - n);
    
    perspective_matrix[3][0] = 0;
    perspective_matrix[3][1] = 0;
    perspective_matrix[3][2] = -1;
    perspective_matrix[3][3] = 0;
    
    return perspective_matrix;
}

Matrix viewport_matrix(int l, int r, int b, int t) {
    
    Matrix viewport_matrix = Matrix::identity();
    
    viewport_matrix[0][0] = (r - l) / 2.f;
    viewport_matrix[0][3] = (r + l) / 2.f;
    viewport_matrix[1][1] = (t - b) / 2.f;
    viewport_matrix[1][3] = (t + b) / 2.f;
    viewport_matrix[2][2] = -1;
    viewport_matrix[2][3] = 1;
    
    return viewport_matrix;
}

// line drawing functions
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    /* Given two coordinates (x0, y0) and (x1, y1), draws a line of given color
     on the input image. Note that the bresenham_line function can handle 
     horizontal and vertical lines, however they have been given their own 
     functions for optimization purposes.*/
    
    if (x0 == x1) {
        vert_line(x0, y0, y1, image, color);
    } else if (y0 == y1) {
        horiz_line(x0, x1, y1, image, color);
    } else {
        bresenham_line(x0, y0, x1, y1, image, color);
    }
    return;
}

void bresenham_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    
    // set steep bool if slope > 1 and swap x/y if so
    bool steep = (std::abs(y1 - y0) > std::abs(x1 - x0)) ? true : false;
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    
    // swap points if x0 > x1
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    const int dx = x1 - x0;
    int dy = y1 - y0;
    int y = y0;
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
        for (int x = x0; x <= x1; x++) {

            image.set(x, y, color);

            if (2 * (err + dy) < dx) {
                err += dy;
            } else {
                y += yincr;
                err += dy-dx;
            }

        }
    } else {
        for (int x = x0; x <= x1; x++) {

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

void horiz_line(int x0, int x1, int y, TGAImage &image, TGAColor color) {
    if (x0 > x1) {
        std::swap(x0, x1);
    }
    
    for (int x = x0; x <= x1; x++) {
        image.set(x, y, color);
    }
    return;
}

void vert_line(int x, int y0, int y1, TGAImage &image, TGAColor color) {
    if (y0 > y1) {
        std::swap(y0, y1);
    }
    
    for (int y = y0; y <= y1; y++) {
        image.set(x, y, color);
    }
    return;
}

// triangle drawing functions

void triangle_fp (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, Vec2f* tex_uv, \
        TGAImage& image, float intensity) {

    BoundingBox bbox(pts, image);
    
    Vec2i P;
    
    for (P.x = bbox.x_lower; P.x < bbox.x_upper; P.x++) {
        for (P.y = bbox.y_lower; P.y < bbox.y_upper; P.y++) {
            
            Vec3f bc = barycentric_fp(pts, P);
            
            float z = z_interpolate(bc, pts);
            
            if (bc[2] >= 0) {
                if (z > (z_buffer[P.x][P.y])) {
                    z_buffer[P.x][P.y] = z;
                    Vec2f uv = uv_interpolate(bc, tex_uv);
                    TGAColor color_uv = model.diffuse(uv);
                    image.set(P.x, P.y, color_uv * intensity);
                }
            }
        }
    }
    
    return;
}