/***
 * written by - Jiafeng
 *
 * graphics primitive apis
 */

#include <string.h>
#include "graphics.h"

// Function to initialize the stack
Stack *stack_create(void)
{
	Stack *stack = (Stack *)malloc(sizeof(Stack));
	if (!stack)
	{
		exit(EXIT_FAILURE);
	}
	stack->top = NULL;
	stack->element_size = 0;
	return stack;
}

// Function to push an element onto the stack
void stack_push(Stack *stack, void *element, size_t element_size)
{
	Node *new_node = (Node *)malloc(sizeof(Node));
	if (!new_node)
	{
		exit(EXIT_FAILURE);
	}

	new_node->data = malloc(element_size);
	if (!new_node->data)
	{
		free(new_node);
		exit(EXIT_FAILURE);
	}

	memcpy(new_node->data, element, element_size);
	new_node->next = stack->top;
	stack->top = new_node;
	stack->element_size += 1;
}

// Function to pop an element from the stack
void *stack_pop(Stack *stack)
{
	if (stack->top == NULL)
	{
		exit(EXIT_FAILURE);
	}

	Node *temp = stack->top;
	void *res = temp->data;

	stack->top = temp->next;
	stack->element_size -= 1;

	free(temp); // Free the node but not the data
	return res;
}

// Function to free the stack
void stack_free(Stack *stack)
{
	Node *current = stack->top;
	while (current != NULL)
	{
		Node *next = current->next;
		free(current->data); // Free the data of each node
		free(current);		 // Free the node itself
		current = next;
	}
	stack->element_size = 0;
	free(stack);
}

/*
	Creates a new linked list and returns a pointer to it.
*/
LinkedList *ll_new(void)
{
	LinkedList *ll;

	ll = malloc(sizeof(LinkedList));
	if (!ll)
	{
		printf("ll_new(): unable to allocate list memory\n");
		return (NULL);
	}

	ll->root = NULL;
	ll->current = ll->root;

	return (ll);
}

/*
	 Inserts the item into the list using the specified comparison
	 function.
*/
void ll_insert(LinkedList *ll, void *item, int (*comp)(const void *, const void *))
{
	Node *p, *q, *n;

	// see if the list is empty or the item replaces the starting element
	if (ll->root == NULL || comp(item, ll->root->data) <= 0)
	{
		n = malloc(sizeof(Node));
		n->next = ll->root;
		n->data = item;
		ll->root = n;
		return;
	}

	// deal with the rest of the list, it doesn't go in front
	q = ll->root;
	p = q->next;
	while (p)
	{
		// see if it comes between p and q
		if (comp(item, p->data) <= 0)
		{
			n = malloc(sizeof(Node));
			n->data = item;
			n->next = p;
			q->next = n;
			return;
		}
		q = p;
		p = q->next;
	}

	// goes at the end
	n = malloc(sizeof(Node));
	n->data = item;
	n->next = NULL;
	q->next = n;

	return;
}

/*
	Returns true if the list is empty
*/
int ll_empty(LinkedList *ll)
{
	if (ll->root == NULL)
		return (1);
	return (0);
}

/*
	 Return a pointer to the top item in the list
*/
void *ll_peek(LinkedList *ll)
{
	// if (ll->root == NULL)
	// 	return (NULL);

	// return (ll->root->data);
	if (ll->current == NULL)
		return (NULL);

	return (ll->current->data);
}

/*
	Sets the current iterator to the head of the list and returns the
	data from the first node.
 */
void *ll_head(LinkedList *ll)
{
	ll->current = ll->root;
	if (ll->current != NULL)
		return (ll->current->data);

	return (NULL);
}

/*
	Returns the data from the next element in the list as specified by
	the iterator.  Returns NULL if the end of the list has been reached.
 */
void *ll_next(LinkedList *ll)
{
	if (ll->current != NULL)
		ll->current = ll->current->next;

	if (ll->current != NULL)
		return (ll->current->data);

	return (NULL);
}

/*
	 Removes the top element from the list and returns it.
 */
void *ll_pop(LinkedList *ll)
{
	Node *n;
	void *node_data;

	if (ll->root == NULL)
		return (NULL);

	n = ll->root;
	ll->root = ll->root->next;

	node_data = n->data;
	free(n);

	return (node_data);
}

/*
	Deletes all elements of the list and the list itself, freeing each
	node's content using the specified function, which could be NULL.
 */
void ll_delete(LinkedList *ll, void (*freefunc)(const void *))
{
	Node *p, *q;

	p = ll->root;
	while (p != NULL)
	{
		q = p->next;
		if (freefunc && p->data)
			freefunc(p->data);
		free(p);
		p = q;
	}
	free(ll);

	return;
}

/***
 * Set the Vector to (x, y, z, 0.0).
 */
void vector_set(Vector *v, double x, double y, double z)
{
	v->val[0] = x;
	v->val[1] = y;
	v->val[2] = z;
	v->val[3] = 0.0;
}

/***
 * Print out the Vector to stream fp in a pretty form.
 */
void vector_print(Vector *v, FILE *fp)
{
	fprintf(fp, "vector (%f, %f, %f)\n", v->val[0], v->val[1], v->val[2]);
}

/***
 * Copy the src Vector into the dest Vector.
 */
void vector_copy(Vector *dest, Vector *src)
{
	dest->val[0] = src->val[0];
	dest->val[1] = src->val[1];
	dest->val[2] = src->val[2];
}

/***
 * Returns the Euclidean length of the vector. The homogeneous coordinate should be 0.
 */
double vector_length(Vector *v)
{
	long double L = v->val[0] * v->val[0] + v->val[1] * v->val[1] + v->val[2] * v->val[2];
	return sqrt(L);
}

/***
 * Normalize the Vector to unit length. Do not modify the homogeneous coordinate.
 */
void vector_normalize(Vector *v)
{
	double L = vector_length(v);
	v->val[0] = v->val[0] / L;
	v->val[1] = v->val[1] / L;
	v->val[2] = v->val[2] / L;
}

/***
 * Returns the scalar product of a_arrow and b_arrow
 */
double vector_dot(Vector *a, Vector *b)
{
	return a->val[0] * b->val[0] + a->val[1] * b->val[1] + a->val[2] * b->val[2];
}

/***
 * Calculates the the cross product (vector product) of a_arrow and b_arrow and puts the result in c_arrow.
 * c.x = a.y*b.z - a.z*b.y
 * c.y = a.z*b.x - a.x*b.z
 * c.z = a.x*b.y - a.y*b.x
 */
void vector_cross(Vector *a, Vector *b, Vector *c)
{
	c->val[0] = a->val[1] * b->val[2] - a->val[2] * b->val[1];
	c->val[1] = a->val[2] * b->val[0] - a->val[0] * b->val[2];
	c->val[2] = a->val[0] * b->val[1] - a->val[1] * b->val[0];
}

/* 2D and Generic Matrix functions */

/***
 * Print out the matrix in a nice 4x4 arrangement with a blank line below.
 */
void matrix_print(Matrix *m, FILE *fp)
{
	for (int i = 0; i < 4; i++)
	{
		fprintf(fp, "%f, %f, %f, %f\n", m->m[i][0], m->m[i][1], m->m[i][2], m->m[i][3]);
	}
}

/***
 * Set the matrix to all zeros.
 */
void matrix_clear(Matrix *m)
{
	for (int i = 0; i < 4; i++)
	{
		m->m[i][0] = m->m[i][1] = m->m[i][2] = m->m[i][3] = 0;
	}
}

/***
 * set the matrix to the identity matrix.
 */
void matrix_identity(Matrix *m)
{
	for (int i = 0; i < 4; i++)
	{
		m->m[i][0] = m->m[i][1] = m->m[i][2] = m->m[i][3] = 0;
		m->m[i][i] = 1;
	}
}

/***
 * Return the element of the matrix at row r, column c.
 */
double matrix_get(Matrix *m, int r, int c)
{
	return m->m[r][c];
}

/***
 * Set the element of the matrix at row r, column c to v.
 */
void matrix_set(Matrix *m, int r, int c, double v)
{
	m->m[r][c] = v;
}

/***
 * Copy the src matrix into the dest matrix.
 */
void matrix_copy(Matrix *dest, Matrix *src)
{
	for (int i = 0; i < 4; i++)
	{
		dest->m[i][0] = src->m[i][0];
		dest->m[i][1] = src->m[i][1];
		dest->m[i][2] = src->m[i][2];
		dest->m[i][3] = src->m[i][3];
	}
}

/***
 * Transpose the matrix m in place.
 */
void matrix_transpose(Matrix *m)
{
	double tmp;
	for (int i = 1; i < 4; i++)
	{
		tmp = m->m[i][0];
		m->m[i][0] = m->m[0][i];
		m->m[0][i] = tmp;
	}
	for (int i = 2; i < 4; i++)
	{
		tmp = m->m[i][1];
		m->m[i][1] = m->m[1][i];
		m->m[1][i] = tmp;
	}
	tmp = m->m[3][2];
	m->m[3][2] = m->m[2][3];
	m->m[2][3] = tmp;
}

/***
 * Multiply left and right and put the result in m.
 */
void matrix_multiply(Matrix *left, Matrix *right, Matrix *m)
{
	Matrix tmpL, tmpR;
	matrix_copy(&tmpL, left);
	matrix_copy(&tmpR, right);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m->m[i][j] = tmpL.m[i][0] * tmpR.m[0][j] + tmpL.m[i][1] * tmpR.m[1][j] + tmpL.m[i][2] * tmpR.m[2][j] + tmpL.m[i][3] * tmpR.m[3][j];
		}
	}
}

/***
 * Transform the point p by the matrix m and put the result in q.
 * For this function, p and q need to be different variables.
 */
void matrix_xformPoint(Matrix *m, Point *p, Point *q)
{
	for (int i = 0; i < 4; i++)
	{
		q->val[i] = m->m[i][0] * p->val[0] + m->m[i][1] * p->val[1] + m->m[i][2] * p->val[2] + m->m[i][3] * p->val[3];
	}
}

/***
 * Transform the vector p by the matrix m and put the result in q.
 * For this function, p and q need to be different variables.
 */
void matrix_xformVector(Matrix *m, Vector *p, Vector *q)
{
	for (int i = 0; i < 4; i++)
	{
		q->val[i] = m->m[i][0] * p->val[0] + m->m[i][1] * p->val[1] + m->m[i][2] * p->val[2];
	}
}

/***
 * Transform the points and surface normals (if they exist) in the Polygon p by the matrix m.
 */
void matrix_xformPolygon(Matrix *m, Polygon *p)
{
	Vector tmp;
	if (p->normal != NULL)
	{
		for (int i = 0; i < p->nVertex; i++)
		{
			matrix_xformVector(m, &p->normal[i], &tmp);
			vector_copy(&(p->normal[i]), &tmp);
		}
	}
	if (p->vertex != NULL)
	{
		for (int i = 0; i < p->nVertex; i++)
		{
			matrix_xformPoint(m, &p->vertex[i], &tmp);
			point_copy(&(p->vertex[i]), &tmp);
		}
	}
}

/***
 * Transform the points in the Polyline p by the matrix m.
 */
void matrix_xformPolyline(Matrix *m, Polyline *p)
{
	Point tmp;
	if (p->vertex != NULL)
	{
		for (int i = 0; i < p->numVertex; i++)
		{
			matrix_xformPoint(m, &p->vertex[i], &tmp);
			point_copy(&(p->vertex[i]), &tmp);
		}
	}
}

/***
 * Transform the points in line by the matrix m.
 */
void matrix_xformLine(Matrix *m, Line *line)
{
	Point tmp;
	matrix_xformPoint(m, &line->a, &tmp);
	point_copy(&(line->a), &tmp);
	matrix_xformPoint(m, &line->b, &tmp);
	point_copy(&(line->b), &tmp);
}

/***
 * Transform the points in BezierCurve by the matrix m.
 */
void matrix_xformBezierCurve(Matrix *m, BezierCurve *b) {
	if (!m || !b) return;
	BezierCurve tmp;
	for (int i=0; i<4; i++) {
		matrix_xformPoint(m, &b->vertex[i], &tmp.vertex[i]);
		point_copy(&b->vertex[i], &tmp.vertex[i]);
	}
}

/***
 * Transform the points in BezierSurface by the matrix m.
 */
void matrix_xformBezierSurface(Matrix *m, BezierSurface *b) {
	if (!m || !b) return;
	BezierSurface tmp;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			matrix_xformPoint(m, &b->vertex[i][j], &tmp.vertex[i][j]);
			point_copy(&b->vertex[i][j], &tmp.vertex[i][j]);
		}
	}
}

/***
 * Transform the center point and radius in circle by the matrix m.
 */
void matrix_xformCircle(Matrix *m, Circle *c, double scale) {
	Point tmp;
	matrix_xformPoint(m, &c->c, &tmp);
	point_copy(&(c->c), &tmp);
	c->r *= scale;
}

/***
 * Premultiply the matrix by a scale matrix parameterized by sx and sy .
 */
void matrix_scale2D(Matrix *m, double sx, double sy)
{
	Matrix S;
 
  matrix_identity(&S);
  S.m[0][0] = sx;
  S.m[1][1] = sy;
 
  matrix_multiply(&S, m, m);
}

/***
 * Premultiply the matrix by a Z-axis rotation matrix parameterized by cos(theta) and sin(theta),
 * where theta is the angle of rotation about the Z-axis.
 */
void matrix_rotateZ(Matrix *m, double cth, double sth)
{
	Matrix r, tmp;
	matrix_clear(&r);
	matrix_copy(&tmp, m);
	matrix_set(&r, 0, 0, cth);
	matrix_set(&r, 0, 1, -sth);
	matrix_set(&r, 1, 0, sth);
	matrix_set(&r, 1, 1, cth);
	matrix_set(&r, 2, 2, 1);
	matrix_set(&r, 3, 3, 1);
	matrix_multiply(&r, &tmp, m);
}

/***
 * Premultiply the matrix by a 2D translation matrix parameterized by tx and ty.
 */
void matrix_translate2D(Matrix *m, double tx, double ty)
{
	Matrix t, tmp;
	matrix_identity(&t);
	matrix_copy(&tmp, m);
	matrix_set(&t, 0, 3, tx);
	matrix_set(&t, 1, 3, ty);
	matrix_multiply(&t, &tmp, m);
}

/***
 * Premultiply the matrix by a 2D shear matrix parameterized by shx and shy.
 */
void matrix_shear2D(Matrix *m, double shx, double shy)
{
	Matrix sh, tmp;
	matrix_identity(&sh);
	matrix_copy(&tmp, m);
	matrix_set(&sh, 0, 1, shx);
	matrix_set(&sh, 1, 0, shy);
	matrix_multiply(&sh, &tmp, m);
}

/* 3D Matrix functions */

/***
 * Premultiply the matrix by a translation matrix parameterized by tx , ty , and tz .
 */
void matrix_translate(Matrix *m, double tx, double ty, double tz)
{
	Matrix t, tmp;
	matrix_identity(&t);
	matrix_copy(&tmp, m);
	matrix_set(&t, 0, 3, tx);
	matrix_set(&t, 1, 3, ty);
	matrix_set(&t, 2, 3, tz);
	matrix_multiply(&t, &tmp, m);
}

/***
 * Premultiply the matrix by a scale matrix parameterized by sx , sy , sz .
 */
void matrix_scale(Matrix *m, double sx, double sy, double sz)
{
	Matrix s, tmp;
	matrix_clear(&s);
	matrix_copy(&tmp, m);
	matrix_set(&s, 0, 0, sx);
	matrix_set(&s, 1, 1, sy);
	matrix_set(&s, 2, 2, sz);
	matrix_set(&s, 3, 3, 1);
	matrix_multiply(&s, &tmp, m);
}

/***
 * Premultiply the matrix by a X-axis rotation matrix parameterized by cos(th) and sin(th),
 * where th is the angle of rotation about the X-axis.
 */
void matrix_rotateX(Matrix *m, double cth, double sth)
{
	Matrix r, tmp;
	matrix_clear(&r);
	matrix_copy(&tmp, m);
	matrix_set(&r, 0, 0, 1);
	matrix_set(&r, 1, 1, cth);
	matrix_set(&r, 1, 2, -sth);
	matrix_set(&r, 2, 1, sth);
	matrix_set(&r, 2, 2, cth);
	matrix_set(&r, 3, 3, 1);
	matrix_multiply(&r, &tmp, m);
}

/***
 * Premultiply the matrix by a Y-axis rotation matrix parameterized by cos(th) and sin(th),
 * where th is the angle of rotation about the Y-axis.
 */
void matrix_rotateY(Matrix *m, double cth, double sth)
{
	Matrix r, tmp;
	matrix_clear(&r);
	matrix_copy(&tmp, m);
	matrix_set(&r, 0, 0, cth);
	matrix_set(&r, 0, 2, sth);
	matrix_set(&r, 1, 1, 1);
	matrix_set(&r, 2, 0, -sth);
	matrix_set(&r, 2, 2, cth);
	matrix_set(&r, 3, 3, 1);
	matrix_multiply(&r, &tmp, m);
}

/***
 * Premultiply the matrix by an XYZ-axis rotation matrix parameterized by the vectors ~u, ~v, and ~w,
 * where the three vectors represent an orthonormal 3D basis.
 */
void matrix_rotateXYZ(Matrix *m, Vector *u, Vector *v, Vector *w)
{
	Matrix r, tmp;
	matrix_clear(&r);
	matrix_copy(&tmp, m);
	matrix_set(&r, 0, 0, u->val[0]);
	matrix_set(&r, 0, 1, u->val[1]);
	matrix_set(&r, 0, 2, u->val[2]);
	matrix_set(&r, 1, 0, v->val[0]);
	matrix_set(&r, 1, 1, v->val[1]);
	matrix_set(&r, 1, 2, v->val[2]);
	matrix_set(&r, 2, 0, w->val[0]);
	matrix_set(&r, 2, 1, w->val[1]);
	matrix_set(&r, 2, 2, w->val[2]);
	matrix_set(&r, 3, 3, 1);
	matrix_multiply(&r, &tmp, m);
}

/***
 * Premultiply the matrix by a shearZ matrix parameterized by shx and shy.
 */
void matrix_shearZ(Matrix *m, double shx, double shy)
{
	Matrix sh, tmp;
	matrix_identity(&sh);
	matrix_copy(&tmp, m);
	matrix_set(&sh, 0, 2, shx);
	matrix_set(&sh, 1, 2, shy);
	matrix_multiply(&sh, &tmp, m);
}

/***
 * Premultiply the matrix by a perspective matrix parameterized by d.
 */
void matrix_perspective(Matrix *m, double d)
{
	Matrix pers;
	matrix_identity(&pers);
	pers.m[3][2] = 1/d;
	pers.m[3][3] = 0.0;
	matrix_multiply(&pers, m, m);
}

/* 2D Viewing */

/***
 * Sets vtm to be the view transformation defined by the 2DView structure.
 */
void matrix_setView2D(Matrix *vtm, View2D *view)
{
	view->dv = (view->du * view->R)/view->C;
	matrix_identity(vtm);
	matrix_translate2D(vtm, -view->v0.val[0], -view->v0.val[1]);
	matrix_rotateZ(vtm, view->n.val[0], -view->n.val[1]);
	matrix_scale2D(vtm, view->C / view->du, -view->R / view->dv);
	matrix_translate2D(vtm, view->C / 2, view->R / 2);
}

/***
 * sets view based on the input
 */
void view2D_set( View2D *view, Point *v0, int du, Vector *n, int C, int R ) {
	view->du = du;
	view->R = R;
	view->C = C;
	view->dv = (view->du * view->R)/view->C;
	view->n = *n;
	view->v0 = *v0;
}

/* 3D VIew */
void matrix_setView3D(Matrix *vtm, View3D *view)
{
	Vector u;
	double dprime;
	matrix_identity(vtm);
	vector_cross(&view->vup, &view->vpn, &u); // calculate u vector
	vector_cross(&view->vpn, &u, &view->vup); // recalcula vup vector
	matrix_translate(vtm, -view->vrp.val[0], -view->vrp.val[1], -view->vrp.val[2]); // translate vrp to origin
	vector_normalize(&u);
	vector_normalize(&view->vpn);
	vector_normalize(&view->vup);
	matrix_rotateXYZ(vtm, &u, &view->vup, &view->vpn); // orient view coordinate
	matrix_translate(vtm, 0, 0, view->d); // move ccenter of projection to origin
	matrix_scale(vtm, (2 * view->d) / ((view->d + view->b) * view->du), (2 * view->d) / ((view->d + view->b) * view->dv), 1 / (view->d + view->b)); // scale to canonical view volume
	matrix_perspective(vtm, view->d / (view->d + view->b));
	dprime = view->d / (view->d + view->b);
	matrix_scale(vtm, -view->screenx/(2*dprime), -view->screeny/(2*dprime), 1.0);
	matrix_translate2D(vtm, view->screenx/2, view->screeny/2);
}

/* DrawState Functions */

/* create a new DrawState structure and initialize the fields. */
DrawState *drawstate_create( void ) {
	DrawState *ds = (DrawState *)malloc(sizeof(DrawState));
    if (ds) {
        // Initialize with default values
        ds->color = (Color){{1.0, 1.0, 1.0}}; 
        ds->flatColor =(Color){{0.0, 0.0, 0.0}};
        ds->body =(Color){{0.0, 0.0, 0.0}};
        ds->surface = (Color){{0.0, 0.0, 0.0}}; 
        ds->surfaceCoeff = 1.0f;
        ds->shade = ShadeFrame;  // Default to frame shading
        ds->zBufferFlag = 1;  // Enable z-buffer by default
        ds->viewer = (Point){{0.0, 0.0, 0.0, 1.0}};  // Viewer at origin
    }
    return ds;
}

/* set the color field to c. */
void drawstate_setColor( DrawState *s, Color c ) {
	if (s) {
        s->color = c;
    }
}

/* set the body field to c. */
void drawstate_setBody( DrawState *s, Color c ) {
	if (s) {
        s->body = c;
    }
}

/* set the surface field to c. */
void drawstate_setSurface( DrawState *s, Color c ) {
	if (s) {
        s->surface = c;
    }
}

/* set the surfaceCoeff field to f. */
void drawstate_setSurfaceCoeff( DrawState *s, float f ) {
	if (s) {
        s->surfaceCoeff = f;
    }
}

/* copy the DrawState data. */
void drawstate_copy( DrawState *to, DrawState *from ) {
	if (to && from) {
        memcpy(to, from, sizeof(DrawState));
    }
}