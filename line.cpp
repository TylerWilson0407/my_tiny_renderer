#include <utility>
#include "tgaimage.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    
    // Swap points if x0 > x1
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    const int dx = x1 - x0;
    int dy = y1 - y0;
    int yincr = 1;
    
    // Set y to decrement if slope is negative
    if (dy < 0) {
        dy = -dy;
        yincr = -1;
    }
    
    // Set steep bool, used to increment by y instead of x if slope > 1
    bool steep = false;
    if (dy > dx) {
        steep = true;
    }
    
    // error term
    int err = 0;
    
    if (!steep) {
        int y = y0;
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
        int x = x0;
        for (int y = y0; y <= y1; y++) {

            image.set(x, y, color);

            if (2 * (err + dx) < dy) {
                err += dx;
            } else {
                x += yincr;
                err += dx-dy;
            }

        }
    }
    return;
}