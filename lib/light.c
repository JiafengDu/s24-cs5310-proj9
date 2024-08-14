/***
 * written by - Jiafeng
 *
 * light and lighting apis
 */

#include "graphics.h"

/* Light Functions */

/* Initialize light to default values */
void light_init(Light *l) {
    Color c;
    color_set(&c, 0.0, 0.0, 0.0);
    Vector d;
    vector_set(&d, 0.0, 0.0, 0.0);
    Point p;
    point_set(&p, 0.0, 0.0, 0.0, 0.0);
    l->type = LightNone;
    l->color = c;
    l->direction = d;
    l->position = p;
    l->cutoff = 0.0;
    l->sharpness = 0.0;
}

/* Copy the contents of one light to another light */
void light_copy(Light *to, Light *from) {
    to->type = from->type;
    color_copy(&to->color, &from->color);
    vector_copy(&to->direction, &from->direction);
    point_copy(&to->position, &from->position);
    to->cutoff = from->cutoff;
    to->sharpness = from->sharpness;
}

/* Lighting Functions */

/* allocate and return a new lighting structure set to default values. */
Lighting *lighting_create( void ) {
    Lighting *l = (Lighting *)malloc(sizeof(Lighting));
    if (l) {
        l->nLights = 0;
        light_init(&l->light[0]);
    }
    return l;
}

/* delete the lighting structure. */
void lighting_delete(Lighting *lights) {
    free(lights);
}

/* Initialize lighting structure to default values */
void lighting_init(Lighting *l) {
    l->nLights = 0;
    light_init(&l->light[0]);
}

/*  reset the Lighting struct to 0 lights */
void lighting_clear(Lighting *l) {
    l->nLights = 0;
    light_init(&l->light[0]);
}

/**
 * add a new light to the lighting structure given the parameters, 
 * some of which may be NULL, depending upon the type. 
 * Make sure you don’t add more lights than MAX LIGHTS.
 */ 
void lighting_add(Lighting *l, LightType type, Color *c, Vector *d, Point *pos, float cutoff, float sharpness) {
    if (l->nLights < MAX_LIGHTS) {
        l->light[l->nLights].type = type;
        color_copy(&l->light[l->nLights].color, c);
        vector_copy(&l->light[l->nLights].direction, d);
        point_copy(&l->light[l->nLights].position, pos);
        l->light[l->nLights].cutoff = cutoff;
        l->light[l->nLights].sharpness = sharpness;
        l->nLights++;
    }
}

/**
 * calculate the proper color given the normal N, view vector V, 3D point P, 
 * body color Cb, surface color Cs, 
 * sharpness value s, the lighting, 
 * and whether the polygon is one-sided or two-sided. 
 * Put the result in the Color c
 */
void lighting_shading(Lighting *l, Vector *N, Vector *V, Point *p, Color *Cb, Color *Cs, float s, int oneSided, Color *c) {
    if (l->nLights == 0) {
        color_copy(c, Cb);
    } else {
        double a0 = 0.001, a1 = 0.5, a2 = 0.001;
        int i;
        Color C;
        Vector L;
        vector_normalize(V);
        vector_normalize(N);
        color_set(&C, 0.0, 0.0, 0.0);
        for (i = 0; i < l->nLights; i++) {
            switch (l->light[i].type) {
                case LightNone:
                    continue;
                case LightAmbient:
                    color_set(&C, C.c[0]+l->light[i].color.c[0] * Cb->c[0], C.c[1]+l->light[i].color.c[1] * Cb->c[1], C.c[2]+l->light[i].color.c[2] * Cb->c[2]);
                    continue;
                case LightPoint:
                    vector_set(&L, l->light[i].position.val[0] - p->val[0], l->light[i].position.val[1] - p->val[1], l->light[i].position.val[2] - p->val[2]);
                    break;
                case LightDirect:
                    vector_set(&L, l->light[i].direction.val[0], l->light[i].direction.val[1], l->light[i].direction.val[2]);
                    break;
                case LightSpot:
                    vector_set(&L, l->light[i].position.val[0] - p->val[0], l->light[i].position.val[1] - p->val[1], l->light[i].position.val[2] - p->val[2]);
                    Vector negL;
                    vector_set(&negL, -L.val[0], -L.val[1], -L.val[2]);
                    double t = vector_dot(&negL, &l->light[i].direction);
                    if (t < l->light[i].cutoff) continue; // or use pow(t, l->light[i].sharpness) to modify intensity
                    break;  
                default:
                    continue;
            }
            double d = vector_length(&L);
            double fd = fmin(1.0, 1/(a0 + a1*d + a2*d*d));
            vector_normalize(&L);
            double theta = vector_dot(&L, N);
            // if it is one-sided, and light source is facing away from the viewer, skip
            if (oneSided==1 && theta < 0) {
                continue;
            }
            double sigma = vector_dot(V, N);
            // if it is two-sided, and light source is facing away from the viewer, skip
            if ((theta<0 && sigma>0) || (theta>0 && sigma<0)) continue;
            Vector H;
            vector_set(&H, (L.val[0]+V->val[0])/2, (L.val[1]+V->val[1])/2, (L.val[2]+V->val[2])/2);
            vector_normalize(&H);
            double beta = vector_dot(&H, N);
            if (theta<0 && oneSided!=1) {
                beta = -beta;
                theta = -theta;
            }
            fprintf(stdout, "H: ");
            vector_print(&H, stdout);
            fprintf(stdout, "L * N: %f   beta: %f \n", theta, beta);
            vector_print(&L, stdout);
            vector_print(N, stdout);
            fprintf(stdout, "Gouraud Shade: %f, %f, %f\n", 
                l->light[i].color.c[0] * (Cb->c[0] * theta + Cs->c[0] * pow(beta, s)),
                l->light[i].color.c[1] * (Cb->c[1] * theta + Cs->c[1] * pow(beta, s)),
                l->light[i].color.c[2] * (Cb->c[2] * theta + Cs->c[2] * pow(beta, s))
            );
            color_set(
                &C,
                C.c[0] + l->light[i].color.c[0] * (Cb->c[0] * theta + Cs->c[0] * pow(beta, s)), 
                C.c[1] + l->light[i].color.c[1] * (Cb->c[1] * theta + Cs->c[1] * pow(beta, s)), 
                C.c[2] + l->light[i].color.c[2] * (Cb->c[2] * theta + Cs->c[2] * pow(beta, s))
            );
        }
        color_copy(c, &C);
    }
}