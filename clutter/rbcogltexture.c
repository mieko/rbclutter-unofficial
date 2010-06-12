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
#include <clutter/clutter.h>

#include "rbclutter.h"
#include "rbcoglhandle.h"
#include "rbcogltexture.h"
#include "rbcoglbitmap.h"

VALUE rb_c_cogl_texture;
static VALUE rb_c_cogl_texture_error;

static int
rb_cogl_texture_get_format_bpp (CoglPixelFormat format)
{
  int bpp = 0;

  format &= COGL_UNORDERED_MASK;

  if (format == COGL_PIXEL_FORMAT_A_8
      || format == COGL_PIXEL_FORMAT_G_8)
    bpp = 1;
  else if (format == COGL_PIXEL_FORMAT_RGB_565
           || format == COGL_PIXEL_FORMAT_RGBA_4444
           || format == COGL_PIXEL_FORMAT_RGBA_5551)
    bpp = 2;
  else if (format == COGL_PIXEL_FORMAT_24)
    bpp = 3;
  else if (format == COGL_PIXEL_FORMAT_32)
    bpp = 4;
  else
    rb_raise (rb_c_cogl_texture_error, "no rowstride given "
              "and unknown pixel format used");

  return bpp;
}

static VALUE
rb_cogl_texture_initialize (int argc, VALUE *argv_in, VALUE self)
{
  GError *error = NULL;
  CoglHandle tex = COGL_INVALID_HANDLE;
  VALUE argv[7];
  int i;

  if (argc > G_N_ELEMENTS (argv))
    rb_raise (rb_eArgError, "Too many arguments given");

  for (i = 0; i < argc; i++)
    argv[i] = argv_in[i];
  for (; i < G_N_ELEMENTS (argv); i++)
    argv[i] = Qnil;

  if (argc >= 2 && argc <= 4
      && rb_obj_is_kind_of (argv[0], rb_cNumeric)
      && rb_obj_is_kind_of (argv[1], rb_cNumeric))
    tex = cogl_texture_new_with_size (NUM2UINT (argv[0]),
                                      NUM2UINT (argv[1]),
                                      NIL_P (argv[2])
                                      ? COGL_TEXTURE_NONE
                                      : RVAL2GFLAGS (argv[1],
                                                     COGL_TYPE_TEXTURE_FLAGS),
                                      NIL_P (argv[3])
                                      ? COGL_PIXEL_FORMAT_ANY
                                      : RVAL2GENUM (argv[2],
                                                    COGL_TYPE_PIXEL_FORMAT));
  else if (rb_obj_is_kind_of (argv[0], rb_c_cogl_bitmap))
    tex = cogl_texture_new_from_bitmap (rb_cogl_handle_get_handle (argv[0]),
                                        NIL_P (argv[1])
                                        ? COGL_TEXTURE_NONE
                                        : RVAL2GFLAGS (argv[1],
                                                       COGL_TYPE_TEXTURE_FLAGS),
                                        NIL_P (argv[2])
                                        ? COGL_PIXEL_FORMAT_ANY
                                        : RVAL2GENUM (argv[2],
                                                      COGL_TYPE_PIXEL_FORMAT));
  else if (argc == 5 && rb_obj_is_kind_of (argv[0], rb_c_cogl_texture))
    {
      CoglHandle handle = rb_cogl_handle_get_handle (argv[0]);
      tex = cogl_texture_new_from_sub_texture (handle,
                                               NUM2INT (argv[1]),
                                               NUM2INT (argv[2]),
                                               NUM2INT (argv[3]),
                                               NUM2INT (argv[4]));
    }
  else if (argc >= 1 && argc <= 3)
    tex = cogl_texture_new_from_file (StringValuePtr (argv[0]),
                                      NIL_P (argv[1])
                                      ? COGL_TEXTURE_NONE
                                      : RVAL2GFLAGS (argv[1],
                                                     COGL_TYPE_TEXTURE_FLAGS),
                                      NIL_P (argv[2])
                                      ? COGL_PIXEL_FORMAT_ANY
                                      : RVAL2GENUM (argv[2],
                                                    COGL_TYPE_PIXEL_FORMAT),
                                      &error);
  else if (argc == 7 && rb_obj_is_kind_of (argv[6], rb_cString))
    {
      guint width = NUM2UINT (argv[0]);
      guint height = NUM2UINT (argv[1]);
      CoglTextureFlags flags
        = (NIL_P (argv[2]) ? COGL_TEXTURE_NONE
           : RVAL2GFLAGS (argv[2], COGL_TYPE_TEXTURE_FLAGS));
      CoglPixelFormat format = RVAL2GENUM (argv[3], COGL_TYPE_PIXEL_FORMAT);
      CoglPixelFormat internal_format
        = (NIL_P (argv[4]) ? COGL_PIXEL_FORMAT_ANY
           : RVAL2GENUM (argv[4], COGL_TYPE_PIXEL_FORMAT));
      unsigned int rowstride;
      const char *data = StringValuePtr (argv[6]);

      /* If there is no rowstride then try to guess what it will be
         from the format */
      if (NIL_P (argv[5]) || (rowstride = NUM2UINT (argv[5])) == 0)
        rowstride = width * rb_cogl_texture_get_format_bpp (format);

      /* Make sure the string is long enough */
      if (RSTRING_LEN (argv[6]) < height * rowstride)
        rb_raise (rb_eArgError, "data string too short");

      tex = cogl_texture_new_from_data (width, height,
                                        flags, format,
                                        internal_format,
                                        rowstride,
                                        (const guchar *) data);
    }
  else if (argc == 7) /* assume new from foreign if last arg is not a string */
    {
      CoglPixelFormat format = RVAL2GENUM (argv[6], COGL_TYPE_PIXEL_FORMAT);
      tex = cogl_texture_new_from_foreign (NUM2UINT (argv[0]),
                                           NUM2INT (argv[1]),
                                           NUM2UINT (argv[2]),
                                           NUM2UINT (argv[3]),
                                           NUM2UINT (argv[4]),
                                           NUM2UINT (argv[5]),
                                           format);
    }
  else
    rb_raise (rb_eArgError, "wrong number of arguments");

  if (error)
    RAISE_GERROR (error);
  else if (tex == COGL_INVALID_HANDLE)
    rb_raise (rb_c_cogl_texture_error, "Cogl texture creation failed");

  rb_cogl_handle_initialize (self, tex);

  return Qnil;
}

static VALUE
rb_cogl_texture_get_width (VALUE self)
{
  CoglHandle tex = rb_cogl_handle_get_handle (self);

  return UINT2NUM (cogl_texture_get_width (tex));
}

static VALUE
rb_cogl_texture_get_height (VALUE self)
{
  CoglHandle tex = rb_cogl_handle_get_handle (self);

  return UINT2NUM (cogl_texture_get_height (tex));
}

static VALUE
rb_cogl_texture_get_format (VALUE self)
{
  CoglHandle tex = rb_cogl_handle_get_handle (self);

  return GENUM2RVAL (cogl_texture_get_format (tex),
                     COGL_TYPE_PIXEL_FORMAT);
}

static VALUE
rb_cogl_texture_get_rowstride (VALUE self)
{
  CoglHandle tex = rb_cogl_handle_get_handle (self);

  return UINT2NUM (cogl_texture_get_rowstride (tex));
}

static VALUE
rb_cogl_texture_get_max_waste (VALUE self)
{
  CoglHandle tex = rb_cogl_handle_get_handle (self);

  return INT2NUM (cogl_texture_get_max_waste (tex));
}

static VALUE
rb_cogl_texture_is_sliced (VALUE self)
{
  CoglHandle tex = rb_cogl_handle_get_handle (self);

  return cogl_texture_is_sliced (tex) ? Qtrue : Qfalse;
}

static VALUE
rb_cogl_texture_get_data (int argc, VALUE *argv, VALUE self)
{
  CoglHandle tex = rb_cogl_handle_get_handle (self);
  VALUE format_arg, rowstride_arg;
  CoglPixelFormat format;
  guint rowstride;
  VALUE data;

  rb_scan_args (argc, argv, "02", &format_arg, &rowstride_arg);

  format = NIL_P (format_arg)
    ? cogl_texture_get_format (tex) : NUM2UINT (format_arg);
  rowstride = (NIL_P (rowstride_arg) || NUM2UINT (rowstride_arg) == 0)
    ? cogl_texture_get_rowstride (tex) : NUM2UINT (rowstride_arg);

  data = rb_str_new (NULL, rowstride * cogl_texture_get_height (tex));
  cogl_texture_get_data (tex, format, rowstride,
                         (guchar *) RSTRING_PTR (data));

  return data;
}

static VALUE
rb_cogl_texture_set_region (VALUE self, VALUE src_x, VALUE src_y,
                            VALUE dst_x, VALUE dst_y,
                            VALUE dst_width, VALUE dst_height,
                            VALUE width_arg, VALUE height_arg,
                            VALUE format, VALUE rowstride_arg,
                            VALUE data_arg)
{
  guint width = NUM2UINT (width_arg);
  guint height = NUM2UINT (height_arg);
  guint rowstride;
  const char *data = StringValuePtr (data_arg);

  /* If there is no rowstride then try to guess what it will be from
     the format */
  if (NIL_P (rowstride_arg) || (rowstride = NUM2UINT (rowstride_arg)) == 0)
    rowstride = width * rb_cogl_texture_get_format_bpp (NUM2UINT (format));

  /* Make sure the string is long enough */
  if (RSTRING_LEN (data_arg) < height * rowstride)
    rb_raise (rb_eArgError, "data string too short");

  if (!cogl_texture_set_region (rb_cogl_handle_get_handle (self),
                                NUM2INT (src_x), NUM2INT (src_y),
                                NUM2INT (dst_x), NUM2INT (dst_y),
                                NUM2UINT (dst_width), NUM2UINT (dst_height),
                                width, height,
                                NUM2UINT (format),
                                rowstride,
                                (const guchar *) data))
    rb_raise (rb_c_cogl_texture_error, "texture set region failed");

  return self;
}

static VALUE
rb_cogl_texture_get_gl_texture (VALUE self)
{
  CoglHandle tex = rb_cogl_handle_get_handle (self);
  GLuint gl_handle;
  GLenum gl_target;

  cogl_texture_get_gl_texture (tex, &gl_handle, &gl_target);

  return rb_ary_new3 (2, UINT2NUM (gl_handle), INT2NUM (gl_target));
}

void
rb_cogl_texture_init ()
{
  VALUE klass = rb_cogl_define_handle (cogl_is_texture, "Texture");

  rb_c_cogl_texture = klass;

  rb_c_cogl_texture_error = rb_define_class_under (klass, "Error",
                                                   rb_eStandardError);

  G_DEF_CLASS (COGL_TYPE_TEXTURE_FLAGS, "Flags", klass);
  G_DEF_CONSTANTS (klass, COGL_TYPE_TEXTURE_FLAGS, "COGL_TEXTURE_");

  rb_define_method (klass, "initialize", rb_cogl_texture_initialize, -1);
  rb_define_method (klass, "width", rb_cogl_texture_get_width, 0);
  rb_define_method (klass, "height", rb_cogl_texture_get_height, 0);
  rb_define_method (klass, "format", rb_cogl_texture_get_format, 0);
  rb_define_method (klass, "rowstride", rb_cogl_texture_get_rowstride, 0);
  rb_define_method (klass, "max_waste", rb_cogl_texture_get_max_waste, 0);
  rb_define_method (klass, "sliced?", rb_cogl_texture_is_sliced, 0);
  rb_define_method (klass, "get_data", rb_cogl_texture_get_data, -1);
  rb_define_alias (klass, "data", "get_data");
  rb_define_method (klass, "set_region",
                    rb_cogl_texture_set_region, 11);
  rb_define_method (klass, "gl_texture", rb_cogl_texture_get_gl_texture, 0);

  G_DEF_SETTERS (klass);
}
