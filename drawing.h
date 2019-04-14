/* 
 * File:   line.h
 * Author: tylerw
 *
 * Created on February 18, 2019, 12:54 PM
 */

#ifndef DRAWING_H
#define	DRAWING_H

#include <cfloat>
#include <iostream>
#include "geometry.h"
#include "model.h"
//#include "render.h"
#include "tgaimage.h"

template <typename T> struct vec<4,T> {
    vec() : x(T()), y(T()), z(T()), w(T()) {}
    vec(T X, T Y, T Z) : x(X), y(Y), z(Z), w(1.f) {}
    vec(vec<3, T> V) : x(V.x), y(V.y), z(V.z), w(1.f) {}
    template <class U> vec<4,T>(const vec<4,U> &v);
          T& operator[](const size_t i) { 
              assert(i<4);
              return i<=0 ? x : (1==i ? y : (2==i ? z : w));
          }
    const T& operator[](const size_t i) const {
        assert(i<4);
        return i<=0 ? x : (1==i ? y : (2==i ? z : w));
    }

    T x,y,z,w;
};

Vec3f barycentric(const Vec3f* pts, const Vec2f& P);

struct BoundingBox {
    float x_lower;
    float x_upper;
    float y_lower;
    float y_upper;
    
    BoundingBox(Vec3f* points, TGAImage& image) {
        
        // initialize bounds to x/y of first point
        x_lower = FLT_MAX;
        x_upper = -FLT_MAX;
        y_lower = FLT_MAX;
        y_upper = -FLT_MAX;
        
        for (const Vec3f* point = points; point < points + 3; point++) {
        
            if (point->x < x_lower) {
                x_lower = std::max(point->x, 0.f);
            }
            if (point->x > x_upper) {
                x_upper = std::min(point->x, image.get_width() - 1.f);
            }
            if (point->y < y_lower) {
                y_lower = std::max(point->y, 0.f);
            }
            if (point->y > y_upper) {
                y_upper = std::min(point->y, image.get_height() - 1.f);
            }   
        }
    }
};



//// function to transform a vector by any number of matrices
//template<typename Vec3f, typename... Matrices>
//Vec3f transform_vertex(Vec3f vertex, Matrices&... matrices) {
//    Vec4f vert_homog = embed<4>(vertex);
//    vert_homog =  matr_mult(vert_homog, matrices...);
//    return proj<3>(vert_homog / vert_homog[3]);
//}
//
//template<typename Vec3f, typename... Matrices>
//Vec3f transform_vector(Vec3f vertex, Matrices&... matrices) {
//    Vec4f vert_homog = embed<4>(vertex, 0.f);
//    vert_homog =  matr_mult(vert_homog, matrices...);
//    return proj<3>(vert_homog);
//}
//
//// base case for recursive matr_mult function
//template<typename Vec4f>
//Vec4f matr_mult(Vec4f& vector) {
//    return proj<3>(vector / vector[3]);
//}
//
//// recursive matr_mult function
//template<typename Vec4f, typename Matrix, typename... Matrices>
//Vec4f matr_mult(Vec4f& vector, Matrix& matrix, Matrices&... matrices) {
//    vector = matrix * vector;
//    return matr_mult(vector, matrices...);
//}

template <class T>
T interpolate(Vec3f bary, T* vert_vals) {
    
    T result;
    
    for (int i = 0; i < 3; i++) {
        result = result + vert_vals[i] * bary[i];
    }
    
    return result;
}

inline TGAColor operator+(const TGAColor& c1, const TGAColor& c2) {
    
    TGAColor result(0, 0, 0, 0);
    
    for (int i = 0; i < 4; i++) {
        result.bgra[i] = c1.bgra[i] + c2.bgra[i];
    }
    
    return result;
}

// matrices
Matrix view_matrix(const Vec3f& from, const Vec3f& to, Vec3f& up);
Matrix perspective_matrix(float fov_x, float fov_y, float n, float f);
Matrix viewport(int l, int r, int b, int t);

// line drawing functions
void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color);
void line(Vec3f p0, Vec3f p1, TGAImage &image, TGAColor color);

// triangle drawing functions
void triangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage &image, TGAColor color);
void triangle(Vec2i* pts, TGAImage& image, TGAColor color);
void triangle_z(Vec2i* pts, \
        Vec3f z_vals, std::vector<std::vector<float>>& z_buffer, \
        Model&, Vec2f* tex_uv, \
        TGAImage& image, float intensity);
void triangle_mat (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, Vec2f* tex_uv, \
        TGAImage& image, float intensity);
void triangle_flat (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, const int& i_face, \
        TGAImage& image, float intensity);
void triangle_gouraud (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, const int& i_face, \
        TGAImage& image, const Vec3f& light_vec, \
        Matrix& viewmat);
void triangle_phong (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, const int& i_face, \
        TGAImage& image, const Vec3f& light_vec, \
        Matrix& viewmat);
void triangle_darboux (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, const int& i_face, \
        TGAImage& image, const Vec3f& light_vec, \
        Matrix& viewmat);

#endif	/* DRAWING_H */

