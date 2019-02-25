/* 
 * File:   line.h
 * Author: tylerw
 *
 * Created on February 18, 2019, 12:54 PM
 */

#ifndef DRAWING_H
#define	DRAWING_H

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

struct BoundingBox {
    int x_lower;
    int x_upper;
    int y_lower;
    int y_upper;
    
    BoundingBox(Vec2i* points, TGAImage& image) {
        
        // initialize bounds to x/y of first point
        x_lower = points[0].x;
        x_upper = points[0].x;
        y_lower = points[0].y;
        y_upper = points[0].y;
        
        for (const Vec2i* point = points + 1; point <= points + 2; point++) {
        
            if (point->x < x_lower) {
                x_lower = std::max(point->x, 0);
            } else if (point->x > x_upper) {
                x_upper = std::min(point->x, image.get_width() - 1);
            }

            if (point->y < y_lower) {
                y_lower = std::max(point->y, 0);
            } else if (point->y > y_upper) {
                y_upper = std::min(point->y, image.get_height() - 1);
        }
    }
    }
};

// utility functions
//Vec3f perspective_transform(Vec3f vert, float c);
void perspective_transform(Vec3f& vert, float c);

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

#endif	/* DRAWING_H */

