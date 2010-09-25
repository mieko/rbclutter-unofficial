/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2010 Mike A. Owens
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

static VALUE
rb_cogl_clip_push_rectangle(VALUE self, VALUE x0, VALUE y0, VALUE x1, VALUE y1)
{
  cogl_clip_push_rectangle(NUM2DBL(x0), NUM2DBL(y0), NUM2DBL(x1), NUM2DBL(y1));
  return Qnil;
}

static VALUE
rb_cogl_clip_push_from_path(VALUE self)
{
  cogl_clip_push_from_path();
  return Qnil;
}

static VALUE
rb_cogl_clip_push_from_path_preserve(VALUE self)
{
  cogl_clip_push_from_path_preserve();
  return Qnil;
}

static VALUE
rb_cogl_clip_pop(VALUE self)
{
  cogl_clip_pop();
  return Qnil;
}

void
rb_cogl_clip_init()
{
  rb_define_singleton_method(rbclt_c_cogl, "clip_push_rectangle",
                             rb_cogl_clip_push_rectangle, 4);
  rb_define_singleton_method(rbclt_c_cogl, "clip_push_window_rectangle",
                             rb_cogl_clip_push_window_rectangle, 4);
  rb_define_singleton_method(rbclt_c_cogl, "clip_push_from_path",
                             rb_cogl_clip_push_from_path, 0);
  rb_define_singleton_method(rbclt_c_cogl, "clip_push_from_path_preserve",
                             rb_cogl_clip_push_from_path_preserve, 0);
  rb_define_singleton_method(rbclt_c_cogl, "clip_pop", rb_cogl_clip_pop, 0);
}
