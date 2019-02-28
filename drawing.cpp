#include <algorithm>
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

// expand this function to interpolate any types
float z_interpolate(Vec3f bary, Vec3f z_vals) {
    
//    return (bary * z_vals);
    
    float z;
    
    for (int i=0; i<3;i++) {
        z += bary[i] * z_vals[i];
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

void perspective_transform(Vec3f& vert, float c) {
    // perspective transform of vertex with camera at distance c
    
    Vec3f vert_persp;
    float r = 1 - (vert[2] / c);
    
//    std::cout << r << std::endl;
    
    for (int i = 0; i < 3; i++) {
        vert[i] = vert[i] / r;
    }
}

Matrix view_matrix(Vec3f& from, Vec3f& to) {
    
    Vec3f forward = from - to;
    forward.normalize();
    
    /* arbitrary temp vector to cross with forward, to get right vector, note
     that if temp is not a unit vector it must be normalized!!!! */
    Vec3f temp(0, 1, 0);
    Vec3f right = cross(temp, forward);
    
    // cross forward and right vecs to get up vector
    Vec3f up = cross(forward, right);
    
    Matrix view;
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            switch(i) {
                case 0: view[i][j] = right[j]; break;
                case 1: view[i][j] = up[j]; break;
                case 2: view[i][j] = forward[j]; break;
                case 3: view[i][j] = from[j]; break;
            }
        }
    }
    
    return view;
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
void triangle (Vec2i A, Vec2i B, Vec2i C, TGAImage &image, TGAColor color) {
    
    // add points into array
    Vec2i pts[] = {A, B, C};
    
    /* get upper and lower bounds of x and y to create bounding box*/
    // initialize with x/y of point A
    int x_bound[] = {A.x, A.x};
    int y_bound[] = {A.y, A.y};
    
    // loop through B and C looking for lesser/greater values of x/y
    // REWRITE TO LOOP THROUGH {B, C}, looks cleaner!!!
    for (Vec2i* ptr = pts + 1; ptr <= pts + 2; ptr++) {
        
        if (ptr->x < x_bound[0]) {
            x_bound[0] = ptr->x;
        } else if (ptr->x > x_bound[1]) {
            x_bound[1] = ptr->x;
        }
        
        if (ptr->y < y_bound[0]) {
            y_bound[0] = ptr->y;
        } else if (ptr->y > y_bound[1]) {
            y_bound[1] = ptr->y;
        }
    }
    
    std::cout << x_bound[0] << ", " << x_bound[1] << std::endl;
    std::cout << y_bound[0] << ", " << y_bound[1] << std::endl;
    
    for (int x = x_bound[0]; x < x_bound[1]; x++) {
        for (int y = y_bound[0]; y < y_bound[1]; y++) {
            
            Vec2i P(x, y);
            
            if (inside_triangle(pts, P)) {
                image.set(x, y, color);
            }
        }
    }
    
    return;
}

void triangle (Vec2i* pts, TGAImage& image, TGAColor color) {

    BoundingBox bbox(pts, image);
    
    for (int x = bbox.x_lower; x < bbox.x_upper; x++) {
        for (int y = bbox.y_lower; y < bbox.y_upper; y++) {
            
            Vec2i P(x, y);
            
            if (inside_triangle(pts, P)) {
                image.set(x, y, color);
            }
        }
    }
    
    return;
}

void triangle_z (Vec2i* pts, \
        Vec3f z_vals, std::vector<std::vector<float>>& z_buffer, \
        Model& model, Vec2f* tex_uv, \
        TGAImage& image, float intensity) {

    BoundingBox bbox(pts, image);
    
    for (int x = bbox.x_lower; x < bbox.x_upper; x++) {
        for (int y = bbox.y_lower; y < bbox.y_upper; y++) {
            
            Vec2i P(x, y);
            
            Vec3f bc = barycentric(pts, P);
            
            float z = z_interpolate(bc, z_vals);
            
            if (bc[2] >= 0) {
                if (z > (z_buffer[x][y])) {
                    z_buffer[x][y] = z;
                    Vec2f uv = uv_interpolate(bc, tex_uv);
                    TGAColor color_uv = model.diffuse(uv);
                    image.set(x, y, color_uv * intensity);
                }
            }
        }
    }
    
    return;
}