#include <algorithm>
#include <iostream>
#include <vector>
#include "drawing.h"
#include "tgaimage.h"

// utility functions

int cross2I(Point& v0, Point& v1) {
    /* cross product of 2D vectors with integer components.  Resulting vector 
 * points in z-direction.  Vectors represented as Points(vec = 0 -> point) */
    
    int k = (v0.x * v1.y) - (v0.y * v1.x);
    
    return k;
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
void triangle (Point A, Point B, Point C, TGAImage &image, TGAColor color) {
    
    // add points into array
    Point pts[] = {A, B, C};
    
    /* get upper and lower bounds of x and y to create bounding box*/
    // initialize with x/y of point A
    int x_bound[] = {A.x, A.x};
    int y_bound[] = {A.y, A.y};
    
    // loop through B and C looking for lesser/greater values of x/y
    // REWRITE TO LOOP THROUGH {B, C}, looks cleaner!!!
    for (Point* ptr = pts + 1; ptr <= pts + 2; ptr++) {
        
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
    
    /* make vectors(represented as Points) for AB and AC and put into array*/
    Point vecs[2];
    for (int i = 0; i < 2; i++) {
        vecs[i] = pts[i + 1] - pts[0];
    }
    
    // use instead of vecs array??
    Point AB = B - A;
    Point AC = C - A;
    
    int ABxAC = cross2I(vecs[0], vecs[1]);
    
    for (int x = x_bound[0]; x < x_bound[1]; x++) {
        for (int y = y_bound[0]; y < y_bound[1]; y++) {
            
            Point P(x, y);
            Point AP = P - A;
            
            /* Calculate cross products here(note that multiplied thru by ABxAC
             * as well).  Figure out best way to check for negative at each calc
             * and go to next iteration if so*/
        }
    }
    
    return;
}