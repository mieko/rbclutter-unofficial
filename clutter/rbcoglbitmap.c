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
#include <cogl/cogl.h>

#include "rbclutter.h"
#include "rbcoglhandle.h"
#include "rbcoglbitmap.h"

VALUE rb_c_cogl_bitmap;
static VALUE rb_c_cogl_bitmap_error;

static VALUE
rb_cogl_bitmap_initialize (VALUE self, VALUE filename)
{
  GError *error = NULL;
  CoglHandle bitmap;

  bitmap = cogl_bitmap_new_from_file (StringValuePtr (filename), &error);

  if (bitmap == COGL_INVALID_HANDLE)
    {
      if (error)
        RAISE_GERROR (error);
      else if (bitmap == COGL_INVALID_HANDLE)
        rb_raise (rb_c_cogl_bitmap_error, "Cogl bitmap creation failed");
    }

  rb_cogl_handle_initialize (self, bitmap);

  return Qnil;
}

static VALUE
rb_cogl_bitmap_get_size_from_file (VALUE self, VALUE filename)
{
  int width, height;

  if (!cogl_bitmap_get_size_from_file (StringValuePtr (filename),
                                       &width, &height))
    rb_raise (rb_c_cogl_bitmap_error, "Failed to get size of bitmap file");

  return rb_ary_new3 (2, INT2NUM (width), INT2NUM (height));
}

void
rb_cogl_bitmap_init ()
{
  VALUE klass = rb_cogl_define_handle (cogl_is_bitmap, "Bitmap");

  rb_c_cogl_bitmap = klass;

  rb_c_cogl_bitmap_error = rb_define_class_under (klass, "Error",
                                                  rb_eStandardError);

  rb_define_method (klass, "initialize", rb_cogl_bitmap_initialize, 1);

  rb_define_singleton_method (klass, "get_size_from_file",
                              rb_cogl_bitmap_get_size_from_file, 1);

  G_DEF_SETTERS (klass);
}
