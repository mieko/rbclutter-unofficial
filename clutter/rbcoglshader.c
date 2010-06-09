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
#include "rbcoglshader.h"
#include "rbcoglhandle.h"

VALUE rb_c_cogl_shader;

static VALUE
rb_cogl_shader_initialize (VALUE self, VALUE shader_type)
{
  CoglHandle shader;

  shader = cogl_create_shader (NUM2UINT (shader_type));

  rb_cogl_handle_initialize (self, shader);

  return Qnil;
}

static VALUE
rb_cogl_shader_source (VALUE self, VALUE source)
{
  cogl_shader_source (rb_cogl_handle_get_handle (self),
                      StringValuePtr (source));

  return self;
}

static VALUE
rb_cogl_shader_compile (VALUE self)
{
  cogl_shader_compile (rb_cogl_handle_get_handle (self));

  return self;
}

static VALUE
rb_cogl_shader_get_info_log (VALUE self)
{
  CoglHandle shader = rb_cogl_handle_get_handle (self);
  gchar *buf;
  VALUE ret;

  buf = cogl_shader_get_info_log (shader);
  ret = rb_str_new2 (buf);
  g_free (buf);

  return ret;
}

void
rb_cogl_shader_init ()
{
  VALUE klass = rb_cogl_define_handle (cogl_is_shader, "Shader");
  rb_c_cogl_shader = klass;

  rb_define_method (klass, "initialize", rb_cogl_shader_initialize, 1);
  rb_define_method (klass, "source", rb_cogl_shader_source, 1);
  rb_define_method (klass, "compile", rb_cogl_shader_compile, 0);
  rb_define_method (klass, "info_log", rb_cogl_shader_get_info_log, 0);
}
