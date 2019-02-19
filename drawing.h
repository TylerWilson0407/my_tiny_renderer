/* 
 * File:   line.h
 * Author: tylerw
 *
 * Created on February 18, 2019, 12:54 PM
 */

#ifndef DRAWING_H
#define	DRAWING_H

#include "tgaimage.h"

// Data structures
struct Point {
    int x;
    int y;
    Point(int x, int y) : x(x), y(y) {}
};

// Utility functions
bool comp_point_x(Point& p0, Point& p1);

// Line drawing functions
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void bresenham_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void horiz_line(int x0, int x1, int y, TGAImage &image, TGAColor color);
void vert_line(int x0, int x1, int y, TGAImage &image, TGAColor color);

// Triangle drawing functions
//void triangle

#endif	/* DRAWING_H */

