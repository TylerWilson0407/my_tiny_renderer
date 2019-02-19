#include <algorithm>
#include <vector>
#include "drawing.h"
#include "tgaimage.h"

// Utility functions
bool comp_point_x(Point& p0, Point& p1) {
    return (p0.x < p1.x);
}

// Data structures
//struct Point {};

// Line drawing functions
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
    
    // Set steep bool if slope > 1 and swap x/y if so
    bool steep = (std::abs(y1 - y0) > std::abs(x1 - x0)) ? true : false;
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    
    // Swap points if x0 > x1
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    const int dx = x1 - x0;
    int dy = y1 - y0;
    int y = y0;
    int yincr = 1;
    int err = 0;
    
    // Set y to decrement if slope is negative
    if (dy < 0) {
        dy = -dy;
        yincr = -1;
    }
    
    /* Transpose line (image.set(y, x, color) instead of (x, y, color) if steep 
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

// Triangle drawing functions
void triangle_v0 (Point p0, Point p1, Point p2, TGAImage &image, TGAColor color) {
    
    // add points to vector and sort by increasing x-value
    std::vector<Point> pts {p0, p1, p2};
    std::sort(pts.begin(), pts.end(), comp_point_x);
    
    for (int x = pts[0].x; x < pts[1].x; x++) {
        
    }
    
    return;
}