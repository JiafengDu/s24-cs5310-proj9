/***
 * written by Jiafeng
 * Summer 2024
 */

#include "graphics.h"

/* 2D and Generic Module Functions */

/* Allocate and return an initialized but empty Element. */
Element *element_create(void) {
	Element *e = malloc(sizeof(Element));
    if (e) {
        e->type = ObjNone;
        e->obj.module = NULL;
        e->next = NULL;
    }
    return e;
}

/* Allocate an Element and store a duplicate of the data pointed to by obj in the Element. 
 * Modules do not get duplicated.
 * The function needs to handle each type of object separately in a case statement.
*/
Element *element_init(ObjectType type, void *obj) {
	Element *e = element_create();
    if (!e) return NULL;

    e->type = type;
    switch(type) {
        case ObjLine:
            line_copy(&e->obj.line, (Line *)obj);
            break;
        case ObjPoint:
            point_copy(&e->obj.point, (Point *)obj);
            break;
        case ObjPolyline:
            polyline_init(&e->obj.polyline);
            polyline_copy(&e->obj.polyline, (Polyline *)obj);
            break;
        case ObjPolygon:
            polygon_init(&e->obj.polygon);
            polygon_copy(&e->obj.polygon, (Polygon *)obj);
            break;
		case ObjBezierCurve:
			bezierCurve_copy(&e->obj.bezierCurve, (BezierCurve*)obj);
			break;
		case ObjBezierSurface:
			bezierSurface_copy(&e->obj.bezierSurface, (BezierSurface*)obj);
			break;
        case ObjMatrix:
            matrix_copy(&e->obj.matrix, (Matrix *)obj);
            break;
        case ObjColor:
        case ObjBodyColor:
        case ObjSurfaceColor:
            color_copy(&e->obj.color, (Color *)obj);
            break;
        case ObjSurfaceCoeff:
            e->obj.coeff = *(float *)obj;  // Simple assignment for float
            break;
        case ObjModule:
            e->obj.module = obj; // Modules don't get duplicated
            break;
		case ObjIdentity:
			matrix_copy(&e->obj.identity, (Matrix *)obj);
			break;
        default:
            element_delete(e);
            return NULL;
    }
    return e;
}

/* free the element and the object it contains, as appropriate.*/
void element_delete(Element *e) {
	if (e) {
        if (e->type == ObjPolyline) {
            polyline_clear(&(e->obj.polyline));
        } else if (e->type == ObjPolygon) {
            polygon_clear(&(e->obj.polygon));
        } else if (e->type == ObjModule) {
			return;
		} else {
        	free(e);
		}
    }
}

/* Allocate an empty module. */
Module *module_create(void) {
	Module *md = malloc(sizeof(Module));
    if (md) {
        md->head = NULL;
        md->tail = NULL;
    }
    return md;
}

/* clear the module’s list of Elements, freeing memory as appropriate. */
void module_clear(Module *md) {
	if (!md) return;
    Element *current = md->head;
    while (current) {
        Element *next = current->next;
        element_delete(current);
        current = next;
    }
    md->head = md->tail = NULL;
}

/* Free all of the memory associated with a module, including the memory pointed to by md. */
void module_delete(Module *md) {
	if (md) {
        module_clear(md);
        free(md);
    }
}

/* Generic insert of an element into the module at the tail of the list. */
void module_insert(Module *md, Element *e) {
	if (!md || !e) return;
    if (!md->head) {
        md->head = md->tail = e;
    } else {
        md->tail->next = e;
        md->tail = e;
    }
    e->next = NULL;
}

/* Adds a pointer to the Module sub to the tail of the module’s list. */
void module_module(Module *md, Module *sub) {
	if (!md || !sub) return;
    Element *e = element_init(ObjModule, sub);
    if (e) module_insert(md, e);
}

/* Adds p to the tail of the module’s list. */
void module_point(Module *md, Point *p) {
	if (!md || !p) return;
    Element *e = element_init(ObjPoint, p);
    if (e) module_insert(md, e);
}

/* Adds p to the tail of the module’s list. */
void module_line(Module *md, Line *p) {
	if (!md || !p) return;
    Element *e = element_init(ObjLine, p);
    if (e) module_insert(md, e);
}

/*  Adds p to the tail of the module’s list. */
void module_polyline(Module *md, Polyline *p) {
	if (!md || !p) return;
    Element *e = element_init(ObjPolyline, p);
    if (e) module_insert(md, e);
}

/* Adds p to the tail of the module’s list. */
void module_polygon(Module *md, Polygon *p) {
	if (!md || !p) return;
    Element *e = element_init(ObjPolygon, p);
    if (e) module_insert(md, e);
}

/**
 * use the de Casteljau algorithm to subdivide the Bezier curve divisions times, 
 * then add the lines connecting the control points to the module.
 */
void module_bezierCurve(Module *m, BezierCurve *b, int divisions) {
	if (!m || !b) return;
	Element *e = element_init(ObjBezierCurve, b);
	if (e) module_insert(m, e);
}

/**
 * use the de Casteljau algorithm to subdivide the Bezier surface divisions times.
 * Add to the module either the lines connecting the control points, if solid is 0.
 * or triangles using the four corner control points if solid is number other than 0.
 */
void module_bezierSurface(Module *m, BezierSurface *b, int divisions, int solid) {
	if (!m || !b) return;
	bezierSurface_setDivisions(b, divisions);
	bezierSurface_setSolid(b, solid);
	Element *e = element_init(ObjBezierSurface, b);
	if (e) module_insert(m, e);
}

/* Object that sets the current transform to the identity, placed at the tail of the module’s list. */
void module_identity(Module *md) {
	if (!md) return;
	Matrix m;
	matrix_identity(&m);
    Element *e = element_init(ObjIdentity, &m);
    if (e) {
		module_insert(md, e);
	}
}

/* Matrix operand to add a translation matrix to the tail of the module’s list. */
void module_translate2D(Module *md, double tx, double ty) {
	if (!md) return;
    Matrix m;
    matrix_identity(&m);
    matrix_translate2D(&m, tx, ty);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}

/* Matrix operand to add a scale matrix to the tail of the module’s list. */
void module_scale2D(Module *md, double sx, double sy) {
	if (!md) return;
    Matrix m;
    matrix_identity(&m);
    matrix_scale2D(&m, sx, sy);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}

/*  Matrix operand to add a rotation about the Z axis to the tail of the module’s list */
void module_rotateZ(Module *md, double cth, double sth) {
	if (!md) return;
    Matrix m;
    matrix_identity(&m);
    matrix_rotateZ(&m, cth, sth);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}


/* Matrix operand to add a 2D shear matrix to the tail of the module’s list. */
void module_shear2D(Module *md, double shx, double shy) {
	if (!md) return;
    Matrix m;
    matrix_identity(&m);
    matrix_shear2D(&m, shx, shy);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}

/**
 * Draw the module into the image using the given 
 * view transformation matrix [VTM],
 * Lighting and
 * DrawState 
 * by traversing the list of Elements. 
 * (For now, Lighting can be an empty structure.)
 */
void module_draw(Module *md, Matrix *VTM, Matrix *GTM, DrawState *ds, Lighting *lighting, Image *src) {
    if (!md || !VTM || !GTM || !ds || !src) return;
    
    Matrix LTM;
    Element *e;
    
    matrix_identity(&LTM);
    
    for (e = md->head; e != NULL; e = e->next) {
        switch(e->type) {
            case ObjColor:
                ds->color = e->obj.color;
                break;

            case ObjBodyColor:
                ds->body = e->obj.color;
                break;

            case ObjSurfaceColor:
                ds->surface = e->obj.color;
                break;
            
            case ObjSurfaceCoeff:
                ds->surfaceCoeff = e->obj.coeff;
                break;
            
            case ObjPoint: {
                Point temp;
                matrix_xformPoint(&LTM, &e->obj.point, &temp);
                matrix_xformPoint(GTM, &temp, &temp);
                matrix_xformPoint(VTM, &temp, &temp);
                point_normalize(&temp);
                if (0 <= temp.val[0] && temp.val[0] < src->cols && 0 <= temp.val[1] && temp.val[1] < src->rows) {
                    point_draw(&temp, src, ds->color);
                }
                break;
            }

            case ObjLine: {
                Line temp;
				line_copy(&temp, &e->obj.line);
                matrix_xformLine(&LTM, &temp);
                matrix_xformLine(GTM, &temp);
                matrix_xformLine(VTM, &temp);
                line_normalize(&temp);
                line_draw(&temp, src, ds->color);
                break;
            }

            case ObjPolyline: {
                Polyline temp;
				polyline_init(&temp);
                polyline_copy(&temp, &e->obj.polyline);
                matrix_xformPolyline(&LTM, &temp);
                matrix_xformPolyline(GTM, &temp);
                matrix_xformPolyline(VTM, &temp);
                polyline_normalize(&temp);
                polyline_draw(&temp, src, ds->color);
                polyline_clear(&temp);
                break;
            }

            case ObjPolygon: {
                Polygon temp;
				polygon_init(&temp);
                polygon_copy(&temp, &e->obj.polygon);
                matrix_xformPolygon(&LTM, &temp);
                matrix_xformPolygon(GTM, &temp);
                
                if (ds->shade == ShadeGouraud || ds->shade == ShadePhong) {
                    printf("shading polygon\n");
                    polygon_shade(&temp, ds, lighting);
                }
                matrix_xformPolygon(VTM, &temp);
                polygon_normalize(&temp);
                polygon_drawShade(&temp, src, ds, lighting);
                polygon_clear(&temp);
                break;
            }

			case ObjBezierCurve: {
				BezierCurve temp; 
				bezierCurve_init(&temp);
				bezierCurve_copy(&temp, &e->obj.bezierCurve);
				matrix_xformBezierCurve(&LTM, &temp);
                matrix_xformBezierCurve(GTM, &temp);
                matrix_xformBezierCurve(VTM, &temp);
				bezierCurve_normalize(&temp);
				bezierCurve_draw(&temp, src, ds->color);
				break;
			}

			case ObjBezierSurface: {
				BezierSurface temp;
				bezierSurface_init(&temp);
				bezierSurface_copy(&temp, &e->obj.bezierSurface);
				matrix_xformBezierSurface(&LTM, &temp);
                matrix_xformBezierSurface(GTM, &temp);
                matrix_xformBezierSurface(VTM, &temp);
				bezierSurface_normalize(&temp);
				bezierSurface_drawLines(&temp, src, ds->color);
				break;
			}

            case ObjMatrix:
                matrix_multiply(&e->obj.matrix, &LTM, &LTM);
                break;

			case ObjIdentity: {
				matrix_identity(&LTM);
				break;
			}

            case ObjModule: {
                Matrix GTMpass;
                DrawState tempDS;
                
                matrix_multiply(GTM, &LTM, &GTMpass);
                drawstate_copy(&tempDS, ds);
                module_draw(e->obj.module, VTM, &GTMpass, &tempDS, lighting, src); 
                break;
            }

            default:
                break;
        }
    }
}

/* 3D Module Functions */

/* Matrix operand to add a 3D translation to the Module. */
void module_translate(Module *md, double tx, double ty, double tz) {
    if (!md) return;
    Matrix m;
    matrix_identity(&m);
    matrix_translate(&m, tx, ty, tz);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}
/* Matrix operand to add a 3D scale to the Module. */
void module_scale(Module *md, double sx, double sy, double sz) {
	if (!md) return;
    Matrix m;
    matrix_identity(&m);
    matrix_scale(&m, sx, sy, sz);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}
/* Matrix operand to add a rotation about the X-axis to the Module. */
void module_rotateX(Module *md, double cth, double sth) {
	if (!md) return;
    Matrix m;
    matrix_identity(&m);
    matrix_rotateX(&m, cth, sth);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}

/* Matrix operand to add a rotation about the Y-axis to the Module. */
void module_rotateY(Module *md, double cth, double sth) {
	if (!md) return;
    Matrix m;
    matrix_identity(&m);
    matrix_rotateY(&m, cth, sth);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}

/* Matrix operand to add a rotation that orients to the orthonormal axes ~u, ~v, w~. */
void module_rotateXYZ(Module *md, Vector *u, Vector *v, Vector *w) {
	if (!md || !u || !v || !w) return;
    Matrix m;
    matrix_identity(&m);
    matrix_rotateXYZ(&m, u, v, w);
    Element *e = element_init(ObjMatrix, &m);
    if (e) module_insert(md, e);
}

/**
 * Adds a unit cube, axis-aligned and centered on zero to the Module. 
 * If solid is zero, add only lines.
 * If solid is non-zero, use polygons.
 * Make sure each polygon has surface normals defined for it.
 */
void module_cube(Module *md, int solid) {
  if (solid==0) {
	//use lines
	Point a, b, c, d, e, f, g, h;

	point_set3D(&a, -0.5, -0.5, -0.5);
	point_set3D(&b, 0.5, -0.5, -0.5);
	point_set3D(&c, -0.5, 0.5, -0.5);
	point_set3D(&d, 0.5, 0.5, -0.5);
	point_set3D(&e, -0.5, 0.5, 0.5);
	point_set3D(&f, 0.5, 0.5, 0.5);
	point_set3D(&g, -0.5, -0.5, 0.5);
	point_set3D(&h, 0.5, -0.5, 0.5);

	Point abdc[5], abhg[5], efhg[5], efdc[5], geca[5], hfdb[5];
	abdc[0] = abhg[0] = geca[3] = abdc[4] = abhg[4] = a;
	abdc[1] = abhg[1] = hfdb[3] = b;
	abdc[2] = efdc[2] = hfdb[2] = d;
	abdc[3] = efdc[3] = geca[2] = c;
	abhg[2] = efhg[2] = hfdb[0] = hfdb[4] = h;
	abhg[3] = efhg[3] = geca[0] = geca[4] = g;
	efhg[0] = efhg[4] = efdc[0] = efdc[4] = geca[1] = e;
	efhg[1] = efdc[1] = hfdb[1] = f;

	Polyline *ABDC, *ABHG, *EFHG, *EFDC, *GECA, *HFDB;
	ABDC = polyline_createp(5, abdc);
	ABHG = polyline_createp(5, abhg);
	EFHG = polyline_createp(5, efhg);
	EFDC = polyline_createp(5, efdc);
	GECA = polyline_createp(5, geca);
	HFDB = polyline_createp(5, hfdb);

	module_polyline(md, ABDC);
	module_polyline(md, ABHG);
	module_polyline(md, EFHG);
	module_polyline(md, EFDC);
	module_polyline(md, GECA);
	module_polyline(md, HFDB);
  } else {
	//use polygons
	Point pt[4];
	Polygon p;

	polygon_init( &p );
	point_set3D( &pt[0], -0.5, -0.5, -0.5 );
	point_set3D( &pt[1], -0.5, -0.5, 0.5 );
	point_set3D( &pt[2], -0.5, 0.5, 0.5 );
	point_set3D( &pt[3], -0.5, 0.5, -0.5);
	polygon_set( &p, 4, pt );
    polygon_setNormals( &p, 4, NULL);
	module_polygon( md, &p );

	point_set3D( &pt[0], 0.5, -0.5, -0.5 );
	point_set3D( &pt[1], 0.5, -0.5, 0.5);
	point_set3D( &pt[2], 0.5, 0.5, 0.5);
	point_set3D( &pt[3], 0.5, 0.5, -0.5);
	polygon_set( &p, 4, pt );
    polygon_setNormals( &p, 4, NULL);
	module_polygon( md, &p );

	point_set3D( &pt[0], -0.5, -0.5, -0.5 );
	point_set3D( &pt[1], -0.5, -0.5, 0.5 );
	point_set3D( &pt[2], 0.5, -0.5, 0.5 );
	point_set3D( &pt[3], 0.5, -0.5, -0.5 );
	polygon_set( &p, 4, pt );
    polygon_setNormals( &p, 4, NULL);
	module_polygon( md, &p );

	point_set3D( &pt[0], -0.5, 0.5, -0.5 );
	point_set3D( &pt[1], -0.5, 0.5, 0.5 );
	point_set3D( &pt[2], 0.5, 0.5, 0.5 );
	point_set3D( &pt[3], 0.5, 0.5, -0.5 );
	polygon_set( &p, 4, pt );
    polygon_setNormals( &p, 4, NULL);  
	module_polygon( md, &p );

	point_set3D( &pt[0], -0.5, -0.5, -0.5 );
	point_set3D( &pt[1], -0.5, 0.5, -0.5);
	point_set3D( &pt[2], 0.5, 0.5, -0.5 );
	point_set3D( &pt[3], 0.5, -0.5, -0.5 );
	polygon_set( &p, 4, pt );
    polygon_setNormals( &p, 4, NULL);
	module_polygon( md, &p );

	point_set3D( &pt[0], -0.5, -0.5, 0.5 );
	point_set3D( &pt[1], -0.5, 0.5, 0.5 );
	point_set3D( &pt[2], 0.5, 0.5, 0.5 );
	point_set3D( &pt[3], 0.5, -0.5, 0.5);
	polygon_set( &p, 4, pt );
    polygon_setNormals( &p, 4, NULL);
	module_polygon( md, &p );
  }
}

/**
 * Adds a unit cylinder, axis-aligned along the Y-axis and centered on zero to the Module. 
 * Takes in the number of subdivisions to use when creating the cylinder.
 * from test6b.c by Professor Bruce Maxwell
 */
void module_cylinder( Module *mod, int sides ) {
  Polygon p;
  Point xtop, xbot;
  double x1, x2, z1, z2;
  int i;

  polygon_init( &p );
  point_set3D( &xtop, 0, 1.0, 0.0 );
  point_set3D( &xbot, 0, 0.0, 0.0 );

  // make a fan for the top and bottom sides
  // and quadrilaterals for the sides
  for(i=0;i<sides;i++) {
    Point pt[4];

    x1 = cos( i * M_PI * 2.0 / sides ); 
    z1 = sin( i * M_PI * 2.0 / sides );
    x2 = cos( ( (i+1)%sides ) * M_PI * 2.0 / sides );
    z2 = sin( ( (i+1)%sides ) * M_PI * 2.0 / sides );

    point_copy( &pt[0], &xtop );
    point_set3D( &pt[1], x1, 1.0, z1 );
    point_set3D( &pt[2], x2, 1.0, z2 );

    polygon_set( &p, 3, pt );
    module_polygon( mod, &p );

    point_copy( &pt[0], &xbot );
    point_set3D( &pt[1], x1, 0.0, z1 );
    point_set3D( &pt[2], x2, 0.0, z2 );

    polygon_set( &p, 3, pt );
    module_polygon( mod, &p );

    point_set3D( &pt[0], x1, 0.0, z1 );
    point_set3D( &pt[1], x2, 0.0, z2 );
    point_set3D( &pt[2], x2, 1.0, z2 );
    point_set3D( &pt[3], x1, 1.0, z1 );
    
    polygon_set( &p, 4, pt );
    module_polygon( mod, &p );
  }

  polygon_clear( &p );
}

/**
 * Adds a unit pyramid, axis-aligned along the Y-axis and centered on zero to the Module. 
 */
void module_pyramid( Module *md ) {
  Polygon p;
  Point xtop, pt[4];

  point_set3D( &xtop, 0, 1.0, 0.0 );

  polygon_init( &p );
  
  // bottom part of pyramid
  point_set3D( &pt[0], -0.5, 0, -0.5 ); //bottom left
  point_set3D( &pt[1], -0.5, 0, 0.5 ); // top left
  point_set3D( &pt[2], 0.5, 0, 0.5 ); // top right
  point_set3D( &pt[3], 0.5, 0, -0.5); //bottom right
  polygon_set( &p, 4, pt );
  module_polygon( md, &p );

  // left side triangle
  point_set3D( &pt[0], -0.5, 0, -0.5 ); //bottom left
  point_set3D( &pt[1], -0.5, 0, 0.5 ); // top left
  point_copy( &pt[2], &xtop);
  polygon_set( &p, 3, pt);
  module_polygon( md, &p );

  // top side triangle
  point_set3D( &pt[0], -0.5, 0, 0.5 ); // top left
  point_set3D( &pt[1], 0.5, 0, 0.5 ); // top right
  polygon_set( &p, 3, pt);
  module_polygon( md, &p );

  // right side triangle
  point_set3D( &pt[0], 0.5, 0, -0.5); //bottom right
  point_set3D( &pt[1], 0.5, 0, 0.5 ); // top right
  polygon_set( &p, 3, pt);
  module_polygon( md, &p );

  // bottom side triangle
  point_set3D( &pt[0], 0.5, 0, -0.5); //bottom right
  point_set3D( &pt[1], -0.5, 0, -0.5 ); //bottom left
  polygon_set( &p, 3, pt);
  module_polygon( md, &p );

  polygon_clear( &p );
}

/**
 * Adds a unit tetrahedron, centered on zero to the Module, and inscribed in the unit cube
 */
void module_tetrahedron( Module *md ) {
  Polygon p;
  Point xtop, pt[4];

  point_set3D( &xtop, 0, 1.0, 0.0 );

  polygon_init( &p );
  
  point_set3D( &pt[0], -0.5, 0, 0.5 ); // bottom upper left
  point_set3D( &pt[1], 0.5, 0, -0.5); // bottom lower right
  point_set3D( &pt[2], -0.5, 1, -0.5 ); // top lower left
  point_set3D( &pt[3], 0.5, 1, 0.5); // top upper right

  polygon_set( &p, 3, pt); // bottom upper left, bottom lower right, top lower left
  module_polygon( md, &p );

  point_set3D( &pt[2], 0.5, 1, 0.5); // top upper right
  polygon_set( &p, 3, pt); // bottom upper left, bottom lower right, top upper right
  module_polygon( md, &p );

  point_set3D( &pt[0], -0.5, 1, -0.5 ); // top lower left
  point_set3D( &pt[1], 0.5, 1, 0.5); // top upper right
  point_set3D( &pt[2], -0.5, 0, 0.5 ); // bottom upper left
  polygon_set( &p, 3, pt); // top lower left, top upper right, bottom upper left
  module_polygon( md, &p );

  point_set3D( &pt[2], 0.5, 0, -0.5); // bottom lower right
  polygon_set( &p, 3, pt); // top lower left, top upper right, bottom lower right
  module_polygon( md, &p );

  polygon_clear( &p );
}

/* Shading/Color Module Functions */

/* Adds the foreground color value to the tail of the module’s list. */
void module_color(Module *md, Color *c) {
    if (!md || !c) return;
    Element *e = element_init(ObjColor, c);
    if (e) module_insert(md, e);
}

/* Adds the body color value to the tail of the module’s list. */
void module_bodyColor(Module *md, Color *c) {
	if (!md || !c) return;
    Element *e = element_init(ObjBodyColor, c);
    if (e) module_insert(md, e);
}

/*  Adds the surface color value to the tail of the module’s list. */
void module_surfaceColor(Module *md, Color *c) {
	if (!md || !c) return;
    Element *e = element_init(ObjSurfaceColor, c);
    if (e) module_insert(md, e);
}

/* Adds the specular coefficient to the tail of the module’s list.*/
void module_surfaceCoeff(Module *md, float coeff) {
	if (!md) return;
    Element *e = element_init(ObjSurfaceCoeff, &coeff);
    if (e) module_insert(md, e);
}