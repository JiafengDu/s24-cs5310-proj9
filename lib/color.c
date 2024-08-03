/***
 * writen by - Jiafeng Du
 *
 * color APIs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "color.h"

/***
 * copies the Color data.
 */
void color_copy(Color *to, Color *from)
{
    to->c[0] = from->c[0];
    to->c[1] = from->c[1];
    to->c[2] = from->c[2];
}

/***
 * sets the Color data.
 */
void color_set(Color *to, float r, float g, float b)
{
    to->c[0] = r;
    to->c[1] = g;
    to->c[2] = b;
}

/***
 * copies the Color data to the proper pixel.
 */
void image_setColor(Image *src, int r, int c, Color val)
{
    if (0>r || 0>c) {
        return;
    }
    if (src->cols <= c || src->rows <= r) {
        return;
    }
    src->data[r][c].rgb[0] = val.c[0];
    src->data[r][c].rgb[1] = val.c[1];
    src->data[r][c].rgb[2] = val.c[2];
}

/***
 * returns a Color structure built from the pixel values.
 * return NULL if r, c out of bound.
 */
Color image_getColor(Image *src, int r, int c)
{
    Color color = {{-1.0, -1.0, -1.0}};
    if (r < 0 || r >= src->rows)
    {
        return color;
    }
    else if (c < 0 || c >= src->cols)
    {
        return color;
    }
    color.c[0] = src->data[r][c].rgb[0];
    color.c[1] = src->data[r][c].rgb[1];
    color.c[2] = src->data[r][c].rgb[2];
    return color;
}

/***
 * returns a Color structure scaled using the 3 factors
 */
Color color_scale(Color *c, float rScale, float gScale, float bScale) {
    Color color;
    color_set(&color, c->c[0]*rScale, c->c[1]*gScale, c->c[2]*bScale);
    return color;
}