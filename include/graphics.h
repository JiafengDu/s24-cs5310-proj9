#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"
#include "color.h"


// Node Structure for Stack and LinkedList
typedef struct Node {
    void *data;
    void *next;
} Node;

// Stack Structure
typedef struct {
    Node *top;
    size_t element_size;
} Stack;

// LinkedList Structure
typedef struct {
    Node *root;
    Node *current;
} LinkedList;

// Matrix Structure
typedef struct {
    double m[4][4];
} Matrix;

// Point and Vector Structures
typedef struct {
    double val[4]; // Four-element vector of doubles
} Point;

typedef Point Vector;

// Line Structure
typedef struct {
    int zBuffer; // Whether to use z-buffer, default to true (1)
    Point a;
    Point b;
} Line;

// Circle Structure
typedef struct {
    double r; // Radius
    Point c;  // Center
} Circle;

// Ellipse Structure
typedef struct {
    double ra; // Major axis radius
    double rb; // Minor axis radius
    Point c;
    double a; // (Optional) angle of major axis relative to the X-axis
} Ellipse;

// Polyline Structure
typedef struct {
    int zBuffer;   // Whether to use z-buffer, default to true (1)
    int numVertex; // Number of vertices
    Point *vertex;
} Polyline;

// Polygon Structure
typedef struct {
    int oneSided;
    int nVertex;
    Point *vertex;  // Vertex information
    Color *color;   // Color information for each vertex
    Vector *normal; // Surface normal information for each vertex
    int zBuffer;
} Polygon;

// Bezier Curve Structure
typedef struct {
    Point vertex[4]; // 4 control points
    int zBuffer;
} BezierCurve;

// Bezier Surface Structure
typedef struct {
    Point vertex[4][4]; // 4x4, 16 total control points
    int divisions; // number of division for control points
    int solid; // if solid is 0, draw with line. Otherwise polygons.
    int zBuffer;
} BezierSurface;

// ObjectType Enumeration
typedef enum {
    ObjNone,
    ObjLine,
    ObjPoint,
    ObjPolyline,
    ObjPolygon,
    ObjBezierCurve,
    ObjBezierSurface,
    ObjIdentity,
    ObjMatrix,
    ObjColor,
    ObjBodyColor,
    ObjSurfaceColor,
    ObjSurfaceCoeff,
    ObjLight,
    ObjModule
} ObjectType;

// union that can hold one instance of any of the constituent types.
typedef union {
    Point point;
    Line line;
    Polyline polyline;
    Polygon polygon;
    Matrix matrix;
    Color color;
    Matrix identity;
    BezierCurve bezierCurve;
    BezierSurface bezierSurface;
    float coeff;
    void *module;
} Object;

// Element Structure for Linked List
typedef struct {
    ObjectType type; // Type of object stored in the obj pointer
    Object obj; // Pointer to the object
    void *next; // Next pointer
} Element;

// Module Structure
typedef struct {
    Element *head; // Pointer to the head of the linked list
    Element *tail; // Pointer to the last object
} Module;

// View2D Structure
typedef struct {
    Point v0;  // Center of the view in world coordinates
    double du;     // Width of the view in world coordinates
    double dv;     // Height of the view in world coordinates
    double thetaV; // Orientation angle theta_v
    Vector n;   // X-axis of the view window expressed as a normalized vector (nx = cos(theta_v), ny = sin(theta_v))
    int C;         // Number of columns in the output image
    int R;         // Number of rows in the output image
} View2D;

// View3D Structure
typedef struct {
    Point vrp;   // View Reference Point: 3-D vector indicating the origin of the view reference coordinates
    Vector vpn;  // View Plane Normal: 3-D vector indicating the direction in which the viewer is looking
    Vector vup;  // View Up Vector: 3-D vector indicating the UP direction on the view plane. CANNOT be parallel to vpn
    double d;    // Projection distance: distance in the negative vpn direction at which the center of projection is located
    double du;   // Width of view plane around the VRP, expressed in world coordinate distances
    double dv;   // Height of view plane around the VRP, expressed in world coordinate distances
    double f;    // Front clip plane expressed as distances along the positive vpn. f > 0 and f < b
    double b;    // Back clip plane expressed as distances along the positive vpn
    int screenx; // X-axis of the desired image in pixels
    int screeny; // Y-axis of the desired image in pixels
} View3D;

// ShadeMethod Enumeration
typedef enum {
    ShadeFrame, // Draw only the borders of objects, including polygons
    ShadeConstant, // Draw objects using the current foreground color, fill polygons
    ShadeDepth, // Draw objects using their depth value
    ShadeFlat, // Draw objects using shading calculations, but each polygon is a constant value
    ShadeGouraud, // Draw objects using Gouraud shading
    ShadePhong, // Draw objects using Phong shading
} ShadeMethod;

// DrawState Structure
typedef struct {
    Color color; // Foreground color, used in the default drawing mode
    Color flatColor; // Color to flat-fill a polygon based on a shading calculation
    Color body; // Body reflection color, used for shading calculations
    Color surface; // Surface reflection color, used for shading calculations
    float surfaceCoeff; // A float that represents the shininess of the surface
    ShadeMethod shade; // An enumerated type
    int zBufferFlag; // Whether to use z-buffer hidden surface removal
    Point viewer; // A Point representing the view location in 3D (identical to the VRP in View3D)
} DrawState;

typedef struct {
  int nLights;
} Lighting;


// Function prototypes
Stack *stack_create(void);
void stack_push(Stack *stack, void *element, size_t element_size);
void *stack_pop(Stack *stack);
void stack_free(Stack *stack);

LinkedList *ll_new(void);
void ll_delete(LinkedList *ll, void (*freefunc)(const void *));
void ll_insert(LinkedList *ll, void *item, int (*comp)(const void *, const void *));
void *ll_peek(LinkedList *ll);
void *ll_head(LinkedList *ll);
void *ll_next(LinkedList *ll);
void *ll_pop(LinkedList *ll);
int ll_empty(LinkedList *ll);

/* Vector functions */
void vector_set(Vector *v, double x, double y, double z);
void vector_print(Vector *v, FILE *fp);
void vector_copy(Vector *dest, Vector *src);
double vector_length(Vector *v);
void vector_normalize(Vector *v);
double vector_dot(Vector *a, Vector *b);
void vector_cross(Vector *a, Vector *b, Vector *c);

/* 2D and Generic Matrix functions */
void matrix_print(Matrix *m, FILE *fp);
void matrix_clear(Matrix *m);
void matrix_identity(Matrix *m);
double matrix_get(Matrix *m, int r, int c);
void matrix_set(Matrix *m, int r, int c, double v);
void matrix_copy(Matrix *dest, Matrix *src);
void matrix_transpose(Matrix *m);
void matrix_multiply(Matrix *left, Matrix *right, Matrix *m);
void matrix_xformPoint(Matrix *m, Point *p, Point *q);
void matrix_xformVector(Matrix *m, Vector *p, Vector *q);
void matrix_xformPolygon(Matrix *m, Polygon *p);
void matrix_xformPolyline(Matrix *m, Polyline *p);
void matrix_xformBezierCurve(Matrix *m, BezierCurve *b);
void matrix_xformBezierSurface(Matrix *m, BezierSurface *b);
void matrix_xformCircle(Matrix *m, Circle *c, double scale);
void matrix_xformLine(Matrix *m, Line *line);
void matrix_scale2D(Matrix *m, double sx, double sy);
void matrix_rotateZ(Matrix *m, double cth, double sth);
void matrix_translate2D(Matrix *m, double tx, double ty);
void matrix_shear2D(Matrix *m, double shx, double shy);

/* 3D Matrix functions */
void matrix_translate(Matrix *m, double tx, double ty, double tz);
void matrix_scale(Matrix *m, double sx, double sy, double sz);
void matrix_rotateX(Matrix *m, double cth, double sth);
void matrix_rotateY(Matrix *m, double cth, double sth);
void matrix_rotateXYZ(Matrix *m, Vector *u, Vector *v, Vector *w);
void matrix_shearZ(Matrix *m, double shx, double shy);
void matrix_perspective(Matrix *m, double d);

/* 2D Viewing */
void matrix_setView2D(Matrix *vtm, View2D *view);
void view2D_set( View2D *view, Point *vrp, int dx, Vector *xaxis, int screenx, int screeny );

/* 3D VIew */
void matrix_setView3D(Matrix *vtm, View3D *view);

/* Point functions */
void point_set2D(Point *p, double x, double y);
void point_set3D(Point *p, double x, double y, double z);
void point_set(Point *p, double x, double y, double z, double h);
void point_normalize(Point *p);
void point_copy(Point *to, Point *from);
void point_draw(Point *p, Image *src, Color c);
void point_drawf(Point *p, Image *src, FPixel c);
void point_print(Point *p, FILE *fp);

/* Line functions */
void line_set2D(Line *l, double x0, double y0, double x1, double y1);
void line_set(Line *l, Point ta, Point tb);
void line_zBuffer(Line *l, int flag);
void line_normalize(Line *l);
void line_copy(Line *to, Line *from);
void line_draw(Line *l, Image *src, Color c);

/* Circle functions */
void circle_set(Circle *c, Point tc, double tr);
void circle_draw(Circle *c, Image *src, Color p);
void circle_drawFill(Circle *c, Image *src, Color p);

/* Eclipse functions */
void ellipse_set(Ellipse *e, Point tc, double ta, double tb);
void ellipse_draw(Ellipse *e, Image *src, Color p);
void ellipse_drawFill(Ellipse *e, Image *src, Color p);

/* Polyline functions */
Polyline *polyline_create(void);
Polyline *polyline_createp(int numV, Point *vlist);
void polyline_free(Polyline *p);
void polyline_init(Polyline *p);
void polyline_set(Polyline *p, int numV, Point *vlist);
void polyline_clear(Polyline *p);
void polyline_zBuffer(Polyline *p, int flag);
void polyline_copy(Polyline *to, Polyline *from);
void polyline_print(Polyline *p, FILE *fp);
void polyline_normalize(Polyline *p);
void polyline_draw(Polyline *p, Image *src, Color c);

/* Polygon functions */
Polygon *polygon_create(void);
Polygon *polygon_createp(int numV, Point *vlist);
void polygon_free(Polygon *p);
void polygon_init(Polygon *p);
void polygon_set(Polygon *p, int numV, Point *vlist);
void polygon_clear(Polygon *p);
void polygon_setSided(Polygon *p, int oneSided);
void polygon_setColors(Polygon *p, int numV, Color *clist);
void polygon_setNormals(Polygon *p, int numV, Vector *nlist);
void polygon_setAll(Polygon *p, int numV, Point *vlist, Color *clist, Vector *nlist, int zBuffer, int oneSided);
void polygon_zBuffer(Polygon *p, int flag);
void polygon_copy(Polygon *to, Polygon *from);
void polygon_print(Polygon *p, FILE *fp);
void polygon_normalize(Polygon *p);
void polygon_draw(Polygon *p, Image *src, Color c);
void polygon_drawShade(Polygon *p, Image *src, DrawState *ds, Lighting *light);
void polygon_drawFill(Polygon *p, Image *src, Color c);
void polygon_drawFillB(Polygon *p, Image *src, Color c);
void polygon_drawFillAA(Polygon *p, Image *src, Color c);

/* Bezier Curve and Surface Functions*/
void bezierCurve_init(BezierCurve *b);
void bezierSurface_init(BezierSurface *b);
void bezierCurve_set(BezierCurve *b, Point *vlist);
void bezierSurface_set(BezierSurface *b, Point *vlist);
void bezierCurve_copy(BezierCurve *to, BezierCurve *from);
void bezierSurface_copy(BezierSurface *to, BezierSurface *from);
void bezierCurve_normalize(BezierCurve *b);
void bezierSurface_normalize(BezierSurface *b);
void bezierCurve_zBuffer(BezierCurve *p, int flag);
void bezierSurface_zBuffer(BezierSurface *p, int flag);
void bezierSurface_setDivisions(BezierSurface *b, int d);
void bezierSurface_setSolid(BezierSurface *b, int s);
void bezierCurve_draw(BezierCurve *b, Image *src, Color c);
void bezierSurface_drawLines(BezierSurface *b, Image *src, Color c);

/* 2D and Generic Module Functions */
Element *element_create(void);
Element *element_init(ObjectType type, void *obj);
void element_delete(Element *e);
Module *module_create(void);
void module_clear(Module *md);
void module_delete(Module *md);
void module_insert(Module *md, Element *e);
void module_module(Module *md, Module *sub);
void module_point(Module *md, Point *p);
void module_line(Module *md, Line *p);
void module_polyline(Module *md, Polyline *p);
void module_polygon(Module *md, Polygon *p);
void module_bezierCurve(Module *m, BezierCurve *b, int divisions);
void module_bezierSurface(Module *m, BezierSurface *b, int divisions, int solid);
void module_identity(Module *md);
void module_translate2D(Module *md, double tx, double ty);
void module_scale2D(Module *md, double sx, double sy);
void module_rotateZ(Module *md, double cth, double sth);
void module_shear2D(Module *md, double shx, double shy);
void module_draw(Module *md, Matrix *VTM, Matrix *GTM, DrawState *ds, Lighting *lighting, Image *src);

/* 3D Module Functions */
void module_translate(Module *md, double tx, double ty, double tz);
void module_scale(Module *md, double sx, double sy, double sz);
void module_rotateX(Module *md, double cth, double sth);
void module_rotateY(Module *md, double cth, double sth);
void module_rotateXYZ(Module *md, Vector *u, Vector *v, Vector *w);
void module_cube(Module *md, int solid);
void module_pyramid( Module *md );
void module_cylinder( Module *mod, int sides );
void module_tetrahedron( Module *md );

/* Shading/Color Module Functions */
void module_color(Module *md, Color *c);
void module_bodyColor(Module *md, Color *c);
void module_surfaceColor(Module *md, Color *c);
void module_surfaceCoeff(Module *md, float coeff);

/* DrawState Functions */
DrawState *drawstate_create( void );
void drawstate_setColor( DrawState *s, Color c );
void drawstate_setBody( DrawState *s, Color c );
void drawstate_setSurface( DrawState *s, Color c );
void drawstate_setSurfaceCoeff( DrawState *s, float f );
void drawstate_copy( DrawState *to, DrawState *from );

/* Others */
void fill(Image *src, Color f, double pixelx, double pixely);

#endif