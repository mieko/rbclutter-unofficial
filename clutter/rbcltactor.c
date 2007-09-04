/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2007  Neil Roberts
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <rbgobject.h>
#include <clutter/clutter-actor.h>
#include <clutter/clutter-enum-types.h>

#include "rbclutter.h"

static VALUE
rbclt_actor_show (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_show (actor);
  return self;
}

static VALUE
rbclt_actor_show_all (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_show_all (actor);
  return self;
}

static VALUE
rbclt_actor_hide (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_hide (actor);
  return self;
}

static VALUE
rbclt_actor_hide_all (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_hide_all (actor);
  return self;
}

static VALUE
rbclt_actor_realize (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_realize (actor);
  return self;
}

static VALUE
rbclt_actor_unrealize (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_unrealize (actor);
  return self;
}

static VALUE
rbclt_actor_paint (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_paint (actor);
  return self;
}

static VALUE
rbclt_actor_pick (VALUE self, VALUE color)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_pick (actor, ((ClutterColor *) RVAL2BOXED (self, CLUTTER_TYPE_COLOR)));
  return self;
}

static VALUE
rbclt_actor_queue_redraw (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_queue_redraw (actor);
  return self;
}

static VALUE
rbclt_actor_destroy (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_destroy (actor);
  return self;
}

static VALUE
rbclt_actor_request_coords (VALUE self, VALUE box_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActorBox box = *(ClutterActorBox *) RVAL2BOXED (box_arg, CLUTTER_TYPE_ACTOR_BOX);
  clutter_actor_request_coords (actor, &box);
  return BOXED2RVAL (&box, CLUTTER_TYPE_ACTOR_BOX);
}

static VALUE
rbclt_actor_query_coords (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActorBox box;
  clutter_actor_query_coords (actor, &box);
  return BOXED2RVAL (&box, CLUTTER_TYPE_ACTOR_BOX);
}

static VALUE
rbclt_actor_geometry (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterGeometry geometry;
  clutter_actor_get_geometry (actor, &geometry);
  return BOXED2RVAL (&geometry, CLUTTER_TYPE_GEOMETRY);
}

static VALUE
rbclt_actor_set_geometry (VALUE self, VALUE geometry_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterGeometry *geometry = (ClutterGeometry *) RVAL2BOXED (geometry_arg, CLUTTER_TYPE_GEOMETRY);
  clutter_actor_set_geometry (actor, geometry);
  return self;
}

static VALUE
rbclt_actor_get_coords (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gint x1, y1, x2, y2;
  clutter_actor_get_coords (actor, &x1, &y1, &x2, &y2);
  return rb_ary_new3 (4, INT2NUM (x1), INT2NUM (y1), INT2NUM (x2), INT2NUM (y2));
}

static VALUE
rbclt_actor_set_size (VALUE self, VALUE width, VALUE height)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_size (actor, NUM2INT (width), NUM2INT (height));
  return self;
}

static VALUE
rbclt_actor_set_position (VALUE self, VALUE x, VALUE y)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_position (actor, NUM2INT (x), NUM2INT (y));
  return self;
}

static VALUE
rbclt_actor_abs_position (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gint x, y;
  clutter_actor_get_abs_position (actor, &x, &y);
  return rb_ary_new3 (2, INT2NUM (x), INT2NUM (y));
}

static VALUE
rbclt_actor_rotate_x (VALUE self, VALUE angle, VALUE y, VALUE z)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_rotate_x (actor, NUM2DBL (angle), NUM2INT (y), NUM2INT (z));
  return self;
}

static VALUE
rbclt_actor_rotate_y (VALUE self, VALUE angle, VALUE x, VALUE z)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_rotate_y (actor, NUM2DBL (angle), NUM2INT (x), NUM2INT (z));
  return self;
}

static VALUE
rbclt_actor_rotate_z (VALUE self, VALUE angle, VALUE x, VALUE y)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_rotate_z (actor, NUM2DBL (angle), NUM2INT (x), NUM2INT (y));
  return self;
}

static VALUE
rbclt_actor_rxang (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return rb_float_new (clutter_actor_get_rxang (actor));
}

static VALUE
rbclt_actor_ryang (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return rb_float_new (clutter_actor_get_ryang (actor));
}

static VALUE
rbclt_actor_rzang (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return rb_float_new (clutter_actor_get_rzang (actor));
}

static VALUE
rbclt_actor_actor_id (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return UINT2NUM (clutter_actor_get_id (actor));
}

static VALUE
rbclt_actor_remove_clip (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_remove_clip (actor);
  return self;
}

static VALUE
rbclt_actor_set_parent (VALUE self, VALUE parent_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActor *parent = CLUTTER_ACTOR (RVAL2GOBJ (parent_arg));
  clutter_actor_set_parent (actor, parent);
  return self;
}

static VALUE
rbclt_actor_parent (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return GOBJ2RVAL (clutter_actor_get_parent (actor));
}

static VALUE
rbclt_actor_reparent (VALUE self, VALUE parent_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActor *parent = CLUTTER_ACTOR (RVAL2GOBJ (parent_arg));
  clutter_actor_reparent (actor, parent);
  return self;
}

static VALUE
rbclt_actor_unparent (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_unparent (actor);
  return self;
}

static VALUE
rbclt_actor_raise (VALUE self, VALUE below_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActor *below = CLUTTER_ACTOR (RVAL2GOBJ (below_arg));
  clutter_actor_raise (actor, below);
  return self;
}

static VALUE
rbclt_actor_lower (VALUE self, VALUE above_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActor *above = CLUTTER_ACTOR (RVAL2GOBJ (above_arg));
  clutter_actor_lower (actor, above);
  return self;
}

static VALUE
rbclt_actor_raise_top (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_raise_top (actor);
  return self;
}

static VALUE
rbclt_actor_lower_bottom (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_lower_bottom (actor);
  return self;
}

static VALUE
rbclt_actor_set_depth (VALUE self, VALUE depth)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_depth (actor, NUM2INT (depth));
  return self;
}

static VALUE
rbclt_actor_depth (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return INT2NUM (clutter_actor_get_depth (actor));
}

static VALUE
rbclt_actor_set_scale (int argc, VALUE *argv, VALUE self)
{
  VALUE scale_x, scale_y, gravity;
  rb_scan_args (argc, argv, "21", &scale_x, &scale_y, &gravity);
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  if (gravity == Qnil)
    clutter_actor_set_scale (actor, NUM2DBL (scale_x), NUM2DBL (scale_y));
  else
    clutter_actor_set_scale_with_gravity (actor, NUM2DBL (scale_x), NUM2DBL (scale_y),
					  RVAL2GENUM (gravity, CLUTTER_TYPE_GRAVITY));
  return self;
}

static VALUE
rbclt_actor_scale (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gdouble scale_x, scale_y;
  clutter_actor_get_scale (actor, &scale_x, &scale_y);
  return rb_ary_new3 (2, rb_float_new (scale_x), rb_float_new (scale_y));
}

static VALUE
rbclt_actor_abs_size (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  guint width, height;
  clutter_actor_get_abs_size (actor, &width, &height);
  return rb_ary_new3 (2, UINT2NUM (width), UINT2NUM (height));
}

static VALUE
rbclt_actor_size (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  guint width, height;
  clutter_actor_get_size (actor, &width, &height);
  return rb_ary_new3 (2, UINT2NUM (width), UINT2NUM (height));
}

static VALUE
rbclt_actor_move_by (VALUE self, VALUE dx, VALUE dy)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_move_by (actor, NUM2INT (dx), NUM2INT (dy));
  return self;
}

static VALUE
rbclt_actor_vertices (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterVertex vertices[4];
  clutter_actor_get_vertices (actor, vertices);
  return rb_ary_new3 (4, BOXED2RVAL (vertices, CLUTTER_TYPE_VERTEX),
		      BOXED2RVAL (vertices + 1, CLUTTER_TYPE_VERTEX),
		      BOXED2RVAL (vertices + 2, CLUTTER_TYPE_VERTEX),
		      BOXED2RVAL (vertices + 3, CLUTTER_TYPE_VERTEX));
}

static VALUE
rbclt_actor_apply_transform_to_point (VALUE self, VALUE point_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterVertex point = *(ClutterVertex *) RVAL2BOXED (point_arg, CLUTTER_TYPE_VERTEX);
  ClutterVertex vertex;
  clutter_actor_apply_transform_to_point (actor, &point, &vertex);
  return BOXED2RVAL (&point, CLUTTER_TYPE_VERTEX);
}

void
rbclt_actor_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_TYPE_ACTOR, "Actor", rbclt_c_clutter);

  rb_define_method (klass, "show", rbclt_actor_show, 0);
  rb_define_method (klass, "show_all", rbclt_actor_show_all, 0);
  rb_define_method (klass, "hide", rbclt_actor_hide, 0);
  rb_define_method (klass, "hide_all", rbclt_actor_hide_all, 0);
  rb_define_method (klass, "realize", rbclt_actor_realize, 0);
  rb_define_method (klass, "unrealize", rbclt_actor_unrealize, 0);
  rb_define_method (klass, "paint", rbclt_actor_paint, 0);
  rb_define_method (klass, "pick", rbclt_actor_pick, 1);
  rb_define_method (klass, "queue_redraw", rbclt_actor_queue_redraw, 0);
  rb_define_method (klass, "destroy", rbclt_actor_destroy, 0);
  rb_define_method (klass, "request_coords", rbclt_actor_request_coords, 1);
  rb_define_method (klass, "query_coords", rbclt_actor_query_coords, 0);
  rb_define_method (klass, "geometry", rbclt_actor_geometry, 0);
  rb_define_method (klass, "set_geometry", rbclt_actor_set_geometry, 1);
  rb_define_method (klass, "coords", rbclt_actor_get_coords, 0);
  rb_define_method (klass, "set_size", rbclt_actor_set_size, 2);
  rb_define_method (klass, "set_position", rbclt_actor_set_position, 2);
  rb_define_method (klass, "abs_position", rbclt_actor_abs_position, 0);
  rb_define_method (klass, "rotate_x", rbclt_actor_rotate_x, 3);
  rb_define_method (klass, "rotate_y", rbclt_actor_rotate_y, 3);
  rb_define_method (klass, "rotate_z", rbclt_actor_rotate_z, 3);
  rb_define_method (klass, "rxang", rbclt_actor_rxang, 0);
  rb_define_method (klass, "ryang", rbclt_actor_ryang, 0);
  rb_define_method (klass, "rzang", rbclt_actor_rzang, 0);
  rb_define_method (klass, "actor_id", rbclt_actor_actor_id, 0);
  rb_define_method (klass, "remove_clip", rbclt_actor_remove_clip, 0);
  rb_define_method (klass, "set_parent", rbclt_actor_set_parent, 1);
  rb_define_method (klass, "parent", rbclt_actor_parent, 0);
  rb_define_method (klass, "reparent", rbclt_actor_reparent, 1);
  rb_define_method (klass, "unparent", rbclt_actor_unparent, 0);
  rb_define_method (klass, "raise", rbclt_actor_raise, 1);
  rb_define_method (klass, "lower", rbclt_actor_lower, 1);
  rb_define_method (klass, "raise_top", rbclt_actor_raise_top, 0);
  rb_define_method (klass, "lower_bottom", rbclt_actor_lower_bottom, 0);
  rb_define_method (klass, "set_depth", rbclt_actor_set_depth, 1);
  rb_define_method (klass, "depth", rbclt_actor_depth, 0);
  rb_define_method (klass, "set_scale", rbclt_actor_set_scale, -1);
  rb_define_method (klass, "scale", rbclt_actor_scale, 0);
  rb_define_method (klass, "abs_size", rbclt_actor_abs_size, 0);
  rb_define_method (klass, "size", rbclt_actor_size, 0);
  rb_define_method (klass, "move_by", rbclt_actor_move_by, 2);
  rb_define_method (klass, "vertices", rbclt_actor_vertices, 0);
  rb_define_method (klass, "apply_transform_to_point", rbclt_actor_apply_transform_to_point, 1);
  
  G_DEF_SETTERS (klass);
}
