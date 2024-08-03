/*
  Jiafeng Du


*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "graphics.h"

int main(int argc, char *argv[]) {
  Image *src;
  Matrix VTM;
  Matrix GTM;
  Module *tetrahedron;
  Module *tetrahedrons;
  Module *scene;
  float angle;
  int rows = 400;
  int cols = 400;
  int i;

  Color Grey;
  Color Yellow;
  Color Blue;

  DrawState *ds;
  View3D view;

	color_set( &Grey, 175/255.0, 178/255.0, 181/255.0 );
	color_set( &Yellow, 240/255.0, 220/255.0, 80/255.0 );
	color_set( &Blue, 50/255.0, 60/255.0, 200/255.0 );

  // initialize the image
  src = image_create(rows, cols);

  // initialize matrices
  matrix_identity(&GTM);
  matrix_identity(&VTM);

  // set the View parameters
  point_set3D(&(view.vrp), 0.0, 0.0, -40.0);
  vector_set(&(view.vpn), 0.0, 0.0, 1.0);
  vector_set(&(view.vup), 0.0, 1.0, 0.0);
  view.d = 2.0;
  view.du = 1.0;
  view.dv = 1.0;
  view.f = 0.0;
  view.b = 50;
  view.screenx = cols;
  view.screeny = rows;
  matrix_setView3D(&VTM, &view);

  // print out VTM
  printf("Final VTM: \n");
  matrix_print(&VTM, stdout);

  // make a simple tetrahedron module
  tetrahedron = module_create();
  module_tetrahedron(tetrahedron);

  // make a set of 3 tetrahedrons
  tetrahedrons = module_create();

  module_identity(tetrahedrons);
  module_color(tetrahedrons, &Grey);
  module_scale(tetrahedrons, 1.5, 2, 1);
  module_translate(tetrahedrons, 1, 1, 1);
  module_module(tetrahedrons, tetrahedron);

  module_identity(tetrahedrons);
  module_color(tetrahedrons, &Yellow);
  module_scale(tetrahedrons, 2, 1, 3);
  module_translate(tetrahedrons, -1, -1, -1);
  module_module(tetrahedrons, tetrahedron);

  module_identity(tetrahedrons);
  module_color(tetrahedrons, &Blue);
  module_scale(tetrahedrons, 2, 2, 2);
  module_module(tetrahedrons, tetrahedron);

  // make a scene with lots of tetrahedron sets
  scene = module_create();

  for(i=0;i<30;i++) {

    // initialize LTM
    module_identity(scene);

    // rotate by some random angles
    angle = drand48() * 2*M_PI;
    module_rotateX(scene, cos(angle), sin(angle));
    angle = drand48() * 2*M_PI;
    module_rotateY(scene, cos(angle), sin(angle));
    angle = drand48() * 2*M_PI;
    module_rotateZ(scene, cos(angle), sin(angle));

    // translate to a location
    module_translate(scene, 
		     (drand48()-0.5)*15.0, 
		     (drand48()-0.5)*15.0, 
		     (drand48()-0.5)*15.0);
   
    // add a tri-tetrahedron
    module_module(scene, tetrahedrons);
  }

  ds = drawstate_create();
  ds->shade = ShadeDepth;

  for(i=0;i<18;i++) {
    char buffer[256];

    image_reset( src );

    matrix_identity(&GTM);
    matrix_rotateY(&GTM, cos(i*2*M_PI/36.0), sin(i*2*M_PI/36.0));
    module_draw(scene, &VTM, &GTM, ds, NULL, src);

    // write out the image
    sprintf(buffer, "../images/tetrahedron-frame-%03d.ppm", i);
    image_write(src, buffer);
  }

  // free stuff here
  module_delete( tetrahedron );
  module_delete( tetrahedrons );
  module_delete( scene );
  image_free( src );

  return(0);
}
