/* 
 * File:   line.h
 * Author: tylerw
 *
 * Created on February 18, 2019, 12:54 PM
 */

#ifndef DRAWING_H
#define	DRAWING_H

#include "tgaimage.h"

// data structures
struct Vec2I {
    int x = 0;
    int y = 0;
    
    // constructor without parameters (initializes to (0, 0))
    Vec2I() : x(0), y(0) {};
    
    // constructor with parameters
    Vec2I(int x, int y) : x(x), y(y) {}
    
    // add operator overload for adding two points
    Vec2I operator+(const Vec2I& pnt) const {
        return Vec2I(x + pnt.x, y + pnt.y);
    }
    
    // subtract operator overload for subtracting two points
    Vec2I operator-(const Vec2I& pnt) const {
        return Vec2I(x - pnt.x, y - pnt.y);
    }
};

struct BoundingBox {
    int x_lower;
    int x_upper;
    int y_lower;
    int y_upper;
    
    BoundingBox(Vec2I* points) {
        
        // initialize bounds to x/y of first point
        x_lower = points[0].x;
        x_upper = points[0].x;
        y_lower = points[0].y;
        y_upper = points[0].y;
        
        for (const Vec2I* point = points + 1; point <= points + 2; point++) {
        
            if (point->x < x_lower) {
                x_lower = point->x;
            } else if (point->x > x_upper) {
                x_upper = point->x;
            }

            if (point->y < y_lower) {
                y_lower = point->y;
            } else if (point->y > y_upper) {
                y_upper = point->y;
        }
    }
    }
};

// utility functions

// line drawing functions
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void bresenham_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void horiz_line(int x0, int x1, int y, TGAImage &image, TGAColor color);
void vert_line(int x0, int x1, int y, TGAImage &image, TGAColor color);

// triangle drawing functions
void triangle(Vec2I p0, Vec2I p1, Vec2I p2, TGAImage &image, TGAColor color);
void triangle (Vec2I* pts, TGAImage& image, TGAColor color);

#endif	/* DRAWING_H */

