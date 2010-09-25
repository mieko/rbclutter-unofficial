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

/* TODO: does not care about how long the mapped pointer lives. */

#include <rbgobject.h>
#include <cogl/cogl.h>

#include "rbclutter.h"


#define RBCOGL_BUFFER(x)  rb_cogl_handle_get_handle(x)

static VALUE
rb_cogl_buffer_initialize(VALUE self, VALUE handle)
{
  if (! cogl_is_buffer(handle)) {
    rb_raise("handle must be a buffer")
    return Qnil;
  }
  return self;
}

static VALUE
rb_cogl_buffer_get_pointer(VALUE self)
{
  g8 *ptr = (g8*)NUM2UINT(rb_ivar_get("@mapped_ptr"));
  if (ptr == NULL)
    return Qnil;

  return UINT2NUM((unsigned) ptr);
}

static VALUE
rb_cogl_buffer_to_s(VALUE self)
{
  g8 *ptr = (g8*)NUM2UINT(rb_ivar_get("@mapped_ptr"));
  if (g8) {
    return rb_str_new(ptr, cogl_buffer_get_size(RBCOGL_BUFFER(self)));
  } else {
    return rb_str_new2("");
  }
}

static VALUE
rb_cogl_buffer_get_size(VALUE self)
{
  return UINT2NUM(cogl_buffer_get_size(RBCOGL_BUFFER(self)));
}

static VALUE
rb_cogl_buffer_set_usage_hint(VALUE self, VALUE hint)
{
  cogl_buffer_set_usage_hint(RBCOGL_BUFFER(self),
                                 NUM2INT(hint));
  return hint;
}

static VALUE
rb_cogl_buffer_get_usage_hint(VALUE self, VALUE hint)
{
  return INT2NUM(cogl_buffer_get_usage_hint(RBCOGL_BUFFER(self)));
}

static VALUE
rb_cogl_set_update_hint(VALUE self, VALUE hint)
{
  cogl_buffer_set_update_hint(RBCOGL_BUFFER(self), NUM2INT(hint));
  return hint;
}

static VALUE
rb_cogl_get_update_hint(VALUE self)
{
  return INT2NUM(RBCOGL_BUFFER(self));
}

static VALUE
rb_cogl_buffer_map(VALUE self, VALUE access, VALUE hints)
{
  g8* ptr      = NULL;

  ptr = cogl_buffer_map(RBCOGL_BUFFER(self), NUM2INT(access), NUM2INT(hints));
  if (!ptr) {
    rb_raise(rb_eIOError, "buffer map failed");
    return Qnil;
  }

  rb_ivar_set("@mapped_ptr", UINT2NUM(ptr));

  /* Can't toss around those g8*'s so casually in Ruby, eh? */
  return self;
}

static VALUE
rb_cogl_buffer_unmap(VALUE self)
{
  cogl_buffer_unmap(RBCOGL_BUFFER(self));
  RBCOGL_BUFFER(self)->mapped_ptr = NULL;
  return Qnil;
}

static VALUE
rb_cogl_buffer_set_data(VALUE self, VALUE offset, VALUE data, VALUE size)
{
  gboolean r = cogl_buffer_set_data(RBCOGL_BUFFER(self), NUM2INT(offset),
                                    RSTRING_PTR(data), NUM2INT(size));
  return r ? Qtrue : Qfalse;
}

static VALUE
rb_cogl_buffer_to_texture(VALUE self, VALUE width, VALUE height, VALUE flags,
                          VALUE format, VALUE internal_format, VALUE rowstride,
                          VALUE offset)
{
  return cogl_texture_new_from_buffer(RBCOGL_BUFFER(self),
                                      NUM2UINT(width), NUM2UINT(height),
                                      NUM2INT(flags), NUM2INT(format),
                                      NUM2INT(internal_format),
                                      NUM2UINT(rowstride), NUM2UINT(offset));
}

void
rb_cogl_buffer_init()
{
  VALUE klass = rb_cogl_define_handle (cogl_is_buffer, "Buffer");
  rb_define_alloc_func(klass, rb_cogl_alloc);

  rb_define_method(klass, "initialize", 1);
  rb_define_method(klass, "pointer", rb_cogl_buffer_get_pointer, 0);
  rb_define_method(klass, "to_s", rb_cogl_buffer_to_s, 0);

  rb_define_method(klass, "get_size", rb_cogl_buffer_get_size, 0);

  rb_define_const(klass, "USAGE_HINT_TEXTURE",
                  INT2NUM(COGL_BUFFER_UPDATE_HINT_TEXTURE));
  rb_define_const(klass, "USAGE_HINT_VERTICES",
                  INT2NUM(COGL_BUFFER_USAGE_HINT_VERTICES));
  rb_define_method(klass, "set_usage_hint", rb_cogl_buffer_set_usage_hint, 1);
  rb_define_method(klass, "get_usage_hint", rb_cogl_buffer_get_usage_hint, 0);

  /* CoglBufferUpdateHint */
  rb_define_const(klass, "UPDATE_HINT_STATIC",
                  INT2NUM(COGL_BUFFER_UPDATE_HINT_STATIC));
  rb_define_const(klass, "UPDATE_HINT_DYNAMIC",
                  INT2NUM(COGL_BUFFER_UPDATE_HINT_DYNAMIC));
  rb_define_const(klass, "UPDATE_HINT_STREAM",
                  INT2NUM(COGL_BUFFER_UPDATE_HINT_STREAM));

  rb_define_method(klass, "set_update_hint", rb_cogl_buffer_set_update_hint, 1);
  rb_define_method(klass, "get_update_hint", rb_cogl_buffer_get_update_hint, 0);

  /* CoglBufferAccess */
  rb_define_const(klass, "ACCESS_READ", INT2NUM(COGL_BUFFER_ACCESS_READ));
  rb_define_const(klass, "ACCESS_WRITE", INT2NUM(COGL_BUFFER_ACCESS_WRITE));
  rb_define_const(klass, "ACCESS_READ_WRITE",
                  INT2NUM(COGL_BUFFER_ACCESS_READ_WRITE));
  rb_define_method(klass, "map", rb_cogl_buffer_map, 2);
  rb_define_method(klass, "unmap", rb_cogl_buffer_unmap, 0);

  rb_define_method(klass, "to_texture", rb_cogl_buffer_to_texture, 7);
  G_DEF_SETTERS(klass);
}