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
struct Point {
    int x = 0;
    int y = 0;
    
    // constructor without parameters (initializes to (0, 0))
    Point() : x(0), y(0) {};
    
    // constructor with parameters
    Point(int x, int y) : x(x), y(y) {}
    
    // add operator overload for adding two points
    Point operator+(const Point& pnt) const {
        return Point(x + pnt.x, y + pnt.y);
    }
    
    // subtract operator overload for subtracting two points
    Point operator-(const Point& pnt) const {
        return Point(x - pnt.x, y - pnt.y);
    }
};

// utility functions
bool comp_point_y(Point& p0, Point& p1);


// line drawing functions
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void bresenham_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void horiz_line(int x0, int x1, int y, TGAImage &image, TGAColor color);
void vert_line(int x0, int x1, int y, TGAImage &image, TGAColor color);

// triangle drawing functions
void triangle(Point p0, Point p1, Point p2, TGAImage &image, TGAColor color);

#endif	/* DRAWING_H */

