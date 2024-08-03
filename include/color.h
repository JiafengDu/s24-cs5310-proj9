#ifndef COLOR_H

#define COLOR_H
#include "image.h"

typedef struct
{
    float c[3]; //rgb values
} Color;

void color_copy(Color *to, Color *from);
void color_set(Color *to, float r, float g, float b);
void image_setColor(Image *src, int r, int c, Color val);
Color image_getColor(Image *src, int r, int c);
Color color_scale(Color *c, float rScale, float gScale, float bScale);

#endif