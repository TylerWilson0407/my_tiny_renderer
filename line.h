/* 
 * File:   line.h
 * Author: tylerw
 *
 * Created on February 18, 2019, 12:54 PM
 */

#ifndef LINE_H
#define	LINE_H

#include "tgaimage.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

void bresenham_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

#endif	/* LINE_H */

