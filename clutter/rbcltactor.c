/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2007-2008  Neil Roberts
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

#include <rbgobject.h>
#include <clutter/clutter.h>

#include "rbclutter.h"
#include "rbcoglmatrix.h"

VALUE rbclt_c_actor;

static VALUE
rbclt_actor_subclass_initialize (int argc, VALUE *argv, VALUE self)
{
  GType gtype = RVAL2GTYPE (self);
  ClutterActor *actor;

  if (gtype == CLUTTER_TYPE_ACTOR)
    rb_raise (rb_eArgError, "Clutter::Actor can not be instantiated directly");

  /* Chain up to let GLib::Object.new create the right instance */
  rb_call_super (argc, argv);

  /* Claim the floating reference if it hasn't been claimed
     already. Doing it conditionally hopefully protects against the
     GObject bindings being fixed one day so that they understand
     GInitiallyUnowned */
  actor = RVAL2GOBJ (self);
  if (g_object_is_floating (actor))
    g_object_ref_sink (actor);

  return Qnil;
}

static VALUE
rbclt_actor_type_register (int argc, VALUE *argv, VALUE self)
{
  VALUE result;
  VALUE mod;

  /* GObject::type_register will define and include a stub module to
     override the initialize constructor with one that calls
     gobj_initialize. That function isn't good enough for ClutterActor
     because it doesn't understand GInitiallyUnowned so it doesn't
     claim the floating reference. So when the actor is added to a
     container the container will steal Ruby's reference. To work
     around this we include another stub module to redefine the
     initialize method again */

  /* Let the real type_register function run */
  result = rb_call_super (argc, argv);

  /* Define a stub module specifically for this new subclass */
  mod = rb_define_module_under (self, "ClutterGObjectHook__");
  /* Give it a constructor */
  rb_define_method (mod, "initialize",
                    rbclt_actor_subclass_initialize, -1);
  /* Make the new class include the module */
  rb_include_module (self, mod);

  return result;
}

static void
rbclt_actor_mark (void *p)
{
  ClutterActor *actor = CLUTTER_ACTOR (p);
  ClutterAnimation *anim;

  /* We need to mark the animation object because this is accessible
     from Ruby so it may have a Ruby proxy object attached. The
     animation object is not a property so it wouldn't get marked any
     other way */

  if ((anim = clutter_actor_get_animation (actor)))
    rbgobj_gc_mark_instance (anim);
}

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
rbclt_actor_map (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_map (actor);
  return self;
}

static VALUE
rbclt_actor_unmap (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_unmap (actor);
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
rbclt_actor_queue_redraw (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_queue_redraw (actor);
  return self;
}

static VALUE
rbclt_actor_queue_relayout (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_queue_relayout (actor);
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
rbclt_actor_get_preferred_width (int argc, VALUE *argv, VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  VALUE for_height;
  gfloat min_width, natural_width;

  rb_scan_args (argc, argv, "01", &for_height);

  clutter_actor_get_preferred_width (actor,
                                     NIL_P (for_height)
                                     ? -1 : NUM2INT (for_height),
                                     &min_width, &natural_width);
  return rb_ary_new3 (2, rb_float_new (min_width),
                      rb_float_new (natural_width));
}

static VALUE
rbclt_actor_get_preferred_height (int argc, VALUE *argv, VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  VALUE for_width;
  gfloat min_height, natural_height;

  rb_scan_args (argc, argv, "01", &for_width);

  clutter_actor_get_preferred_height (actor,
                                      NIL_P (for_width)
                                      ? -1 : NUM2INT (for_width),
                                      &min_height, &natural_height);
  return rb_ary_new3 (2,
                      rb_float_new (min_height),
                      rb_float_new (natural_height));
}

static VALUE
rbclt_actor_get_preferred_size (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gfloat min_width, min_height, natural_width, natural_height;

  clutter_actor_get_preferred_size (actor, &min_width, &min_height,
                                    &natural_width, &natural_height);

  return rb_ary_new3 (4, rb_float_new (min_width),
                      rb_float_new (min_height),
                      rb_float_new (natural_width),
                      rb_float_new (natural_height));
}

static VALUE
rbclt_actor_allocate (VALUE self, VALUE box_arg, VALUE flags)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActorBox box
    = *(ClutterActorBox *) RVAL2BOXED (box_arg, CLUTTER_TYPE_ACTOR_BOX);

  clutter_actor_allocate (actor, &box,
                          RVAL2GFLAGS (flags, CLUTTER_TYPE_ALLOCATION_FLAGS));

  return self;
}

static VALUE
rbclt_actor_allocate_preferred_size (VALUE self, VALUE flags_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterAllocationFlags flags
    = RVAL2GFLAGS (flags_arg, CLUTTER_TYPE_ALLOCATION_FLAGS);

  clutter_actor_allocate_preferred_size (actor, flags);

  return self;
}

static VALUE
rbclt_actor_allocate_available_size (VALUE self,
                                     VALUE x,
                                     VALUE y,
                                     VALUE available_width,
                                     VALUE available_height,
                                     VALUE flags_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterAllocationFlags flags
    = RVAL2GFLAGS (flags_arg, CLUTTER_TYPE_ALLOCATION_FLAGS);

  clutter_actor_allocate_available_size (actor,
                                         NUM2DBL (x),
                                         NUM2DBL (y),
                                         NUM2DBL (available_width),
                                         NUM2DBL (available_height),
                                         flags);

  return self;
}

static VALUE
rbclt_actor_get_allocation_box (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActorBox box;

  clutter_actor_get_allocation_box (actor, &box);

  return BOXED2RVAL (&box, CLUTTER_TYPE_ACTOR_BOX);
}

static VALUE
rbclt_actor_get_allocation_geometry (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterGeometry geometry;

  clutter_actor_get_allocation_geometry (actor, &geometry);

  return BOXED2RVAL (&geometry, CLUTTER_TYPE_GEOMETRY);
}

static VALUE
rbclt_actor_get_allocation_vertices (int argc, VALUE *argv, VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActor *ancestor;
  ClutterVertex verts[4];
  VALUE ancestor_arg;
  VALUE values[4];
  int i;

  rb_scan_args (argc, argv, "01", &ancestor_arg);
  ancestor = NIL_P (ancestor_arg)
    ? NULL : CLUTTER_ACTOR (RVAL2GOBJ (ancestor_arg));

  clutter_actor_get_allocation_vertices (actor, ancestor, verts);

  for (i = 0; i < 4; i++)
    values[i] = BOXED2RVAL (verts + i, CLUTTER_TYPE_VERTEX);

  return rb_ary_new4 (4, values);
}

static VALUE
rbclt_actor_get_abs_allocation_vertices (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterVertex verts[4];
  VALUE values[4];
  int i;

  clutter_actor_get_abs_allocation_vertices (actor, verts);

  for (i = 0; i < 4; i++)
    values[i] = BOXED2RVAL (verts + i, CLUTTER_TYPE_VERTEX);

  return rb_ary_new4 (4, values);
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
rbclt_actor_set_size (VALUE self, VALUE width, VALUE height)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_size (actor, NUM2INT (width), NUM2INT (height));
  return self;
}

static VALUE
rbclt_actor_get_transformed_size (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gfloat width, height;
  clutter_actor_get_transformed_size (actor, &width, &height);
  return rb_ary_new3 (2, rb_float_new (width), rb_float_new (height));
}

static VALUE
rbclt_actor_set_position (VALUE self, VALUE x, VALUE y)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_position (actor, NUM2INT (x), NUM2INT (y));
  return self;
}

static VALUE
rbclt_actor_get_position (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gfloat x, y;
  clutter_actor_get_position (actor, &x, &y);
  return rb_ary_new3 (2, rb_float_new (x), rb_float_new (y));
}

static VALUE
rbclt_actor_get_transformed_position (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gfloat x, y;
  clutter_actor_get_transformed_position (actor, &x, &y);
  return rb_ary_new3 (2, rb_float_new (x), rb_float_new (y));
}

static VALUE
rbclt_actor_set_rotation (VALUE self, VALUE axis, VALUE angle,
                          VALUE x, VALUE y, VALUE z)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_rotation (actor,
                              RVAL2GENUM (axis, CLUTTER_TYPE_ROTATE_AXIS),
                              NUM2DBL (angle),
                              NUM2INT (x),
                              NUM2INT (y),
                              NUM2INT (z));
  return self;
}

static VALUE
rbclt_actor_set_z_rotation_from_gravity (VALUE self,
                                         VALUE angle,
                                         VALUE gravity_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterGravity gravity = RVAL2GENUM (gravity_arg, CLUTTER_TYPE_GRAVITY);
  clutter_actor_set_z_rotation_from_gravity (actor, NUM2DBL (angle), gravity);
  return self;
}

static VALUE
rbclt_actor_get_rotation (VALUE self, VALUE axis)
{
  gdouble angle;
  gfloat x = 0, y = 0, z = 0;
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));

  angle = clutter_actor_get_rotation (actor,
                                      RVAL2GENUM (axis,
                                                  CLUTTER_TYPE_ROTATE_AXIS),
                                      &x, &y, &z);

  return rb_ary_new3 (4, rb_float_new (angle),
                      rb_float_new (x), rb_float_new (y), rb_float_new (z));
}

static VALUE
rbclt_actor_get_paint_opacity (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return INT2NUM (clutter_actor_get_paint_opacity (actor));
}

static VALUE
rbclt_actor_get_paint_visibility (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return clutter_actor_get_paint_visibility (actor) ? Qtrue : Qfalse;
}

static VALUE
rbclt_actor_gid (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return UINT2NUM (clutter_actor_get_gid (actor));
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
rbclt_actor_set_scale (int argc, VALUE *argv, VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  VALUE scale_x, scale_y, scale_center_x, scale_center_y;

  switch (rb_scan_args (argc, argv, "22",
                        &scale_x, &scale_y, &scale_center_x, &scale_center_y))
    {
    case 2:
      clutter_actor_set_scale (actor, NUM2DBL (scale_x), NUM2DBL (scale_y));
      break;

    case 3:
      clutter_actor_set_scale_with_gravity (actor,
                                            NUM2DBL (scale_x),
                                            NUM2DBL (scale_y),
                                            RVAL2GENUM (scale_center_x,
                                                        CLUTTER_TYPE_GRAVITY));
      break;

    case 4:
      clutter_actor_set_scale_full (actor, NUM2DBL (scale_x), NUM2DBL (scale_y),
                                    NUM2DBL (scale_center_x),
                                    NUM2DBL (scale_center_y));
      break;
    }

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
rbclt_actor_get_scale_center (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gfloat scale_center_x, scale_center_y;
  clutter_actor_get_scale_center (actor, &scale_center_x, &scale_center_y);
  return rb_ary_new3 (2,
                      rb_float_new (scale_center_x),
                      rb_float_new (scale_center_y));
}

static VALUE
rbclt_actor_size (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gfloat width, height;
  clutter_actor_get_size (actor, &width, &height);
  return rb_ary_new3 (2, rb_float_new (width), rb_float_new (height));
}

static VALUE
rbclt_actor_move_by (VALUE self, VALUE dx, VALUE dy)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_move_by (actor, NUM2INT (dx), NUM2INT (dy));
  return self;
}

static VALUE
rbclt_actor_apply_transform_to_point (VALUE self, VALUE point_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterVertex point = *(ClutterVertex *) RVAL2BOXED (point_arg,
                                                       CLUTTER_TYPE_VERTEX);
  ClutterVertex vertex;
  clutter_actor_apply_transform_to_point (actor, &point, &vertex);
  return BOXED2RVAL (&vertex, CLUTTER_TYPE_VERTEX);
}

static VALUE
rbclt_actor_apply_relative_transform_to_point (VALUE self,
                                               VALUE ancestor,
                                               VALUE point_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterVertex point = *(ClutterVertex *) RVAL2BOXED (point_arg,
                                                       CLUTTER_TYPE_VERTEX);
  ClutterVertex vertex;

  clutter_actor_apply_relative_transform_to_point (actor,
                                                   RVAL2GOBJ (ancestor),
                                                   &point, &vertex);

  return BOXED2RVAL (&vertex, CLUTTER_TYPE_VERTEX);
}

static VALUE
rbclt_actor_event (VALUE self, VALUE event_arg, VALUE use_capture)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterEvent *event = (ClutterEvent *) RVAL2BOXED (event_arg,
                                                     CLUTTER_TYPE_EVENT);

  clutter_actor_event (actor, event, RTEST (use_capture));

  return self;
}

static VALUE
rbclt_actor_should_pick_paint (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return clutter_actor_should_pick_paint (actor) ? Qtrue : Qfalse;
}

static VALUE
rbclt_actor_set_shader (VALUE self, VALUE shader)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_shader (actor, CLUTTER_SHADER (RVAL2GOBJ (shader)));
  return self;
}

static VALUE
rbclt_actor_shader (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return GOBJ2RVAL (clutter_actor_get_shader (actor));
}

static VALUE
rbclt_actor_set_shader_param (VALUE self, VALUE param, VALUE value)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  GValue gval = { 0, };
  g_value_init (&gval, RVAL2GTYPE (value));
  clutter_actor_set_shader_param (actor, StringValuePtr (param), &gval);
  g_value_unset (&gval);
  return self;
}

static VALUE
rbclt_actor_set_anchor_point (VALUE self, VALUE x, VALUE y)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_anchor_point (actor, NUM2INT (x), NUM2INT (y));
  return self;
}

static VALUE
rbclt_actor_move_anchor_point (VALUE self, VALUE x, VALUE y)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_move_anchor_point (actor, NUM2INT (x), NUM2INT (y));
  return self;
}

static VALUE
rbclt_actor_set_anchor_point_from_gravity (VALUE self, VALUE gravity)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterGravity gravity_val = RVAL2GENUM (gravity, CLUTTER_TYPE_GRAVITY);
  clutter_actor_set_anchor_point_from_gravity (actor, gravity_val);
  return self;
}

static VALUE
rbclt_actor_get_anchor_point (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gfloat anchor_x, anchor_y;

  clutter_actor_get_anchor_point (actor, &anchor_x, &anchor_y);

  return rb_ary_new3 (2, rb_float_new (anchor_x), rb_float_new (anchor_y));
}

static VALUE
rbclt_actor_move_anchor_point_from_gravity (VALUE self, VALUE gravity)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterGravity gravity_val = RVAL2GENUM (gravity, CLUTTER_TYPE_GRAVITY);
  clutter_actor_move_anchor_point_from_gravity (actor, gravity_val);
  return self;
}

static VALUE
rbclt_actor_transform_stage_point (VALUE self, VALUE x, VALUE y)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  gfloat x_out, y_out;

  clutter_actor_transform_stage_point (actor,
                                       NUM2DBL (x), NUM2DBL (y),
                                       &x_out, &y_out);

  return rb_ary_new3 (2, rb_float_new (x_out), rb_float_new (y_out));
}

static VALUE
rbclt_actor_is_rotated (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return clutter_actor_is_rotated (actor) ? Qtrue : Qfalse;
}

static VALUE
rbclt_actor_is_scaled (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return clutter_actor_is_scaled (actor) ? Qtrue : Qfalse;
}

static VALUE
rbclt_actor_stage (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return GOBJ2RVAL (clutter_actor_get_stage (actor));
}

static VALUE
rbclt_actor_grab_key_focus (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_grab_key_focus (actor);
  return self;
}

static VALUE
rbclt_actor_get_pango_context (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return GOBJ2RVAL (clutter_actor_get_pango_context (actor));
}

static VALUE
rbclt_actor_create_pango_context (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return GOBJ2RVALU (clutter_actor_create_pango_context (actor));
}

static VALUE
rbclt_actor_create_pango_layout (VALUE self, VALUE text_arg)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  const gchar *text = StringValuePtr (text_arg);
  return GOBJ2RVALU (clutter_actor_create_pango_layout (actor, text));
}

static VALUE
rbclt_actor_get_transformation_matrix (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  VALUE result = rb_cogl_matrix_alloc ();
  CoglMatrix *matrix = rb_cogl_matrix_get_pointer (result);

  clutter_actor_get_transformation_matrix (actor, matrix);

  return result;
}

static VALUE
rbclt_actor_is_in_clone_paint (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));

  return clutter_actor_is_in_clone_paint (actor) ? Qtrue : Qfalse;
}

static VALUE
rbclt_actor_has_pointer (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));

  return clutter_actor_has_pointer (actor) ? Qtrue : Qfalse;
}

static VALUE
rbclt_actor_set_flags (VALUE self, VALUE flags)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_set_flags (actor, RVAL2GFLAGS (flags,
                                               CLUTTER_TYPE_ACTOR_FLAGS));
  return self;
}

static VALUE
rbclt_actor_unset_flags (VALUE self, VALUE flags)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  clutter_actor_unset_flags (actor,
                             RVAL2GFLAGS (flags, CLUTTER_TYPE_ACTOR_FLAGS));
  return self;
}

static VALUE
rbclt_actor_get_flags (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  ClutterActorFlags flags = clutter_actor_get_flags (actor);
  return GFLAGS2RVAL (flags, CLUTTER_TYPE_ACTOR_FLAGS);
}

static VALUE
rbclt_get_actor_by_gid (VALUE self, VALUE gid)
{
  ClutterActor *actor = clutter_get_actor_by_gid (NUM2UINT (gid));
  return GOBJ2RVAL (actor);
}

static VALUE
rbclt_actor_push_internal (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));

  clutter_actor_push_internal (actor);

  return self;
}

static VALUE
rbclt_actor_pop_internal (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));

  clutter_actor_pop_internal (actor);

  return self;
}

void
rbclt_actor_init ()
{
  VALUE klass = G_DEF_CLASS2 (CLUTTER_TYPE_ACTOR, "Actor", rbclt_c_clutter,
                              rbclt_actor_mark, NULL);

  rbclt_c_actor = klass;

  rb_define_singleton_method (klass, "type_register",
                              rbclt_actor_type_register, -1);

  G_DEF_CLASS (CLUTTER_TYPE_ACTOR_FLAGS, "Flags", klass);
  G_DEF_CONSTANTS (klass, CLUTTER_TYPE_ACTOR_FLAGS, "CLUTTER_ACTOR_");

  rb_define_method (klass, "show", rbclt_actor_show, 0);
  rb_define_method (klass, "show_all", rbclt_actor_show_all, 0);
  rb_define_method (klass, "hide", rbclt_actor_hide, 0);
  rb_define_method (klass, "hide_all", rbclt_actor_hide_all, 0);
  rb_define_method (klass, "realize", rbclt_actor_realize, 0);
  rb_define_method (klass, "unrealize", rbclt_actor_unrealize, 0);
  rb_define_method (klass, "map", rbclt_actor_map, 0);
  rb_define_method (klass, "unmap", rbclt_actor_unmap, 0);
  rb_define_method (klass, "paint", rbclt_actor_paint, 0);
  rb_define_method (klass, "queue_redraw", rbclt_actor_queue_redraw, 0);
  rb_define_method (klass, "queue_relayout", rbclt_actor_queue_relayout, 0);
  rb_define_method (klass, "destroy", rbclt_actor_destroy, 0);
  rb_define_method (klass, "get_preferred_width",
                    rbclt_actor_get_preferred_width, -1);
  rb_define_alias (klass, "preferred_width", "get_preferred_width");
  rb_define_method (klass, "get_preferred_height",
                    rbclt_actor_get_preferred_height, -1);
  rb_define_alias (klass, "preferred_height", "get_preferred_height");
  rb_define_method (klass, "preferred_size",
                    rbclt_actor_get_preferred_size, 0);
  rb_define_method (klass, "allocate", rbclt_actor_allocate, 2);
  rb_define_method (klass, "allocate_preferred_size",
                    rbclt_actor_allocate_preferred_size, 1);
  rb_define_method (klass, "allocate_available_size",
                    rbclt_actor_allocate_available_size, 5);
  rb_define_method (klass, "allocation_box",
                    rbclt_actor_get_allocation_box, 0);
  rb_define_method (klass, "allocation_geometry",
                    rbclt_actor_get_allocation_geometry, 0);
  rb_define_method (klass, "get_allocation_vertices",
                    rbclt_actor_get_allocation_vertices, -1);
  rb_define_alias (klass, "allocation_vertices", "get_allocation_vertices");
  rb_define_method (klass, "abs_allocation_vertices",
                    rbclt_actor_get_abs_allocation_vertices, 0);
  rb_define_method (klass, "geometry", rbclt_actor_geometry, 0);
  rb_define_method (klass, "set_geometry", rbclt_actor_set_geometry, 1);
  rb_define_method (klass, "set_size", rbclt_actor_set_size, 2);
  rb_define_method (klass, "set_position", rbclt_actor_set_position, 2);
  rb_define_method (klass, "position", rbclt_actor_get_position, 0);
  rb_define_method (klass, "transformed_size",
                    rbclt_actor_get_transformed_size, 0);
  rb_define_method (klass, "transformed_position",
                    rbclt_actor_get_transformed_position, 0);
  rb_define_method (klass, "set_rotation", rbclt_actor_set_rotation, 5);
  rb_define_method (klass, "set_z_rotation_from_gravity",
                    rbclt_actor_set_z_rotation_from_gravity, 2);
  rb_define_alias (klass, "z_rotation_gravity",
                   "rotation_center_z_gravity");
  rb_define_method (klass, "get_rotation", rbclt_actor_get_rotation, 1);
  rb_define_method (klass, "paint_opacity", rbclt_actor_get_paint_opacity, 0);
  rb_define_method (klass, "paint_visibility",
                    rbclt_actor_get_paint_visibility, 0);
  rb_define_method (klass, "gid", rbclt_actor_gid, 0);
  rb_define_method (klass, "remove_clip", rbclt_actor_remove_clip, 0);
  rb_define_method (klass, "set_parent", rbclt_actor_set_parent, 1);
  rb_define_method (klass, "parent", rbclt_actor_parent, 0);
  rb_define_method (klass, "reparent", rbclt_actor_reparent, 1);
  rb_define_method (klass, "unparent", rbclt_actor_unparent, 0);
  rb_define_method (klass, "raise", rbclt_actor_raise, 1);
  rb_define_method (klass, "lower", rbclt_actor_lower, 1);
  rb_define_method (klass, "raise_top", rbclt_actor_raise_top, 0);
  rb_define_method (klass, "lower_bottom", rbclt_actor_lower_bottom, 0);
  rb_define_method (klass, "set_scale", rbclt_actor_set_scale, -1);
  rb_define_method (klass, "scale", rbclt_actor_scale, 0);
  rb_define_method (klass, "scale_center", rbclt_actor_get_scale_center, 0);
  rb_define_method (klass, "size", rbclt_actor_size, 0);
  rb_define_method (klass, "move_by", rbclt_actor_move_by, 2);
  rb_define_method (klass, "event", rbclt_actor_event, 2);
  rb_define_method (klass, "apply_transform_to_point",
                    rbclt_actor_apply_transform_to_point, 1);
  rb_define_method (klass, "apply_relative_transform_to_point",
                    rbclt_actor_apply_relative_transform_to_point, 2);
  rb_define_method (klass, "should_pick_paint",
                    rbclt_actor_should_pick_paint, 0);
  rb_define_alias (klass, "pick_paint?", "should_pick_paint");
  rb_define_method (klass, "set_shader", rbclt_actor_set_shader, 1);
  rb_define_method (klass, "shader", rbclt_actor_shader, 0);
  rb_define_method (klass, "set_shader_param", rbclt_actor_set_shader_param, 2);
  rb_define_method (klass, "set_anchor_point", rbclt_actor_set_anchor_point, 2);
  rb_define_method (klass, "move_anchor_point",
                    rbclt_actor_move_anchor_point, 2);
  rb_define_method (klass, "anchor_point",
                    rbclt_actor_get_anchor_point, 0);
  rb_define_method (klass, "set_anchor_point_from_gravity",
                    rbclt_actor_set_anchor_point_from_gravity, 1);
  rb_define_method (klass, "move_anchor_point_from_gravity",
                    rbclt_actor_move_anchor_point_from_gravity, 1);
  rb_define_method (klass, "transform_stage_point",
                    rbclt_actor_transform_stage_point, 2);
  rb_define_method (klass, "rotated?", rbclt_actor_is_rotated, 0);
  rb_define_method (klass, "scaled?", rbclt_actor_is_scaled, 0);
  rb_define_method (klass, "stage", rbclt_actor_stage, 0);

  rb_define_method (klass, "grab_key_focus",
                    rbclt_actor_grab_key_focus, 0);

  rb_define_method (klass, "pango_context",
                    rbclt_actor_get_pango_context, 0);
  rb_define_method (klass, "create_pango_context",
                    rbclt_actor_create_pango_context, 0);
  rb_define_method (klass, "create_pango_layout",
                    rbclt_actor_create_pango_layout, 1);

  rb_define_method (klass, "transformation_matrix",
                    rbclt_actor_get_transformation_matrix, 0);

  rb_define_method (klass, "in_clone_paint?",
                    rbclt_actor_is_in_clone_paint, 0);
  rb_define_method (klass, "has_pointer?",
                    rbclt_actor_has_pointer, 0);

  rb_define_method (klass, "push_internal", rbclt_actor_push_internal, 0);
  rb_define_method (klass, "pop_internal", rbclt_actor_pop_internal, 0);

  /* This function adds the flags in so we don't want to call it flags= */
  rb_define_method (klass, "set_flags", rbclt_actor_set_flags, 1);
  rb_define_method (klass, "unset_flags", rbclt_actor_unset_flags, 1);
  rb_define_method (klass, "flags", rbclt_actor_get_flags, 0);

  rb_define_singleton_method (rbclt_c_clutter, "get_actor_by_gid",
                              rbclt_get_actor_by_gid, 1);

  G_DEF_SETTERS (klass);
}
