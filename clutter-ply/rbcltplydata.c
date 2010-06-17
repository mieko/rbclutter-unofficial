/* Ruby bindings for the Clutter 'interactive canvas' library.
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
#include <clutter-ply/clutter-ply.h>

#include "rbclutterply.h"

static VALUE
rbcltply_data_initialize (VALUE self)
{
  ClutterPlyData *data;

  data = clutter_ply_data_new ();

  G_INITIALIZE (self, data);

  return Qnil;
}

static VALUE
rbcltply_data_load (VALUE self, VALUE filename)
{
  ClutterPlyData *data = CLUTTER_PLY_DATA (RVAL2GOBJ (self));
  GError *error = NULL;

  if (!clutter_ply_data_load (data, StringValuePtr (filename), &error))
    RAISE_GERROR (error);

  return self;
}

static VALUE
rbcltply_data_render (VALUE self)
{
  ClutterPlyData *data = CLUTTER_PLY_DATA (RVAL2GOBJ (self));

  clutter_ply_data_render (data);

  return self;
}

static VALUE
rbcltply_data_get_extents (VALUE self)
{
  ClutterPlyData *data = CLUTTER_PLY_DATA (RVAL2GOBJ (self));
  ClutterVertex min_vertex, max_vertex;

  clutter_ply_data_get_extents (data, &min_vertex, &max_vertex);

  return rb_ary_new3 (2,
                      BOXED2RVAL (&min_vertex, CLUTTER_TYPE_VERTEX),
                      BOXED2RVAL (&max_vertex, CLUTTER_TYPE_VERTEX));
}

void
rbcltply_data_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_PLY_TYPE_DATA, "Data",
                             rbcltply_c_clutter_ply);

  rb_define_method (klass, "initialize", rbcltply_data_initialize, 0);
  rb_define_method (klass, "load", rbcltply_data_load, 1);
  rb_define_method (klass, "render", rbcltply_data_render, 0);
  rb_define_method (klass, "extents", rbcltply_data_get_extents, 0);

  G_DEF_ERROR (CLUTTER_PLY_DATA_ERROR, "Error", klass,
               rb_eRuntimeError, CLUTTER_PLY_TYPE_DATA_ERROR);
}
