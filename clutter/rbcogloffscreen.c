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

static VALUE rb_c_cogl_offscreen_error;

static VALUE
rb_cogl_offscreen_initialize (VALUE self, VALUE tex)
{
  CoglHandle offscreen;
  CoglHandle tex_handle = rb_cogl_handle_get_handle (tex);

  if (!cogl_is_texture (tex_handle))
    rb_raise (rb_eArgError, "CoglTexture handle expected");

  offscreen = cogl_offscreen_new_to_texture (tex_handle);

  if (offscreen == COGL_INVALID_HANDLE)
    rb_raise (rb_c_cogl_offscreen_error, "failed to create offscreen object");

  rb_cogl_handle_initialize (self, offscreen);

  return Qnil;
}

static VALUE
rb_cogl_pop_framebuffer (VALUE self)
{
  cogl_pop_framebuffer ();
  return Qnil;
}

static VALUE
rb_cogl_push_framebuffer (VALUE self, VALUE buffer)
{
  cogl_push_framebuffer (rb_cogl_handle_get_handle(buffer));
  return Qnil;
}

static VALUE
rb_cogl_set_framebuffer (VALUE self, VALUE buffer)
{
  cogl_set_framebuffer (rb_cogl_handle_get_handle(buffer));
  return Qnil;
}

void
rb_cogl_offscreen_init ()
{
  VALUE klass = rb_cogl_define_handle (cogl_is_offscreen, "Offscreen");

  rb_c_cogl_offscreen_error = rb_define_class_under (klass, "Error",
                                                     rb_eStandardError);

  rb_define_method (klass, "initialize", rb_cogl_offscreen_initialize, 1);

  rb_define_singleton_method(rbclt_c_cogl, "pop_framebuffer",
                             rb_cogl_pop_framebuffer, 0);
  rb_define_singleton_method(rbclt_c_cogl, "push_framebuffer",
                             rb_cogl_push_framebuffer, 1);
  rb_define_singleton_method(rbclt_c_cogl, "set_framebuffer",
                             rb_cogl_set_framebuffer, 1);
}
