/***
 * written by - Jiafeng
 *
 * graphics primitive - polygon apis
 */

#include "graphics.h"

/* Point functions */

/***
 * set the first two values of the vector to x and y.
 * Set the third value to 0.0 and the fourth value to 1.0.
 */
void point_set2D(Point *p, double x, double y)
{
    p->val[0] = x;
    p->val[1] = y;
    p->val[2] = 0.0;
    p->val[3] = 1.0;
}

/***
 * set the point’s values to x and y and z. Set the homogeneous coordinate to 1.0.
 */
void point_set3D(Point *p, double x, double y, double z)
{
    p->val[0] = x;
    p->val[1] = y;
    p->val[2] = z;
    p->val[3] = 1.0;
}

/***
 * set the four values of the vector to x, y, z, and h, respectively.
 */
void point_set(Point *p, double x, double y, double z, double h)
{
    p->val[0] = x;
    p->val[1] = y;
    p->val[2] = z;
    p->val[3] = h;
}

/***
 * normalize the x and y values of a point by its homogeneous coordinate: x = x/h, y = y/h.
 */
void point_normalize(Point *p)
{
    p->val[0] = p->val[0] / p->val[3];
    p->val[1] = p->val[1] / p->val[3];
    p->val[3] = 1.0;
}

/***
 * copy the point data structure.
 */
void point_copy(Point *to, Point *from)
{
    if (from) {
        to->val[0] = from->val[0];
        to->val[1] = from->val[1];
        to->val[2] = from->val[2];
        to->val[3] = from->val[3];
    } else {
        to = NULL;
    }
}

/***
 * draw the point into src using color c.
 */
void point_draw(Point *p, Image *src, Color c)
{
    int row = p->val[1], col = p->val[0];
    src->data[row][col].rgb[0] = c.c[0];
    src->data[row][col].rgb[1] = c.c[1];
    src->data[row][col].rgb[2] = c.c[2];
}

/***
 * draw the p into src using FPixel c.
 */
void point_drawf(Point *p, Image *src, FPixel c)
{
    int row = p->val[1], col = p->val[0];
    src->data[row][col] = c;
}

/***
 * use fprintf to print the contents of the Point to the stream fp.
 * The FILE pointer fp can be an actual file opened using fopen or
 * a standard output stream like stdout or stderr, both defined in stdio.h.
 * The point print function doesn’t care which it is, it just uses fprintf,
 * which takes a FILE pointer as the first argument.
 */
void point_print(Point *p, FILE *fp)
{
    fprintf(fp, " ( %f %f %f %f )\n", p->val[0], p->val[1], p->val[2], p->val[3]);
}

/* Line functions */

/***
 * initialize a 2D line.
 */
void line_set2D(Line *l, double x0, double y0, double x1, double y1)
{
    point_set2D(&l->a, x0, y0);
    point_set2D(&l->b, x1, y1);
    l->zBuffer = 1;
}

/***
 * initialize a line to ta and tb.
 */
void line_set(Line *l, Point ta, Point tb)
{
    l->a = ta;
    l->b = tb;
    l->zBuffer = 1;
}

/***
 * set the z-buffer flag to the given value.
 */
void line_zBuffer(Line *l, int flag)
{
    l->zBuffer = flag;
}

/***
 * normalize the x and y values of the endpoints by their homogeneous coordinate.
 */
void line_normalize(Line *l)
{
    Point *a = &l->a, *b = &l->b;
    point_normalize(a);
    point_normalize(b);
}

/***
 * copy the line data structure.
 */
void line_copy(Line *to, Line *from)
{
    Point a = from->a, b = from->b;
    line_set(to, a, b);
}


void _line_clipping(Line *l, Image *src, Line *to);
/***
 * Liang-Barsky/Cyrus-Beck clipping
 */
void _line_clipping(Line *l, Image *src, Line *to) {
    Point start, end;
    Vector v;
    vector_set(&v, l->b.val[0]-l->a.val[0], l->b.val[1]-l->a.val[1], l->b.val[2]-l->a.val[2]);
    int t0=0, tf=1;
    int p, q;
    double t;
    //finding bounding min, max
    Point boundMin, boundMax;
    point_set3D(&boundMin, 0.0, 0.0, 0.0);
    point_set3D(&boundMax, src->cols, src->rows, src->depth[0][0]);
    //updating t0, tf
    for (int i=0; i<4; i++) {
        if (i/2+i/2 < i) { //i = 1, 3 meanning p2, p4
            p = v.val[i/2];
            q = boundMax.val[i/2] - l->a.val[i/2];
        } else { //i = 0, 2 meanning p1, p3
            p = -v.val[i/2];
            q = l->a.val[i/2] - boundMin.val[i/2];
        }
        if (p==0 && q<0) return;
        t = (float)q/p;
        if (p<0) {
            t0 = fmax(t0, t);
        } else {
            tf = fmin(tf, t);
        }
        if (t0>tf) return;
    }
    point_set3D(&start, l->a.val[0]+t0*v.val[0], l->a.val[1]+t0*v.val[1], l->a.val[2]+t0*v.val[2]);
    point_set3D(&end, l->a.val[0]+tf*v.val[0], l->a.val[1]+tf*v.val[1], l->a.val[2]+tf*v.val[2]);
    line_set(to, start, end);
}

void _line_draw(Line *l, Image *src, Color c);
/***
 * helper function to line_draw
 */
void _line_draw(Line *l, Image *src, Color c)
{
    int x, y, dx, dy, adx, ady, px, py, xe, ye;
    int x1 = (int)l->a.val[0] + 0.5, y1 = (int)l->a.val[1] + 0.5;
    int x2 = (int)l->b.val[0] + 0.5, y2 = (int)l->b.val[1] + 0.5;
    float delta = 1/l->b.val[2] - 1/l->a.val[2], zbuffer;
    dx = x2 - x1;
    dy = y2 - y1;
    adx = abs(dx);
    ady = abs(dy);
    int two_ady = ady + ady;
    int two_adx = adx + adx;
    px = 3 * ady - 2 * adx;
    py = 3 * adx - 2 * ady;
    if (ady <= adx) // octant 1, 4, 5, 8
    {
        delta /= dx;
        if (dx >= 0)
        {
            x = x1, y = y1, xe = x2;
        }
        else
        {
            x = x2, y = y2, xe = x1;
        }
        zbuffer = 1/l->a.val[2];
        if (zbuffer>=image_getz(src, y, x)) {
            image_setColor(src, y, x, c);
            image_setz(src, y, x, zbuffer);
        }
        while (x < xe)
        {
            zbuffer += delta;
            x++;
            if (px < 0)
            {
                px += two_ady;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) // octant 1, 5
                {
                    y++;
                }
                else // octant 4, 8
                {
                    y--;
                }
                px += two_ady - two_adx;
            }
            if (zbuffer>=image_getz(src, y, x)) {
                image_setColor(src, y, x, c);
                image_setz(src, y, x, zbuffer);
            }
        }
    }
    else // octant 2, 3, 6, 7
    {
        delta /= dy;
        if (dy >= 0)
        {
            x = x1, y = y1, ye = y2;
        }
        else
        {
            x = x2, y = y2, ye = y1;
        }
        zbuffer = 1/l->a.val[2];
        if (zbuffer>=image_getz(src, y, x)) {
            image_setColor(src, y, x, c);
            image_setz(src, y, x, zbuffer);
        }
        while (y < ye)
        {
            zbuffer += delta;
            y++;
            if (py < 0)
            {
                py += two_adx;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) // octant 2, 6
                {
                    x++;
                }
                else // octant 3, 7
                {
                    x--;
                }
                py += two_adx - two_ady;
            }
            if (zbuffer>=image_getz(src, y, x)) {
                image_setColor(src, y, x, c);
                image_setz(src, y, x, zbuffer);
            }
        }
    }
}

/***
 * draw the line into src using color c and the z-buffer, if appropriate.
 * reference:
 * https://ghost-together.medium.com/how-to-code-your-first-algorithm-draw-a-line-ca121f9a1395
 */
void line_draw(Line *l, Image *src, Color c) {
    Line g;
    _line_clipping(l, src, &g);
    _line_draw(&g, src, c);
}

/* Circle functions */

/***
 * initialize to center tc and radius tr.
 */
void circle_set(Circle *c, Point tc, double tr)
{
    c->c = tc;
    c->r = tr;
}

/***
 * draw the circle into src using color p.
 */
void circle_draw(Circle *c, Image *src, Color p)
{
    int x, y, e, cx, cy, radius;
    cx = (int)c->c.val[0] + 0.5, cy = (int)c->c.val[1] + 0.5;
    radius = (int)c->r + 0.5;
    x = -1, y = -radius, e = 1 - radius;
    image_setColor(src, y + cy, x + cx, p);
    image_setColor(src, y + cy, -x - 1 + cx, p);
    image_setColor(src, x + cy, -y - 1 + cx, p);
    image_setColor(src, -x - 1 + cy, -y - 1 + cx, p);
    image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
    image_setColor(src, -y - 1 + cy, x + cx, p);
    image_setColor(src, -x - 1 + cy, y + cx, p);
    image_setColor(src, x + cy, y + cx, p);
    while (x > y)
    {
        x--;
        if (e < 0)
        {
            e += 1 - x - x;
        }
        else
        {
            y++;
            e += 1 - 2 * (x - y);
        }
        image_setColor(src, y + cy, x + cx, p);
        image_setColor(src, y + cy, -x - 1 + cx, p);
        image_setColor(src, x + cy, -y - 1 + cx, p);
        image_setColor(src, -x - 1 + cy, -y - 1 + cx, p);
        image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
        image_setColor(src, -y - 1 + cy, x + cx, p);
        image_setColor(src, -x - 1 + cy, y + cx, p);
        image_setColor(src, x + cy, y + cx, p);
    }
}

/***
 * draw a filled circle into src using color p.
 */
void circle_drawFill(Circle *c, Image *src, Color p)
{
    int x, y, e, cx, cy, radius;
    cx = (int)c->c.val[0] + 0.5, cy = (int)c->c.val[1] + 0.5;
    radius = (int)c->r + 0.5;
    x = -1, y = -radius, e = 1 - radius;
    image_setColor(src, y + cy, x + cx, p);
    image_setColor(src, y + cy, -x - 1 + cx, p);
    for (int i = x; i < -x - 1; i++)
    {
        image_setColor(src, y + cy, i + cx, p);
    }
    image_setColor(src, x + cy, -y - 1 + cx, p);
    image_setColor(src, x + cy, y + cx, p);
    for (int i = y; i < -y - 1; i++)
    {
        image_setColor(src, x + cy, i + cx, p);
    }
    image_setColor(src, -x - 1 + cy, -y - 1 + cx, p);
    image_setColor(src, -x - 1 + cy, y + cx, p);
    for (int i = y; i < -y - 1; i++)
    {
        image_setColor(src, -x - 1 + cy, i + cx, p);
    }
    image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
    image_setColor(src, -y - 1 + cy, x + cx, p);
    for (int i = x; i < -x - 1; i++)
    {
        image_setColor(src, -y - 1 + cy, i + cx, p);
    }

    while (x > y)
    {
        x--;
        if (e < 0)
        {
            e += 1 - x - x;
        }
        else
        {
            y++;
            e += 1 - 2 * (x - y);
        }
        image_setColor(src, y + cy, x + cx, p);
        image_setColor(src, y + cy, -x - 1 + cx, p);
        for (int i = x; i < -x - 1; i++)
        {
            image_setColor(src, y + cy, i + cx, p);
        }
        image_setColor(src, x + cy, -y - 1 + cx, p);
        image_setColor(src, x + cy, y + cx, p);
        for (int i = y; i < -y - 1; i++)
        {
            image_setColor(src, x + cy, i + cx, p);
        }
        image_setColor(src, -x - 1 + cy, -y - 1 + cx, p);
        image_setColor(src, -x - 1 + cy, y + cx, p);
        for (int i = y; i < -y - 1; i++)
        {
            image_setColor(src, -x - 1 + cy, i + cx, p);
        }
        image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
        image_setColor(src, -y - 1 + cy, x + cx, p);
    }
}

/* Eclipse functions */

/***
 * nitialize an ellipse to location tc and radii ta and tb.
 */
void ellipse_set(Ellipse *e, Point tc, double ta, double tb)
{
    e->ra = ta;
    e->rb = tb;
    e->c = tc;
}

/***
 * draw into src using color p.
 */
void ellipse_draw(Ellipse *e, Image *src, Color color)
{
    int x, y, cx, cy, rx, ry, px, py, prx, pry, p;
    cx = (int)e->c.val[0] + 0.5, cy = (int)e->c.val[1] + 0.5;
    rx = (int)e->ra + 0.5, ry = (int)e->rb + 0.5; // assume e->a is 0 radian
    x = -1, y = -ry;
    pry = ry * ry;
    prx = rx * rx;
    px = 2 * pry;
    py = 2 * prx * (-y);
    image_setColor(src, y + cy, x + cx, color);
    image_setColor(src, y + cy, -x - 1 + cx, color);
    image_setColor(src, -y - 1 + cy, x + cx, color);
    image_setColor(src, -y - 1 + cy, -x - 1 + cx, color);
    p = 2 * pry - prx * ry + prx / 4 + px;

    while (px < py)
    {
        x--;
        px += 2 * pry;
        if (p < 0)
        {
            p += px + pry;
        }
        else
        {
            y++;
            py += -2 * prx;
            p += px - py + pry;
        }
        image_setColor(src, y + cy, x + cx, color);
        image_setColor(src, y + cy, -x - 1 + cx, color);
        image_setColor(src, -y - 1 + cy, x + cx, color);
        image_setColor(src, -y - 1 + cy, -x - 1 + cx, color);
    }
    p += pry * (x * x + x) + prx * (y * y - 2 * y + 1) - prx * pry + prx - py;
    while (y < 0)
    {
        y++;
        py += -2 * prx;
        if (p < 0)
        {
            p += prx - py;
        }
        else
        {
            x--;
            px += 2 * pry;
            p += prx - py + px;
        }
        image_setColor(src, y + cy, x + cx, color);
        image_setColor(src, y + cy, -x - 1 + cx, color);
        image_setColor(src, -y - 1 + cy, x + cx, color);
        image_setColor(src, -y - 1 + cy, -x - 1 + cx, color);
    }
}

/***
 * draw a filled ellipse into src using color p.
 */
void ellipse_drawFill(Ellipse *e, Image *src, Color color)
{
    int x, y, cx, cy, rx, ry, px, py, prx, pry, p;
    cx = (int)e->c.val[0] + 0.5, cy = (int)e->c.val[1] + 0.5;
    rx = (int)e->ra + 0.5, ry = (int)e->rb + 0.5; // assume e->a is 0 radian
    x = -1, y = -ry;
    pry = ry * ry;
    prx = rx * rx;
    px = 2 * pry;
    py = 2 * prx * (-y);
    image_setColor(src, y + cy, x + cx, color);
    image_setColor(src, y + cy, -x - 1 + cx, color);
    image_setColor(src, -y - 1 + cy, x + cx, color);
    image_setColor(src, -y - 1 + cy, -x - 1 + cx, color);
    for (int i = x + 1; i < -x - 1; i++)
    {
        image_setColor(src, y + cy, i + cx, color);
        image_setColor(src, -y - 1 + cy, i + cx, color);
    }
    p = 2 * pry - prx * ry + prx / 4 + px;

    while (px < py)
    {
        x--;
        px += 2 * pry;
        if (p < 0)
        {
            p += px + pry;
        }
        else
        {
            y++;
            py += -2 * prx;
            p += px - py + pry;
        }
        image_setColor(src, y + cy, x + cx, color);
        image_setColor(src, y + cy, -x - 1 + cx, color);
        image_setColor(src, -y - 1 + cy, x + cx, color);
        image_setColor(src, -y - 1 + cy, -x - 1 + cx, color);
        for (int i = x + 1; i < -x - 1; i++)
        {
            image_setColor(src, y + cy, i + cx, color);
            image_setColor(src, -y - 1 + cy, i + cx, color);
        }
    }
    p += pry * (x * x + x) + prx * (y * y - 2 * y + 1) - prx * pry + prx - py;
    while (y < 0)
    {
        y++;
        py += -2 * prx;
        if (p < 0)
        {
            p += prx - py;
        }
        else
        {
            x--;
            px += 2 * pry;
            p += prx - py + px;
        }
        image_setColor(src, y + cy, x + cx, color);
        image_setColor(src, y + cy, -x - 1 + cx, color);
        image_setColor(src, -y - 1 + cy, x + cx, color);
        image_setColor(src, -y - 1 + cy, -x - 1 + cx, color);
        for (int i = x + 1; i < -x - 1; i++)
        {
            image_setColor(src, y + cy, i + cx, color);
            image_setColor(src, -y - 1 + cy, i + cx, color);
        }
    }
}

/* Polyline functions */

/***
 * returns an allocated Polyline pointer initialized so that numVertex is 0 and vertex is NULL.
 */
Polyline *polyline_create(void)
{
    Polyline *poly = (Polyline *)malloc(sizeof(Polyline));
    poly->numVertex = 0;
    poly->vertex = NULL;
    poly->zBuffer = 1;
    return poly;
}

/***
 * returns an allocated Polyline pointer with the vertex list initialized to the points in vlist.
 */
Polyline *polyline_createp(int numV, Point *vlist)
{
    Polyline *poly = (Polyline *)malloc(sizeof(Polyline));
    poly->zBuffer = 1;
    poly->numVertex = numV;
    poly->vertex = (Point *)malloc(sizeof(Point) * numV);
    for (int i = 0; i < numV; i++)
    {
        point_copy(&poly->vertex[i], &vlist[i]);
    }
    return poly;
}

/***
 * frees the internal data and the Polyline pointer.
 */
void polyline_free(Polyline *p)
{
    if (p == NULL)
    {
        return;
    }
    else if (p->vertex == NULL)
    {
        free(p);
        return;
    }
    else
    {
        free(p->vertex);
        free(p);
        return;
    }
}

/***
 * initializes the pre-existing Polyline to an empty Polyline.
 */
void polyline_init(Polyline *p)
{
    p->numVertex = 0;
    p->vertex = NULL;
    p->zBuffer = 1;
}

/***
 * initializes the vertex list to the points in vlist. De-allocates/allocates the vertex list for p, as necessary.
 */
void polyline_set(Polyline *p, int numV, Point *vlist)
{
    if (p->vertex != NULL)
    {
        free(p->vertex);
    }
    p->numVertex = numV;
    p->vertex = (Point *)malloc(sizeof(Point) * numV);
    for (int i = 0; i < numV; i++)
    {
        point_copy(&p->vertex[i], &vlist[i]);
    }
}

/***
 * frees the internal data for a Polyline, if necessary, and sets numVertex to 0 and vertex to NULL.
 */
void polyline_clear(Polyline *p)
{
    if (p->vertex != NULL)
    {
        free(p->vertex);
    }
    p->numVertex = 0;
    p->vertex = NULL;
    p->zBuffer = 1;
}

/***
 * sets the z-buffer flag to the given value.
 */
void polyline_zBuffer(Polyline *p, int flag)
{
    p->zBuffer = flag;
}

/***
 * De-allocates/allocates space as necessary in the destination Polyline data structure
 * and copies the vertex data from the source polyline (from) to the destination (to).
 */
void polyline_copy(Polyline *to, Polyline *from)
{
    polyline_set(to, from->numVertex, from->vertex);
    polyline_zBuffer(to, from->zBuffer);
}

/***
 * prints Polyline data to the stream designated by the FILE pointer.
 */
void polyline_print(Polyline *p, FILE *fp)
{
    fprintf(fp, "polyline number of vertices: %d\n", p->numVertex);
    fprintf(fp, "polyline zBuffer: %d\n", p->zBuffer);
    for (int i = 0; i < p->numVertex; i++)
    {
        fprintf(fp, "#%d point: (%f, %f)\n", i, p->vertex[i].val[0], p->vertex[i].val[1]);
    }
}

/***
 * normalize the x and y values of each vertex by the homogeneous coordinate.
 */
void polyline_normalize(Polyline *p)
{
    for (int i = 0; i < p->numVertex; i++)
    {
        point_normalize(&p->vertex[i]);
    }
}

/***
 * draw the polyline using color c and the z-buffer, if appropriate.
 */
void polyline_draw(Polyline *p, Image *src, Color c)
{
    Line l;
    for (int i = 1; i < p->numVertex; i++)
    {
        line_set(&l, p->vertex[i - 1], p->vertex[i]);
        line_zBuffer(&l, p->zBuffer);
        line_draw(&l, src, c);
    }
}


/***
 * scanline flood fill function which should not really be here as its not a primitive, but for now, I will put it here
 * f - color to fill in the space
 * (x, y) - the pixel you select
 */
void fill(Image *src, Color f, double pixelx, double pixely)
{
    Stack *stack = stack_create();
    int pixel[2] = {(int)(pixelx + 0.5), (int)(pixely + 0.5)};
    int nextPixel[2];
    int *p;
    Color o = image_getColor(src, pixel[1], pixel[0]);

    // Return if the initial pixel's color is the same as the fill color
    if (o.c[0] == f.c[0] && o.c[1] == f.c[1] && o.c[2] == f.c[2])
    {
        stack_free(stack);
        return;
    }

    image_setColor(src, pixel[1], pixel[0], f);
    stack_push(stack, pixel, sizeof(pixel));

    while (stack->element_size != 0)
    {
        p = (int *)stack_pop(stack); // Cast the void* returned by stack_pop to int*
        if (!p)
            continue; // Check if stack_pop returned null

        Color currentColor = image_getColor(src, p[1], p[0]);

        // Move to the left edge of the segment
        while (p[0] >= 0 && currentColor.c[0] == o.c[0] && currentColor.c[1] == o.c[1] && currentColor.c[2] == o.c[2])
        {
            p[0]--;
            if (p[0] >= 0)
            {
                currentColor = image_getColor(src, p[1], p[0]);
            }
        }
        p[0]++; // Move back to the first pixel inside the segment
        if (p[0] < 0)
            continue; // Ensure p[0] is within bounds

        currentColor = image_getColor(src, p[1], p[0]);
        // Fill the segment and push new pixels onto the stack
        while (p[0] < src->cols && currentColor.c[0] == o.c[0] && currentColor.c[1] == o.c[1] && currentColor.c[2] == o.c[2])
        {
            image_setColor(src, p[1], p[0], f);

            // Check the pixel above
            nextPixel[0] = p[0];
            nextPixel[1] = p[1] + 1;
            if (nextPixel[1] < src->rows)
            {
                currentColor = image_getColor(src, nextPixel[1], nextPixel[0]);
                if (currentColor.c[0] == o.c[0] && currentColor.c[1] == o.c[1] && currentColor.c[2] == o.c[2])
                {
                    int newPixel[2] = {nextPixel[0], nextPixel[1]};
                    stack_push(stack, newPixel, sizeof(newPixel));
                }
            }

            // Check the pixel below
            nextPixel[0] = p[0];
            nextPixel[1] = p[1] - 1;
            if (nextPixel[1] >= 0)
            {
                currentColor = image_getColor(src, nextPixel[1], nextPixel[0]);
                if (currentColor.c[0] == o.c[0] && currentColor.c[1] == o.c[1] && currentColor.c[2] == o.c[2])
                {
                    int newPixel[2] = {nextPixel[0], nextPixel[1]};
                    stack_push(stack, newPixel, sizeof(newPixel));
                }
            }

            p[0]++;
            if (p[0] < src->cols)
            {
                currentColor = image_getColor(src, p[1], p[0]);
            }
        }

        free(p); // Free the popped pixel array
    }
    stack_free(stack);
}

/********************
Scanline Fill Algorithm
********************/

// define the struct here, because it is local to only this file
typedef struct tEdge
{
    float x0, y0, z0;                /* start point for the edge */
    float x1, y1, z1;                /* end point for the edge */
    int yStart, yEnd;            /* start row and end row */
    float xIntersect, dxPerScan; /* where the edge intersects the current scanline and how it changes */
    float zIntersect, dzPerScan; /* where the edge intersects the current scanline */
    Color c0, c1; /* the color of the edge */
    Color cIntersect, dcPerScan; /* where the edge intersects the current scanline */
    struct tEdge *next;
} Edge;

/*
    This is a comparison function that returns a value < 0 if a < b, a
    value > 0 if a > b, and 0 if a = b.  It uses the yStart field of the
    Edge structure.  It is used to sort the overall edge list.
 */
static int compYStart(const void *a, const void *b)
{
    Edge *ea = (Edge *)a;
    Edge *eb = (Edge *)b;

    return (ea->yStart - eb->yStart);
}

/*
    This is a comparison function that returns a value < 0 if a < b, a
    value > 0 if a > b, and 0 if a = b.  It uses the xIntersect field of the
    Edge structure.  It is used to sort the active edge list.
 */
static int compXIntersect(const void *a, const void *b)
{
    Edge *ea = (Edge *)a;
    Edge *eb = (Edge *)b;
    if (ea->xIntersect < eb->xIntersect)
        return (-1);
    else if (ea->xIntersect > eb->xIntersect)
        return (1);

    return (0);
}

/*
    Allocates, creates, fills out, and returns an Edge structure given
    the inputs.

    Current inputs are just the start and end location in image space.
    Eventually, the points will be 3D and we'll add color and texture
    coordinates.
 */
static Edge *makeEdgeRec(Point start, Point end, Color *c0, Color *c1, Image *src) {
    Edge *edge;
    float dscan = end.val[1] - start.val[1];

    // Check if the starting row is below the image or the end row is
    // above the image and skip the edge if either is true
    if (start.val[1] < 0 || end.val[1] > src->rows)
    {
        return NULL;
    }

    // allocate an edge structure and set the x0, y0, x1, y1 values
    edge = malloc(sizeof(Edge));
    edge->x0 = start.val[0];
    edge->y0 = start.val[1];
    edge->z0 = start.val[2];
    edge->x1 = end.val[0];
    edge->y1 = end.val[1];
    edge->z1 = end.val[2];
    edge->c0 = *c0;
    edge->c1 = *c1;

    // turn on an edge only if the edge starts in the top half of it or
    // the lower half of the pixel above it.  In other words, round the
    // start y value to the nearest integer and assign it to
    // edge->yStart.
    edge->yStart = (int)(edge->y0 + 0.5);

    // Turn off the edge if it starts in the lower half or the top half
    // of the next pixel.  In other words, round the end y value to the
    // nearest integer and subtract 1, assigning the result to
    // edge->yEnd.
    edge->yEnd = (int)(edge->y1 + 0.5) - 1;

    // Clip yEnd to the number of rows-1.
    if (edge->yEnd > src->rows - 1)
    {
        edge->yEnd = src->rows - 1;
    }

    edge->dxPerScan = (edge->x1 - edge->x0) / dscan;
    edge->dzPerScan = (1/edge->z1 - 1/edge->z0) / dscan;
    edge->dcPerScan.c[0] = (edge->c1.c[0]/edge->z1 - edge->c0.c[0]/edge->z0) / dscan;
    edge->dcPerScan.c[1] = (edge->c1.c[1]/edge->z1 - edge->c0.c[1]/edge->z0) / dscan;
    edge->dcPerScan.c[2] = (edge->c1.c[2]/edge->z1 - edge->c0.c[2]/edge->z0) / dscan;
    
    if ((edge->y0 - (int)edge->y0) <= 0.5) {
        edge->xIntersect = edge->x0 + (0.5 - (edge->y0 - (int)edge->y0)) * edge->dxPerScan;
        edge->zIntersect = 1/edge->z0 + (0.5 - (edge->y0 - (int)edge->y0)) * edge->dzPerScan;

        edge->cIntersect.c[0] = edge->c0.c[0]/edge->z0 + (0.5 - (edge->y0 - (int)edge->y0)) * edge->dcPerScan.c[0];
        edge->cIntersect.c[1] = edge->c0.c[1]/edge->z0 + (0.5 - (edge->y0 - (int)edge->y0)) * edge->dcPerScan.c[1];
        edge->cIntersect.c[2] = edge->c0.c[2]/edge->z0 + (0.5 - (edge->y0 - (int)edge->y0)) * edge->dcPerScan.c[2];

    } else {
        edge->xIntersect = edge->x0 + (1 + 0.5 - (edge->y0 - (int)edge->y0)) * edge->dxPerScan;
        edge->zIntersect = 1/edge->z0 + (1 + 0.5 - (edge->y0 - (int)edge->y0)) * edge->dzPerScan;
        edge->cIntersect.c[0] = edge->c0.c[0]/edge->z0 + (1 + 0.5 - (edge->y0 - (int)edge->y0)) * edge->dcPerScan.c[0];
        edge->cIntersect.c[1] = edge->c0.c[1]/edge->z0 + (1 + 0.5 - (edge->y0 - (int)edge->y0)) * edge->dcPerScan.c[1];
        edge->cIntersect.c[2] = edge->c0.c[2]/edge->z0 + (1 + 0.5 - (edge->y0 - (int)edge->y0)) * edge->dcPerScan.c[2];   
    }
    if (edge->y0 < 0) {
        edge->xIntersect += edge->dxPerScan * (-edge->y0);
        edge->zIntersect += edge->dzPerScan * (-edge->y0);
        edge->cIntersect.c[0] += edge->dcPerScan.c[0] * (-edge->y0);
        edge->cIntersect.c[1] += edge->dcPerScan.c[1] * (-edge->y0);
        edge->cIntersect.c[2] += edge->dcPerScan.c[2] * (-edge->y0);
        edge->x0 = edge->xIntersect;
        edge->z0 = edge->zIntersect;
        edge->c0 = edge->cIntersect;
        edge->y0 = 0;
        edge->yStart = 0;
    }
    // check for really bad cases with steep slopes where xIntersect has gone beyond the end of the edge
    if (edge->xIntersect >= edge->x1 && edge->xIntersect >= edge->x0) {
        edge->xIntersect = edge->x1;
    }
    // return the newly created edge data structure
    return (edge);
}

/*
    Returns a list of all the edges in the polygon in sorted order by
    smallest row.
*/
static LinkedList *setupEdgeList(Polygon *p, Image *src)
{
    LinkedList *edges = NULL;
    Point v1, v2;
    Color c1, c2;
    int i;
    // create a linked list
    edges = ll_new();

    // walk around the polygon, starting with the last point
    v1 = p->vertex[p->nVertex - 1];
    if (p->color) {
        color_copy(&c1, &p->color[p->nVertex - 1]);
    }

    for (i = 0; i < p->nVertex; i++)
    {
        // the current point (i) is the end of the segment
        v2 = p->vertex[i];
        if (p->color) {
            color_copy(&c2, &p->color[i]);
        }

        // if it is not a horizontal line
        if ((int)(v1.val[1] + 0.5) != (int)(v2.val[1] + 0.5))
        {
            Edge *edge;
            // if the first coordinate is smaller (top edge)
            if (v1.val[1] < v2.val[1])
                edge = makeEdgeRec(v1, v2, &c1, &c2, src);
            else
                edge = makeEdgeRec(v2, v1, &c2, &c1, src);
            // insert the edge into the list of edges if it's not null
            if (edge)
                ll_insert(edges, edge, compYStart);
        }
        v1 = v2;
        if (p->color) {
            color_copy(&c1, &c2);
        }
    }

    // check for empty edges (like nothing in the viewport)
    if (ll_empty(edges))
    {
        ll_delete(edges, NULL);
        edges = NULL;
    }

    return (edges);
}

/*
    Draw one scanline of a polygon given the scanline, the active edges,
    a DrawState, the image, and some Lights (for Phong shading only).
 */
static void fillScan(int scan, LinkedList *active, Image *src, DrawState *ds) {
    Edge *p1, *p2;
    int i, f;
    float dzPerColumn, curZ;
    Color curColor, dcPerColumn, trueColor;
    // loop over the list
    p1 = ll_head(active);
    while (p1)
    {
        p2 = ll_next(active);
        if (!p2) {
            printf("bad bad bad (your edges are not coming in pairs), and p1 is: (%f, %f), (%f, %f)\n", p1->x0, p1->y0, p1->x1, p1->y1);
            break;
        }
        if (p2->xIntersect == p1->xIntersect) {
            p1 = ll_next(active);
            continue;
        }
        i = (int)p1->xIntersect + 0.5;
        if (i < 0) {
            i = 0;
        }
        f = (int)p2->xIntersect + 0.5;
        if (f > src->cols) {
            f = src->cols;
        }
        dzPerColumn = (p2->zIntersect - p1->zIntersect)/(p2->xIntersect - p1->xIntersect);
        dcPerColumn.c[0] = (p2->cIntersect.c[0] - p1->cIntersect.c[0])/(p2->xIntersect - p1->xIntersect);
        dcPerColumn.c[1] = (p2->cIntersect.c[1] - p1->cIntersect.c[1])/(p2->xIntersect - p1->xIntersect);
        dcPerColumn.c[2] = (p2->cIntersect.c[2] - p1->cIntersect.c[2])/(p2->xIntersect - p1->xIntersect);
        curZ = p1->zIntersect;
        curColor = p1->cIntersect;
        for (int cur = i; cur < f; cur++) {
            if (curZ>=image_getz(src, scan, cur)) {
                image_setz(src, scan, cur, curZ);
                switch(ds->shade) {
                    case ShadeConstant:
                        image_setColor(src, scan, cur, ds->color);
                        break;
                    case ShadeDepth:
                        float scaleFactor = 1-curZ;
                        image_setColor(src, scan, cur, 
                            (Color){{ds->color.c[0]*scaleFactor, ds->color.c[1]*scaleFactor, ds->color.c[2]*scaleFactor}});
                        break;
                    case ShadeGouraud:
                        color_set(&trueColor, curColor.c[0]/curZ, curColor.c[1]/curZ, curColor.c[2]/curZ);
                        image_setColor(src, scan, cur, trueColor);
                        break;
                    default:
                        break;
                }
            }
            curZ += dzPerColumn;
            curColor.c[0] += dcPerColumn.c[0];
            curColor.c[1] += dcPerColumn.c[1];
            curColor.c[2] += dcPerColumn.c[2];
        }
        p1 = ll_next(active);
    }
    return;
}

/*
     Process the edge list, assumes the edges list has at least one entry
*/
static int processEdgeList(LinkedList *edges, Image *src, DrawState *ds) {
    LinkedList *active = NULL;
    LinkedList *tmplist = NULL;
    LinkedList *transfer = NULL;
    Edge *current;
    Edge *tedge;
    int scan = 0;

    active = ll_new();
    tmplist = ll_new();

    // get a pointer to the first item on the list and reset the current pointer
    current = ll_head(edges);

    // start at the first scanline and go until the active list is empty
    for (scan = current->yStart; scan < src->rows; scan++)
    {
        // printf("processing current scan: %d\n", scan);
        // grab all edges starting on this row
        while (current != NULL && current->yStart == scan)
        {
            ll_insert(active, current, compXIntersect);
            current = ll_next(edges);
        }
        // current is either null, or the first edge to be handled on some future scanline

        if (ll_empty(active)) {
            break;
        }
        // if there are active edges
        // fill out the scanline
        fillScan(scan, active, src, ds);

        // remove any ending edges and update the rest
        for (tedge = ll_pop(active); tedge != NULL; tedge = ll_pop(active))
        {

            // keep anything that's not ending
            if (tedge->yEnd > scan) {
                float a;

                // update the edge information with the dPerScan values
                tedge->xIntersect += tedge->dxPerScan;
                tedge->zIntersect += tedge->dzPerScan;
                tedge->cIntersect.c[0] += tedge->dcPerScan.c[0];
                tedge->cIntersect.c[1] += tedge->dcPerScan.c[1];
                tedge->cIntersect.c[2] += tedge->dcPerScan.c[2];

                // adjust in the case of partial overlap
                if (tedge->dxPerScan < 0.0 && tedge->xIntersect < tedge->x1) {
                    a = (tedge->xIntersect - tedge->x1) / tedge->dxPerScan;
                    tedge->xIntersect = tedge->x1;
                }
                else if (tedge->dxPerScan > 0.0 && tedge->xIntersect > tedge->x1) {
                    a = (tedge->xIntersect - tedge->x1) / tedge->dxPerScan;
                    tedge->xIntersect = tedge->x1;
                }

                ll_insert(tmplist, tedge, compXIntersect);
            }
        }

        transfer = active;
        active = tmplist;
        tmplist = transfer;
    }

    // get rid of the lists, but not the edge records
    ll_delete(active, NULL);
    ll_delete(tmplist, NULL);

    return (0);
}
void _polygon_drawFill(Polygon *p, Image *src, DrawState *ds, Lighting *ls);
/***
 * helper method for polygon_drawFill
 */
void _polygon_drawFill(Polygon *p, Image *src, DrawState *ds, Lighting *ls) {
    LinkedList *edges = NULL;

    // set up the edge list
    edges = setupEdgeList(p, src);
    printf("edge list created\n");
    if (!edges){
        return;
    }
    // process the edge list (should be able to take an arbitrary edge list)
    processEdgeList(edges, src, ds);
// clean up
    ll_delete(edges, (void (*)(const void *))free);

    return;
}

/**
 * Draws a filled polygon of the specified color into the image src.
 */
void polygon_drawFill(Polygon *p, Image *src, Color c, Lighting *ls)
{
    DrawState ds;
    ds = *drawstate_create();
    drawstate_setColor(&ds, c);
    _polygon_drawFill(p, src, &ds, ls);
}

/****************************************
End Scanline Fill
*****************************************/

/***
 * returns an allocated Polygon pointer initialized so that numVertex is 0 and vertex is NULL.
 */
Polygon *polygon_create(void)
{
    Polygon *p = (Polygon *)malloc(sizeof(Polygon));
    p->zBuffer = 1;
    p->nVertex = 0;
    p->vertex = NULL;
    p->color = NULL;
    p->normal = NULL;
    p->oneSided = 0;
    return p;
}

/***
 * returns an allocated Polygon pointer with the vertex list initialized to a copy of the points in vlist.
 */
Polygon *polygon_createp(int numV, Point *vlist)
{
    Polygon *p = (Polygon *)malloc(sizeof(Polygon));
    p->nVertex = numV;
    p->vertex = (Point *)malloc(sizeof(Point) * numV);
    for (int i = 0; i < numV; i++)
    {
        point_copy(&p->vertex[i], &vlist[i]);
    }
    p->color = NULL;
    p->normal = NULL;
    p->oneSided = 0;
    return p;
}

/***
 * frees the internal data for a Polygon and the Polygon pointer.
 */
void polygon_free(Polygon *p)
{
    if (p == NULL)
    {
        return;
    }
    if (p->normal != NULL)
    {
        free(p->normal);
    }
    if (p->color != NULL)
    {
        free(p->color);
    }
    if (p->vertex != NULL)
    {
        free(p->vertex);
    }
    free(p);
}

/***
 * initializes the existing Polygon to an empty Polygon.
 */
void polygon_init(Polygon *p)
{
    p->vertex = NULL;
    p->normal = NULL;
    p->color = NULL;
    p->zBuffer = 1;
    p->nVertex = 0;
    p->oneSided = 0;
}

/***
 * initializes the vertex array to the points in vlist.
 */
void polygon_set(Polygon *p, int numV, Point *vlist)
{
    if (vlist==NULL) {
        return;
    }
    if (p->vertex != NULL)
    {
        free(p->vertex);
    }
    p->vertex = (Point *)malloc(sizeof(Point) * numV);
    p->nVertex = numV;
    int i;
    for (i = 0; i < numV; i++)
    {
        point_copy(&p->vertex[i], &vlist[i]);
    }
}

/***
 * frees the internal data and resets the fields.
 */
void polygon_clear(Polygon *p)
{
    if (p->vertex != NULL)
    {
        free(p->vertex);
        p->vertex = NULL;
    }
    if (p->normal != NULL)
    {
        free(p->normal);
        p->normal = NULL;
    }
    if (p->color != NULL)
    {
        free(p->color);
        p->color = NULL;
    }
    p->zBuffer = 1;
    p->nVertex = 0;
    p->oneSided = 0;
}

/***
 * sets the oneSided field to the value.
 */
void polygon_setSided(Polygon *p, int oneSided)
{
    p->oneSided = oneSided;
}

/***
 * initializes the color array to the colors in clist.
 */
void polygon_setColors(Polygon *p, int numV, Color *clist)
{
    if (p->color != NULL)
    {
        free(p->color);
    }
    if (clist==NULL) {
        p->color = NULL;
        return;
    }
    p->color = (Color *)malloc(sizeof(Color) * numV);
    for (int i = 0; i < numV; i++)
    {
        color_copy(&p->color[i], &clist[i]);
    }
}

/***
 * initializes the normal array to the vectors in nlist.
 * if nlist is NULL, use the vertices to build normal based on equations
 */
void polygon_setNormals(Polygon *p, int numV, Vector *nlist)
{
    if (numV==0) {
        return;
    }
    if (p->normal!=NULL) free(p->normal);
    p->normal = (Vector *)malloc(sizeof(Vector) * numV);
    int i;
    if (nlist==NULL) {
        double a, b, c;
        double x1, x2, x3, y1, y2, y3, z1, z2, z3;
        for (i = 0; i < numV-2; i++) {
            x1 = p->vertex[i].val[0];
            y1 = p->vertex[i].val[1];
            z1 = p->vertex[i].val[2];
            x2 = p->vertex[i+1].val[0];
            y2 = p->vertex[i+1].val[1];
            z2 = p->vertex[i+1].val[2];
            x3 = p->vertex[i+2].val[0];
            y3 = p->vertex[i+2].val[1];
            z3 = p->vertex[i+2].val[2];
            a = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
            b = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
            c = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
            vector_set(&p->normal[i], a, b, c);
        }
        for (i = 0; i < 2; i++) {
            x1 = p->vertex[(i+numV-2)%numV].val[0];
            y1 = p->vertex[(i+numV-2)%numV].val[1];
            z1 = p->vertex[(i+numV-2)%numV].val[2];
            x2 = p->vertex[(i+numV-1)%numV].val[0];
            y2 = p->vertex[(i+numV-1)%numV].val[1];
            z2 = p->vertex[(i+numV-1)%numV].val[2];
            x3 = p->vertex[(i+numV)%numV].val[0];
            y3 = p->vertex[(i+numV)%numV].val[1];
            z3 = p->vertex[(i+numV)%numV].val[2];
            a = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
            b = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
            c = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
            vector_set(&p->normal[i+numV-2], a, b, c);
        }
    } else {
        for (i = 0; i < numV; i++) {
            vector_copy(&p->normal[i], &nlist[i]);
        }
    }
}

/***
 * initializes the vertex list to the points in vlist,
 * the colors to the colors in clist,
 * the normals to the vectors in nlist,
 * and the zBuffer and oneSided flags to their respectively values.
 */
void polygon_setAll(Polygon *p, int numV, Point *vlist, Color *clist, Vector *nlist, int zBuffer, int oneSided)
{
    polygon_set(p, numV, vlist);
    polygon_setColors(p, numV, clist);
    polygon_setNormals(p, numV, nlist);
    polygon_setSided(p, oneSided);
    p->zBuffer = zBuffer;
}

/***
 * sets the z-buffer flag to the given value.
 */
void polygon_zBuffer(Polygon *p, int flag)
{
    p->zBuffer = flag;
}

/***
 * De-allocates/allocates space and copies the vertex and color data from one polygon to the other.
 */
void polygon_copy(Polygon *to, Polygon *from)
{
    polygon_clear(to);
    polygon_setAll(to, from->nVertex, from->vertex, from->color, from->normal, from->zBuffer, from->oneSided);
}

/***
 * prints polygon data to the stream designated by the FILE pointer.
 */
void polygon_print(Polygon *p, FILE *fp)
{
    fprintf(fp, "polygon number of vertices: %d\n", p->nVertex);
    fprintf(fp, "polygon is oneSided: %d\n", p->oneSided);
    fprintf(fp, "polygon zBuffer: %d\n", p->zBuffer);
    // if (p->color != NULL)
    // {
    //     fprintf(fp, "color:\n");
    //     for (int i = 0; i < p->nVertex; i++)
    //     {
    //         fprintf(fp, "%f, %f, %f\n", p->color[i].c[0], p->color[i].c[1], p->color[i].c[2]);
    //     }
    // }
    if (p->normal != NULL)
    {
        fprintf(fp, "normal:\n");
        for (int i = 0; i < p->nVertex; i++)
        {
            vector_print(&p->normal[i], fp);
        }
    }
    if (p->vertex != NULL)
    {
        fprintf(fp, "%d vertices:\n", p->nVertex);
        for (int i = 0; i < p->nVertex; i++)
        {
            point_print(&p->vertex[i], fp);
        }
    }
}

/***
 * normalize the x and y values of each vertex by the homogeneous coord.
 */
void polygon_normalize(Polygon *p)
{
    for (int i = 0; i < p->nVertex; i++)
    {
        p->vertex[i].val[0] /= p->vertex[i].val[3];
        p->vertex[i].val[1] /= p->vertex[i].val[3];
        p->vertex[i].val[3] = 1.0;
    }
}

/***
 * helper method for polygon_draw
 * draw the outline of the polygon using color c.
 */
void polygon_draw(Polygon *p, Image *src, Color c)
{
    Line l;
    for (int i = 1; i < p->nVertex; i++)
    {
        line_set(&l, p->vertex[i - 1], p->vertex[i]);
        line_zBuffer(&l, p->zBuffer);
        line_draw(&l, src, c);
    }
    if (p->nVertex > 2)
    {
        line_set(&l, p->vertex[0], p->vertex[p->nVertex - 1]);
        line_zBuffer(&l, p->zBuffer);
        line_draw(&l, src, c);
    }
}

/**
 * calculates the color of each vertex of the polygon based on the lighting model.
 */
void polygon_shade(Polygon *p, DrawState *ds, Lighting *ls) {
    polygon_print(p, stdout);
    if (p != NULL && ds != NULL && ls != NULL) {
        Vector V;
        p->color = (Color *)malloc(sizeof(Color) * p->nVertex);
        for (int i = 0; i < p->nVertex; i++) {
            vector_set(&V, ds->viewer.val[0] - p->vertex[i].val[0], ds->viewer.val[1] - p->vertex[i].val[1], ds->viewer.val[2] - p->vertex[i].val[2]);
            lighting_shading(ls, &p->normal[i], &V, &p->vertex[i], &ds->body, &ds->surface, ds->surfaceCoeff, p->oneSided, &p->color[i]);
        }
    }
}


/***
 * Draw the filled polygon using the given DrawState.
 * The shade field of the DrawState determines how the polygon should be rendered.
 * The Lighting parameter should be NULL unless you are doing Phong shading.
 */
void polygon_drawShade(Polygon *p, Image *src, DrawState *ds, Lighting *ls) {
    switch (ds->shade) {
    case ShadeFrame:
        polygon_draw(p, src, ds->color);
        break;

    case ShadeConstant:
    case ShadeDepth:
        _polygon_drawFill(p, src, ds, NULL);
        break;
    case ShadeGouraud:
    case ShadePhong:
        _polygon_drawFill(p, src, ds, ls);
        break;
    default:
        break;
    }
}
/***
 * draw the filled polygon using color c with the Barycentric coordinates algorithm.
 */
void polygon_drawFillB(Polygon *p, Image *src, Color c)
{
    float xmin, ymin, xmax, ymax, alpha, beta, gamma, x, y;
    xmin = fminf(p->vertex[0].val[0], p->vertex[1].val[0]);
    xmin = fminf(xmin, p->vertex[2].val[0]);
    ymin = fminf(p->vertex[0].val[1], p->vertex[1].val[1]);
    ymin = fminf(ymin, p->vertex[2].val[1]);
    xmax = fmaxf(p->vertex[0].val[0], p->vertex[1].val[0]);
    xmax = fmaxf(xmax, p->vertex[2].val[0]);
    ymax = fmaxf(p->vertex[0].val[1], p->vertex[1].val[1]);
    ymax = fmaxf(ymax, p->vertex[2].val[1]);

    for (int row = ymin; row < ymax; row++)
    {
        for (int col = xmin; col < xmax; col++)
        {
            x = col + 0.5;
            y = row + 0.5;
            beta = ((p->vertex[2].val[1] - p->vertex[0].val[1]) * x + (p->vertex[0].val[0] - p->vertex[2].val[0]) * y + p->vertex[2].val[0] * p->vertex[0].val[1] - p->vertex[0].val[0] * p->vertex[2].val[1]) / ((p->vertex[2].val[1] - p->vertex[0].val[1]) * p->vertex[1].val[0] + (p->vertex[0].val[0] - p->vertex[2].val[0]) * p->vertex[1].val[1] + p->vertex[2].val[0] * p->vertex[0].val[1] - p->vertex[0].val[0] * p->vertex[2].val[1]);
            gamma = ((p->vertex[0].val[1] - p->vertex[1].val[1]) * x + (p->vertex[1].val[0] - p->vertex[0].val[0]) * y + p->vertex[0].val[0] * p->vertex[1].val[1] - p->vertex[1].val[0] * p->vertex[0].val[1]) / ((p->vertex[0].val[1] - p->vertex[1].val[1]) * p->vertex[2].val[0] + (p->vertex[1].val[0] - p->vertex[0].val[0]) * p->vertex[2].val[1] + p->vertex[0].val[0] * p->vertex[1].val[1] - p->vertex[1].val[0] * p->vertex[0].val[1]);
            alpha = 1 - beta - gamma;
            if (alpha < 0 || beta < 0 || gamma < 0 || alpha > 1 || beta > 1 || gamma > 1)
            {
                continue;
            }
            else
            {
                image_setColor(src, row, col, c);
            }
        }
    }
}

void polygon_drawFillAA(Polygon *p, Image *src, Color c)
{
    // before draw, make all points 4x larger
    int scale = 4;
    Polygon *pScaled = polygon_createp(p->nVertex, p->vertex);

    Image *tmp = image_create(src->rows * scale, src->cols * scale);
    for (int i = 0; i < src->rows; i++)
    {
        for (int j = 0; j < src->cols; j++)
        {
            Color color = image_getColor(src, i, j);

            // Set the color in the scaled image
            for (int di = 0; di < scale; di++)
            {
                for (int dj = 0; dj < scale; dj++)
                {
                    image_setColor(tmp, i * scale + di, j * scale + dj, color);
                }
            }
        }
    }

    for (int i = 0; i < pScaled->nVertex; i++)
    {
        pScaled->vertex[i].val[0] = pScaled->vertex[i].val[0] * scale;
        pScaled->vertex[i].val[1] = pScaled->vertex[i].val[1] * scale;
    }

    polygon_drawFill(pScaled, tmp, c, NULL);
    polygon_free(pScaled);
    // downsize
    for (int row = 0; row < src->rows; row++)
    {
        for (int col = 0; col < src->cols; col++)
        {
            float r = 0, g = 0, b = 0;
            int scaleDouble = scale * scale;
            for (int dr = 0; dr < scale; dr++)
            {
                for (int dc = 0; dc < scale; dc++)
                {
                    Color cTmp = image_getColor(tmp, row * scale + dr, col * scale + dc);
                    r += cTmp.c[0];
                    g += cTmp.c[1];
                    b += cTmp.c[2];
                }
            }

            r /= scaleDouble;
            g /= scaleDouble;
            b /= scaleDouble;

            Color cAvg = {{r, g, b}};
            image_setColor(src, row, col, cAvg);
        }
    }
    image_free(tmp);
}

/**
 * sets the zbuffer flag to 1 and the curve points to the X-axis between 0 and 1.
 */
void bezierCurve_init(BezierCurve *b) {
    if (b != NULL) {
        b->zBuffer = 1;
        for (int i = 0; i < 4; i++) {
            b->vertex[i] = (Point){.val = {0.0, 0.0, 0.0, 1.0}};
        }
    }
}

/**
 * sets the zbuffer flag to 1 and the surface to the X-Z plane between (0, 0) and (1, 1).
 */
void bezierSurface_init(BezierSurface *b) {
    if (b != NULL) {
        b->zBuffer = 1;
        for (int i = 0; i < 4; i++) {
            for (int j=0; j < 4; j++) {
                b->vertex[i][j] = (Point){.val = {0.0, 0.0, 0.0, 1.0}};
            }
        }
    }
}

/**
 *  sets the control points of the BezierCurve to the four points in the vlist array.
 */
void bezierCurve_set(BezierCurve *b, Point *vlist) {
    if (b != NULL && vlist != NULL) {
        for (int i = 0; i < 4; i++) {
            point_copy(&b->vertex[i],  &vlist[i]);
        }
    }
}

/**
 * sets the control points of the BezierSurface to the 16 points in the vlist array.
 */
void bezierSurface_set(BezierSurface *b, Point *vlist) {
    if (b != NULL && vlist != NULL) {
        for (int i = 0; i < 4; i++) {
            for (int j=0; j < 4; j++) {
                point_copy(&b->vertex[i][j],  &vlist[i*4+j]);
            }
        }
    }
}

/**
 * sets the z-buffer flag to the given value.
 */
void bezierCurve_zBuffer(BezierCurve *p, int flag) {
    if (p != NULL) {
        p->zBuffer = flag;
    }
}

/**
 * sets the z-buffer flag to the given value.
 */
void bezierSurface_zBuffer(BezierSurface *p, int flag) {
    if (p != NULL) {
        p->zBuffer = flag;
    }
}

/**
 * copy the bezierCurve data structure
 */
void bezierCurve_copy(BezierCurve *to, BezierCurve *from) {
    if (!to || !from) return;
    for (int i=0; i<4; i++) {
        point_copy(&to->vertex[i], &from->vertex[i]);
    }
    bezierCurve_zBuffer(to, from->zBuffer);
}

/**
 * copy the bezierSurface data structure
 */
void bezierSurface_copy(BezierSurface *to, BezierSurface *from) {
    if (!to || !from) return;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            point_copy(&to->vertex[i][j], &from->vertex[i][j]);
        }
    }
    bezierSurface_zBuffer(to, from->zBuffer);
    bezierSurface_setDivisions(to, from->divisions);
    bezierSurface_setSolid(to, from->solid);
}

/***
 * normalize the x and y values of each vertex by the homogeneous coordinate.
 */
void bezierCurve_normalize(BezierCurve *b)
{
    for (int i = 0; i < 4; i++)
    {
        point_normalize(&b->vertex[i]);
    }
}

/**
 * normalize the x and y values of each vertex by the homogeneous coordinate.
 */
void bezierSurface_normalize(BezierSurface *b) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            point_normalize(&b->vertex[i][j]);
        }
    }
}

/**
 * set divisions attribute for the bezierSurface
 */
void bezierSurface_setDivisions(BezierSurface *b, int d) {
    b->divisions = d;
}

/**
 * set solid attribute for the buzierSurface.
 */
void bezierSurface_setSolid(BezierSurface *b, int s) {
    b->solid = s;
}

void point_mid(Point *p1, Point *p2, Point *mid);
void drawLine(Image *src, Color c, Point p1, Point p2);

/**
 * helper funtion to store the mid point between p1 and p2 to mid.
 */
void point_mid(Point *p1, Point *p2, Point *mid) {
    mid->val[0] = (p1->val[0] + p2->val[0]) / 2.0;
    mid->val[1] = (p1->val[1] + p2->val[1]) / 2.0;
    mid->val[2] = (p1->val[2] + p2->val[2]) / 2.0;
    mid->val[3] = (p1->val[3] + p2->val[3]) / 2.0;
}

/**
 * helper funtion to draw line based on two points by constructing a line object.
 */
void drawLine(Image *src, Color c, Point p1, Point p2) {
    Line l;
    line_set(&l, p1, p2);
    line_draw(&l, src, c);
}

/** helper function to do de casteljau process */
void deCasteljau(BezierCurve *b, Point* q, Point* r);
void deCasteljau(BezierCurve *b, Point* q, Point* r) {
    Point mid;
    
    q[0] = b->vertex[0];
    r[3] = b->vertex[3];
    
    point_mid(&b->vertex[0], &b->vertex[1], &q[1]);
    point_mid(&b->vertex[1], &b->vertex[2], &mid);
    point_mid(&q[1], &mid, &q[2]);

    point_mid(&b->vertex[2], &b->vertex[3], &r[2]);
    point_mid(&mid, &r[2], &r[1]);
    
    point_mid(&q[2], &r[1], &q[3]);
    r[0] = q[3];
}

/**
 * draws the Bezier curve, given in screen coordinates, into the image using the given color.
 */
void bezierCurve_draw(BezierCurve *b, Image *src, Color c) {
    // Calculate bounding box
    float minX = b->vertex[0].val[0];
    float maxX = b->vertex[0].val[0];
    float minY = b->vertex[0].val[1];
    float maxY = b->vertex[0].val[1];

    for (int i = 1; i < 4; i++) {
        if (b->vertex[i].val[0] < minX) minX = b->vertex[i].val[0];
        if (b->vertex[i].val[0] > maxX) maxX = b->vertex[i].val[0];
        if (b->vertex[i].val[1] < minY) minY = b->vertex[i].val[1];
        if (b->vertex[i].val[1] > maxY) maxY = b->vertex[i].val[1];
    }

    float width = maxX - minX;
    float height = maxY - minY;
    float threshold = 10.0;

    if (width < threshold && height < threshold) {
        // Draw the three line segments between the four control points
        drawLine(src, c, b->vertex[0], b->vertex[1]);
        drawLine(src, c, b->vertex[1], b->vertex[2]);
        drawLine(src, c, b->vertex[2], b->vertex[3]);
        return;
    }

    // Use de Casteljau's algorithm for subdivision if bounding box is larger than the threshold
    Point q[4], r[4];
    deCasteljau(b, q, r);
    BezierCurve leftCurve, rightCurve;
    bezierCurve_set(&leftCurve, q);
    bezierCurve_set(&rightCurve, r);

    bezierCurve_draw(&leftCurve, src, c);
    bezierCurve_draw(&rightCurve, src, c);
}

void bezierSurface_subdivide(BezierSurface *b, BezierSurface *subsurfaces);

/**
 * helper funtion to subdivide the bezierSurface
 */
void bezierSurface_subdivide(BezierSurface *b, BezierSurface *subsurfaces) {
    BezierCurve u[4], v[4];
    Point q[2][4][4], r[2][4][4], p[4];
    Point temp[16];
    // control points in the u-direction
    bezierCurve_set(&u[0], b->vertex[0]);
    bezierCurve_set(&u[1], b->vertex[1]);
    bezierCurve_set(&u[2], b->vertex[2]);
    bezierCurve_set(&u[3], b->vertex[3]);
    // control points in the v-direction
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            p[j] = b->vertex[j][i];
        }
    bezierCurve_set(&v[i], p);
    }

    // de casteljau process to split subsurfaces in u direction
    for (int i = 0; i < 4; i++) {
        deCasteljau(&u[i], q[0][i], r[0][i]);
        // rowMid[i][0] = b->vertex[i][0];
        // rowMid[i][7] = b->vertex[i][3];
        // point_mid(&b->vertex[i][0], &b->vertex[i][1], &rowMid[i][1]);
        // point_mid(&b->vertex[i][1], &b->vertex[i][2], &mid[i][0]);
        // point_mid(&rowMid[i][1], &mid[i][0], &rowMid[i][2]);
        // point_mid(&b->vertex[i][2], &b->vertex[i][3], &rowMid[i][6]);
        // point_mid(&mid[i][0], &rowMid[i][6], &rowMid[i][5]);
        // point_mid(&rowMid[i][2], &rowMid[i][5], &rowMid[i][3]);
        // rowMid[i][4] = rowMid[i][3];
    }

    // split subsurfaces in v direction for the first half
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            v[i].vertex[j] = q[0][j][i];
        }
        deCasteljau(&v[i], q[1][i], r[1][i]);
    }
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            temp[i*4+j] = q[1][j][i];
        }
    }
    bezierSurface_set(&subsurfaces[0], temp);
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            temp[i*4+j] = r[1][j][i];
        }
    }
    bezierSurface_set(&subsurfaces[1], temp);

    // split subsurfaces in v direction for the second half
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            v[i].vertex[j] = r[0][j][i];
        }
        deCasteljau(&v[i], q[1][i], r[1][i]);
    }
    for (int k = 0; k < 2; k++) {
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                temp[i*4+j] = q[1][j][i];
            }
        }
        bezierSurface_set(&subsurfaces[2], temp);
    }
    for (int k = 0; k < 2; k++) {
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                temp[i*4+j] = r[1][j][i];
            }
        }
        bezierSurface_set(&subsurfaces[3], temp);
    }
}

/**
 * draws the Bezier surface, given in screen coordinates, into the image using the given color.
 */
void bezierSurface_drawLines(BezierSurface *b, Image *src, Color c) {
    if (b->divisions<=0) {
        // draw the horizontal and vertical lines
        for (int i=0; i<4; i++) {
            for (int j=0; j<3; j++) {
                drawLine(src, c, b->vertex[i][j], b->vertex[i][j+1]);
                drawLine(src, c, b->vertex[j][i], b->vertex[j+1][i]);
            }
        }
        return;
    }
    BezierSurface subsurfaces[4];
    for (int i=0; i<4; i++) {
        bezierSurface_setDivisions(&subsurfaces[i], b->divisions-1);
        bezierSurface_setSolid(&subsurfaces[i], b->solid);
        bezierSurface_zBuffer(&subsurfaces[i], b->zBuffer);
    }
    bezierSurface_subdivide(b, subsurfaces);
    for (int i=0; i<4; i++) {
        bezierSurface_drawLines(&subsurfaces[i], src, c);
    }
}