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
#include "tgaimage.h"

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
        
        x_lower = std::max(x_lower, 0.f);
        x_upper = std::min(x_upper, image.get_width() - 1.f);
        y_lower = std::max(y_lower, 0.f);
        y_upper = std::min(y_upper, image.get_height() - 1.f);
    }
};

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

struct Homogeneous : public Vec4f
{
    explicit Homogeneous(const Vec3f& v, float vw=1) : Vec4f() {
        x = v.x;
        y = v.y;
        z = v.z;
        w = vw;
    };
};

struct Cartesian : public Vec3f
{
    explicit Cartesian(const Vec4f& v) : Vec3f() {
        x = v.x / v.w;
        y = v.y / v.w;
        z = v.z / v.w;
    }
};

// interpolate template
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
Matrix viewport_matrix(int l, int r, int b, int t);

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
void triangle (Vec3f* pts, \
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

