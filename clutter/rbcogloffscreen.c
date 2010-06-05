/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2008  Neil Roberts
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
#include "rbcogltexture.h"

static VALUE rb_c_cogl_offscreen;
static VALUE rb_c_cogl_offscreen_error;

static void
rb_cogl_offscreen_free (void *ptr)
{
  if (ptr)
    cogl_handle_unref (ptr);
}

static VALUE
rb_cogl_offscreen_allocate (VALUE klass)
{
  return Data_Wrap_Struct (klass, NULL, rb_cogl_offscreen_free, NULL);
}

static VALUE
rb_cogl_offscreen_initialize (VALUE self, VALUE tex)
{
  CoglHandle offscreen;
  CoglHandle tex_handle = rb_cogl_texture_get_handle (tex);

  offscreen = cogl_offscreen_new_to_texture (tex_handle);

  if (offscreen == COGL_INVALID_HANDLE)
    rb_raise (rb_c_cogl_offscreen_error, "failed to create offscreen object");

  DATA_PTR (self) = offscreen;

  return Qnil;
}

void
rb_cogl_offscreen_init ()
{
  VALUE klass = rb_define_class_under (rbclt_c_cogl, "Offscreen",
                                       rb_cObject);
  rb_c_cogl_offscreen = klass;

  rb_c_cogl_offscreen_error = rb_define_class_under (klass, "Error",
                                                     rb_eStandardError);

  rb_define_alloc_func (klass, rb_cogl_offscreen_allocate);

  rb_define_method (klass, "initialize", rb_cogl_offscreen_initialize, -1);
}
