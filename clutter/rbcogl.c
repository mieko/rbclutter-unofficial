/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2008  Neil Roberts
 * Copyright (C) 2010  Intel Corporation
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
#include <cogl/cogl.h>

#include "rbclutter.h"
#include "rbcoglhandle.h"

extern void rb_cogl_consts_init (void);

VALUE rbclt_c_cogl;

static VALUE
rb_cogl_get_features (VALUE self)
{
  return INT2NUM (cogl_get_features ());
}

static VALUE
rb_cogl_features_available (VALUE self, VALUE features)
{
  return cogl_features_available (NUM2INT (features)) ? Qtrue : Qfalse;
}

static VALUE
rb_cogl_get_bitmasks (VALUE self)
{
  int red, green, blue, alpha;

  cogl_get_bitmasks (&red, &green, &blue, &alpha);

  return rb_ary_new3 (4, INT2NUM (red), INT2NUM (green),
                      INT2NUM (blue), INT2NUM (alpha));
}

static VALUE
rb_cogl_push_matrix (VALUE self)
{
  cogl_push_matrix ();

  return Qnil;
}

static VALUE
rb_cogl_pop_matrix (VALUE self)
{
  cogl_pop_matrix ();

  return Qnil;
}

static VALUE
rb_cogl_perspective (VALUE self, VALUE fovy, VALUE aspect,
                     VALUE z_near, VALUE z_far)
{
  cogl_perspective (NUM2DBL (fovy),
                    NUM2DBL (aspect),
                    NUM2DBL (z_near),
                    NUM2DBL (z_far));

  return Qnil;
}

static VALUE
rb_cogl_scale (VALUE self, VALUE x, VALUE y, VALUE z)
{
  cogl_scale (NUM2DBL (x),
              NUM2DBL (y),
              NUM2DBL (z));

  return Qnil;
}

static VALUE
rb_cogl_translate (VALUE self, VALUE x, VALUE y, VALUE z)
{
  cogl_translate (NUM2DBL (x),
                  NUM2DBL (y),
                  NUM2DBL (z));

  return Qnil;
}

static VALUE
rb_cogl_rotate (VALUE self, VALUE angle, VALUE x, VALUE y, VALUE z)
{
  cogl_rotate (NUM2DBL (angle),
               NUM2INT (x), NUM2INT (y), NUM2INT (z));

  return Qnil;
}

static VALUE
rb_cogl_get_projection_matrix (VALUE self)
{
  /* TODO */
  return Qnil;
}

static VALUE
rb_cogl_get_modelview_matrix (VALUE self)
{
  /* TODO */
  return Qnil;
}

static VALUE
rb_cogl_get_viewport (VALUE self)
{
  gfloat vfloat[4], *src;
  VALUE vvalue[4], *dst = vvalue + 4;

  cogl_get_viewport (vfloat);

  for (src = vfloat + 4; src > vfloat;)
    *(--dst) = rb_float_new (*(--src));

  return rb_ary_new4 (4, vvalue);
}

static VALUE
rb_cogl_set_backface_culling_enabled (VALUE self, VALUE v)
{
  cogl_set_backface_culling_enabled (RTEST (v));

  return Qnil;
}

static VALUE
rb_cogl_set_depth_test_enabled (VALUE self, VALUE v)
{
  cogl_set_depth_test_enabled (RTEST (v));

  return Qnil;
}

static VALUE
rb_cogl_set_source (VALUE self, VALUE source)
{
  CoglHandle handle = rb_cogl_handle_get_handle (source);
  cogl_set_source (handle);
  return Qnil;
}

void
rb_cogl_init ()
{
  rbclt_c_cogl = rb_define_module ("Cogl");

  G_DEF_CLASS (COGL_TYPE_PIXEL_FORMAT, "PixelFormat", rbclt_c_cogl);

  rb_define_singleton_method (rbclt_c_cogl, "get_features",
                              rb_cogl_get_features, 0);
  rb_define_singleton_method (rbclt_c_cogl, "features_available?",
                              rb_cogl_features_available, 1);
  rb_define_singleton_method (rbclt_c_cogl, "get_bitmasks",
                              rb_cogl_get_bitmasks, 0);
  rb_define_singleton_method (rbclt_c_cogl, "push_matrix",
                              rb_cogl_push_matrix, 0);
  rb_define_singleton_method (rbclt_c_cogl, "pop_matrix",
                              rb_cogl_pop_matrix, 0);
  rb_define_singleton_method (rbclt_c_cogl, "perspective",
                              rb_cogl_perspective, 4);
  rb_define_singleton_method (rbclt_c_cogl, "scale",
                              rb_cogl_scale, 3);
  rb_define_singleton_method (rbclt_c_cogl, "translate",
                              rb_cogl_translate, 3);
  rb_define_singleton_method (rbclt_c_cogl, "rotate",
                              rb_cogl_rotate, 4);
  rb_define_singleton_method (rbclt_c_cogl, "get_projection_matrix",
                              rb_cogl_get_projection_matrix, 0);
  rb_define_singleton_method (rbclt_c_cogl, "get_modelview_matrix",
                              rb_cogl_get_modelview_matrix, 0);
  rb_define_singleton_method (rbclt_c_cogl, "get_viewport",
                              rb_cogl_get_viewport, 0);
  rb_define_singleton_method (rbclt_c_cogl, "set_depth_test_enabled",
                              rb_cogl_set_depth_test_enabled, 1);
  rb_define_singleton_method (rbclt_c_cogl, "set_backface_culling_enabled",
                              rb_cogl_set_backface_culling_enabled, 1);
  rb_define_singleton_method (rbclt_c_cogl, "set_source",
                              rb_cogl_set_source, 1);
}
