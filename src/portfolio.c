/*
  Jiafeng Du
  Summer 2024

  draw cube using graphics
*/

#include "graphics.h"

void setBlack( Image *src ) {
  int i, j;
  Color White;

  color_set(&White, 0.0, 0.0, 0.0);

  for(i=0;i<src->rows;i++) {
    for(j=0;j<src->cols;j++) {
      image_setColor( src, i, j, White );
    }
  }
}

int main(int argc, char *argv[])
{
  Image *src;
  Module *scene;
  Module *formation;
  Module *creature;
  Module *head;
  Module *body;
  Module *legs;
  View3D view;
  Matrix vtm, gtm;
  DrawState *ds;
  Color blue = {{0.3, 0.3, 1}};

  // point_set3D( &(view.vrp), 80, 30, 40 );
  // vector_set( &(view.vpn), -1, -0.1, -0.1 );
  // vector_set( &(view.vup), 0, 1, 0 );
  
  point_set3D( &(view.vrp), 160, 140, 120 );
  vector_set( &(view.vpn), -20, -20, -20 );
  vector_set( &(view.vup), 0, 1, 0 );

  // point_set3D( &(view.vrp), 120, 70, 100 );
  // vector_set( &(view.vpn), -1, -10, -10 );
  // vector_set( &(view.vup), 0, 1, 0 );
  view.d = 1.5;
  view.du = 1.6;
  view.dv = 0.9;
  view.f = 1;
  view.b = 100;
  view.screenx = 1280;
  view.screeny = 720;


  matrix_setView3D( &vtm, &view );
  matrix_identity( &gtm );

  double leg_length = 7.0;
  legs = module_create();
  module_scale(legs, 3, 3, leg_length);
  module_translate(legs, 20, 20, 20);
  module_cube(legs, 1);
  module_translate(legs, 4, 0, 0);
  module_cube(legs, 1);

  double body_height = 5.0;
  body = module_create();
  module_scale(body, 8, 5, body_height);
  module_translate(body, 23, 21, 20+leg_length);
  module_cube(body, 1);
  module_identity(body);
  module_scale(body, 2, 2, 6);
  module_translate(body, 18, 20, 20+leg_length-1);
  module_cube(body, 1);
  module_translate(body, 9, 0, 0);
  module_cube(body, 1);

  head = module_create();
  module_scale(head, 4, 4, 4);
  module_translate(head, 23, 22.5, 20+leg_length+body_height+1);
  module_cube(head, 1);

  creature = module_create();
  module_module(creature, legs);
  module_module(creature, body);
  module_module(creature, head);

  formation = module_create();
  module_module(formation, creature);
  module_translate(formation, 15, 0, 0);
  module_module(formation, creature);
  module_translate(formation, -15, 0, 0);
  module_rotateZ(formation, 0, 1);
  module_translate(formation, 25, -7.5, 0);
  module_module(formation, creature);

  scene = module_create();
  module_translate(scene, -80, 20, 0 );
  module_module(scene, formation);
  module_translate(scene, 80, 20, 0);
  module_module(scene, formation);
  module_translate(scene, 0, 0, 0);
  module_module(scene, formation);

  src = image_create( 360, 640 );
  ds = drawstate_create();
  ds->shade = ShadeDepth;
  ds->color = blue;
  
  // draw into the scene
  module_draw( scene, &vtm, &gtm, ds, NULL, src );

  // write out the scene
  image_write( src, "../images/portfolio.ppm" );

  // cleanup
  module_delete(scene);
  module_delete(formation);
  module_delete(creature);
  module_delete(legs);
  free(ds);
  image_free( src );

  return(0);
}