/* 
 * File:   line.h
 * Author: tylerw
 *
 * Created on February 18, 2019, 12:54 PM
 */

#ifndef DRAWING_H
#define	DRAWING_H

#include <cfloat>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

struct BoundingBox {
    float x_lower;
    float x_upper;
    float y_lower;
    float y_upper;
    
    BoundingBox(Vec3f* points, TGAImage& image) {
        
        // initialize bounds to x/y of first point
        x_lower = FLT_MAX;
        x_upper = points[0].x;
        y_lower = points[0].y;
        y_upper = points[0].y;
        
        for (const Vec3f* point = points; point <= points + 2; point++) {
        
            if (point->x < x_lower) {
                x_lower = std::max(point->x, 0.f);
            } else if (point->x > x_upper) {
                x_upper = std::min(point->x, image.get_width() - 1.f);
            }

            if (point->y < y_lower) {
                y_lower = std::max(point->y, 0.f);
            } else if (point->y > y_upper) {
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
    explicit Homogeneous(const Vec3f& v) : Vec4f() {
        x = v.x;
        y = v.y;
        z = v.z;
        w = 1.f;
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

// matrices
Matrix view_matrix(const Vec3f& from, const Vec3f& to, Vec3f& up);
Matrix perspective_matrix(float fov_x, float fov_y, float n, float f);
Matrix viewport_matrix(int l, int r, int b, int t);

// line drawing functions
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void bresenham_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void horiz_line(int x0, int x1, int y, TGAImage &image, TGAColor color);
void vert_line(int x0, int x1, int y, TGAImage &image, TGAColor color);

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
void triangle_fp (Vec3f* pts, \
        std::vector<std::vector<float>>& z_buffer, \
        Model& model, Vec2f* tex_uv, \
        TGAImage& image, float intensity);

#endif	/* DRAWING_H */

