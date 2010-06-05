/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2007-2008  Neil Roberts
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

#include <ruby.h>
#include <rbgobject.h>
#include <rb_cairo.h>
#include <cairo.h>
#include <glib-object.h>
#include <clutter/clutter.h>

#include "rbclutter.h"

static ID id_finish;

static VALUE rbclt_cairo_texture_context_finish (VALUE self);

static VALUE
rbclt_cairo_texture_initialize (VALUE self, VALUE width, VALUE height)
{
  ClutterActor *actor
    = clutter_cairo_texture_new (NUM2UINT (width), NUM2UINT (height));

  rbclt_initialize_unowned (self, actor);

  return Qnil;
}

static VALUE
rbclt_cairo_texture_invoke_finish (VALUE self)
{
  return rb_funcall (self, id_finish, 0);
}

static VALUE
rbclt_cairo_texture_set_source_color (VALUE self, VALUE cairo, VALUE color_arg)
{
  ClutterColor *color = RVAL2BOXED (color_arg, CLUTTER_TYPE_COLOR);

  clutter_cairo_set_source_color (RVAL2CRCONTEXT (cairo), color);

  return cairo;
}

static VALUE
rbclt_cairo_texture_create (int argc, VALUE *argv, VALUE self)
{
  ClutterCairoTexture *cairo = CLUTTER_CAIRO_TEXTURE (RVAL2GOBJ (self));
  cairo_t *cr;
  VALUE ret;

  if (argc == 0)
    cr = clutter_cairo_texture_create (cairo);
  else if (argc == 4)
    cr = clutter_cairo_texture_create_region (cairo, NUM2INT (argv[0]),
                                              NUM2INT (argv[1]),
                                              NUM2UINT (argv[2]),
                                              NUM2UINT (argv[3]));
  else
    rb_raise (rb_eArgError,
              "wrong number of arguments (%i for 0 or 4)", argc);


  ret = CRCONTEXT2RVAL (cr);
  /* The context returned by clutter_cairo_texture_create has a reference
     owned by us, but CRCONTEXT2RVAL creates another reference held by
     Ruby, so we should drop the original reference to let Ruby handle
     it */
  cairo_destroy (cr);

  /* If a block is given then evaluate that block with the context and
     call finish when it is over */
  if (rb_block_given_p ())
    return rb_ensure (rb_yield, ret, rbclt_cairo_texture_invoke_finish, ret);
  else
    return ret;
}

static VALUE
rbclt_cairo_texture_context_finish (VALUE self)
{
  cairo_surface_t *surface;
  cairo_t *new_cr, *old_cr;

  /* Make a new stub cairo context so the Ruby object will still be
     valid */
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);
  new_cr = cairo_create (surface);
  cairo_surface_destroy (surface);

  /* Replace the old cairo context with the stub */
  Data_Get_Struct (self, cairo_t, old_cr);
  DATA_PTR (self) = new_cr;

  /* Destroy the old context so that Clutter will update the texture */
  if (old_cr)
    cairo_destroy (old_cr);

  return self;
}

static VALUE
rbclt_cairo_texture_create_region (VALUE self, VALUE x, VALUE y,
                                   VALUE width, VALUE height)
{
  VALUE argv[4] = { x, y, width, height };

  return rbclt_cairo_texture_create (4, argv, self);
}

static VALUE
rbclt_cairo_texture_set_surface_size (VALUE self, VALUE width, VALUE height)
{
  ClutterCairoTexture *cairo = CLUTTER_CAIRO_TEXTURE (RVAL2GOBJ (self));

  clutter_cairo_texture_set_surface_size (cairo,
                                          NUM2UINT (width),
                                          NUM2UINT (height));

  return self;
}

static VALUE
rbclt_cairo_texture_get_surface_size (VALUE self)
{
  ClutterCairoTexture *cairo = CLUTTER_CAIRO_TEXTURE (RVAL2GOBJ (self));
  guint width, height;

  clutter_cairo_texture_get_surface_size (cairo, &width, &height);

  return rb_ary_new3 (2, UINT2NUM (width), UINT2NUM (height));
}

VALUE
rbclt_cairo_texture_clear (VALUE self)
{
  ClutterCairoTexture *cairo = CLUTTER_CAIRO_TEXTURE (RVAL2GOBJ (self));

  clutter_cairo_texture_clear (cairo);

  return self;
}

void
rbclt_cairo_texture_init ()
{
  VALUE klass;
  VALUE cairo_class, context_class;

  rb_require ("cairo");
  rb_require ("clutter");

  klass = G_DEF_CLASS (CLUTTER_TYPE_CAIRO_TEXTURE,
                       "CairoTexture", rbclt_c_clutter);
  rb_define_method (klass, "initialize", rbclt_cairo_texture_initialize, 2);
  rb_define_method (klass, "create", rbclt_cairo_texture_create, -1);
  rb_define_method (klass, "create_region",
                    rbclt_cairo_texture_create_region, 4);
  rb_define_method (klass, "clear", rbclt_cairo_texture_clear, 0);
  rb_define_method (klass, "set_surface_size",
                    rbclt_cairo_texture_set_surface_size, 2);
  rb_define_method (klass, "surface_size",
                    rbclt_cairo_texture_get_surface_size, 0);
  rb_define_singleton_method (klass, "set_source_color",
                              rbclt_cairo_texture_set_source_color, 2);

  /* Before version 0.7.0, the Ruby bindings of cairo had no 'destroy'
     method. Instead this was called when the object gets reaped by
     the garbage collecter. However in clutter-cairo, the cairo
     context needs to be explicitly destroyed before clutter will
     update the texture. If the 'destroy' method is not available, we
     define a new method which works around the problem by replacing
     the cairo context in the ruby object with a new stub context so
     that the original context can be destroyed without ruining the
     ruby object. Otherwise we just define 'finish' as an alias for
     destroy to maintain backward compatibility */

  cairo_class = rb_const_get (rb_cObject, rb_intern ("Cairo"));
  context_class = rb_const_get (cairo_class, rb_intern ("Context"));

  if (rb_method_boundp (context_class, rb_intern ("destroy"), TRUE))
    rb_define_alias (context_class, "finish", "destroy");
  else
    rb_define_method (context_class, "finish",
                      rbclt_cairo_texture_context_finish, 0);

  id_finish = rb_intern ("finish");
}
