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
#include <mash/mash.h>

#include "rbmash.h"

static VALUE
rbmash_data_initialize (VALUE self)
{
  MashData *data;

  data = mash_data_new ();

  G_INITIALIZE (self, data);

  return Qnil;
}

static VALUE
rbmash_data_load (VALUE self, VALUE filename)
{
  MashData *data = MASH_DATA (RVAL2GOBJ (self));
  GError *error = NULL;

  if (!mash_data_load (data, StringValuePtr (filename), &error))
    RAISE_GERROR (error);

  return self;
}

static VALUE
rbmash_data_render (VALUE self)
{
  MashData *data = MASH_DATA (RVAL2GOBJ (self));

  mash_data_render (data);

  return self;
}

static VALUE
rbmash_data_get_extents (VALUE self)
{
  MashData *data = MASH_DATA (RVAL2GOBJ (self));
  ClutterVertex min_vertex, max_vertex;

  mash_data_get_extents (data, &min_vertex, &max_vertex);

  return rb_ary_new3 (2,
                      BOXED2RVAL (&min_vertex, CLUTTER_TYPE_VERTEX),
                      BOXED2RVAL (&max_vertex, CLUTTER_TYPE_VERTEX));
}

void
rbmash_data_init ()
{
  VALUE klass = G_DEF_CLASS (MASH_TYPE_DATA, "Data",
                             rbmash_c_mash);

  rb_define_method (klass, "initialize", rbmash_data_initialize, 0);
  rb_define_method (klass, "load", rbmash_data_load, 1);
  rb_define_method (klass, "render", rbmash_data_render, 0);
  rb_define_method (klass, "extents", rbmash_data_get_extents, 0);

  G_DEF_ERROR (MASH_DATA_ERROR, "Error", klass,
               rb_eRuntimeError, MASH_TYPE_DATA_ERROR);
}
