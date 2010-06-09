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

#include <cogl/cogl.h>

#include "rbclutter.h"
#include "rbcoglmatrix.h"

static VALUE rb_cogl_c_matrix;

static void
rb_cogl_matrix_free (void *data)
{
  g_slice_free (CoglMatrix, data);
}

static VALUE
rb_cogl_matrix_alloc_with_class (VALUE klass)
{
  CoglMatrix *matrix = g_slice_new (CoglMatrix);

  return Data_Wrap_Struct (klass, 0, rb_cogl_matrix_free, matrix);
}

VALUE
rb_cogl_matrix_alloc (void)
{
  return rb_cogl_matrix_alloc_with_class (rb_cogl_c_matrix);
}

CoglMatrix *
rb_cogl_matrix_get_pointer (VALUE self)
{
  CoglMatrix *matrix;

  Data_Get_Struct (self, CoglMatrix, matrix);

  return matrix;
}

static void
rb_cogl_matrix_initialize_from_array (CoglMatrix *matrix, VALUE *argv)
{
  float float_values[16];
  int i;

  for (i = 0; i < 16; i++)
    float_values[i] = NUM2DBL (argv[i]);

  cogl_matrix_init_from_array (matrix, float_values);
}

static VALUE
rb_cogl_matrix_initialize (int argc, VALUE *argv, VALUE self)
{
  CoglMatrix *matrix;

  Data_Get_Struct (self, CoglMatrix, matrix);

  /* We'll accept either zero arguments to create the identity matrix,
     1 argument with an array of values or 16 arguments with all the
     values */

  if (argc == 0)
    cogl_matrix_init_identity (matrix);
  else if (argc == 1)
    {
      if (TYPE (argv[0]) != T_ARRAY || RARRAY_LEN (argv[0]) != 16)
        rb_raise (rb_eArgError, "array with 16 elements expected");
      else
        rb_cogl_matrix_initialize_from_array (matrix, RARRAY_PTR (argv[0]));
    }
  else if (argc == 16)
    rb_cogl_matrix_initialize_from_array (matrix, argv);
  else
    rb_raise (rb_eArgError, "wrong number of arguments (0, 1 or 16 expected)");

  return Qnil;
}

gboolean
rb_cogl_is_kind_of_matrix (VALUE self)
{
  return TYPE (self) == T_DATA && RDATA (self)->dfree == rb_cogl_matrix_free;
}

void
rb_cogl_assert_is_kind_of_matrix (VALUE arg)
{
  if (!rb_cogl_is_kind_of_matrix (arg))
    rb_raise (rb_eTypeError, "wrong argument type");
}

static VALUE
rb_cogl_matrix_initialize_copy (VALUE self, VALUE orig)
{
  CoglMatrix *matrix_self, *matrix_orig;

  rb_cogl_assert_is_kind_of_matrix (orig);

  Data_Get_Struct (self, CoglMatrix, matrix_self);
  Data_Get_Struct (orig, CoglMatrix, matrix_orig);

  memcpy (matrix_self, matrix_orig, sizeof (CoglMatrix));

  return Qnil;
}

static VALUE
rb_cogl_matrix_to_a (VALUE self)
{
  CoglMatrix *matrix;
  float *matrix_array;
  VALUE array_values[16];
  int i;

  Data_Get_Struct (self, CoglMatrix, matrix);

  matrix_array = &matrix->xx;

  for (i = 0; i < 16; i++)
    array_values[i] = rb_float_new (matrix_array[i]);

  return rb_ary_new4 (16, array_values);
}

static VALUE
rb_cogl_matrix_aref (VALUE self, VALUE index_value)
{
  CoglMatrix *matrix;
  int index_num = NUM2INT (index_value);

  Data_Get_Struct (self, CoglMatrix, matrix);

  if (index_num < 0 || index_num >= 16)
    rb_raise (rb_eArgError, "index out of range");

  return rb_float_new ((&matrix->xx)[index_num]);
}

static VALUE
rb_cogl_matrix_multiply (VALUE a_arg, VALUE b_arg)
{
  VALUE result_value;
  CoglMatrix *a, *b, *result;

  rb_cogl_assert_is_kind_of_matrix (b_arg);

  result_value = rb_cogl_matrix_alloc ();
  Data_Get_Struct (result_value, CoglMatrix, result);
  Data_Get_Struct (a_arg, CoglMatrix, a);
  Data_Get_Struct (b_arg, CoglMatrix, b);

  cogl_matrix_multiply (result, a, b);

  return result_value;
}

static VALUE
rb_cogl_matrix_rotate (VALUE self, VALUE angle, VALUE x, VALUE y, VALUE z)
{
  CoglMatrix *matrix;

  Data_Get_Struct (self, CoglMatrix, matrix);

  cogl_matrix_rotate (matrix, NUM2DBL (angle),
                      NUM2DBL (x), NUM2DBL (y), NUM2DBL (z));

  return self;
}

static VALUE
rb_cogl_matrix_translate (VALUE self, VALUE x, VALUE y, VALUE z)
{
  CoglMatrix *matrix;

  Data_Get_Struct (self, CoglMatrix, matrix);

  cogl_matrix_translate (matrix, NUM2DBL (x), NUM2DBL (y), NUM2DBL (z));

  return self;
}

static VALUE
rb_cogl_matrix_scale (VALUE self, VALUE x, VALUE y, VALUE z)
{
  CoglMatrix *matrix;

  Data_Get_Struct (self, CoglMatrix, matrix);

  cogl_matrix_scale (matrix, NUM2DBL (x), NUM2DBL (y), NUM2DBL (z));

  return self;
}

static VALUE
rb_cogl_matrix_frustum (VALUE self,
                        VALUE left, VALUE right,
                        VALUE bottom, VALUE top,
                        VALUE z_near, VALUE z_far)
{
  CoglMatrix *matrix;

  Data_Get_Struct (self, CoglMatrix, matrix);

  cogl_matrix_frustum (matrix,
                       NUM2DBL (left), NUM2DBL (right),
                       NUM2DBL (bottom), NUM2DBL (top),
                       NUM2DBL (z_near), NUM2DBL (z_far));

  return self;
}

static VALUE
rb_cogl_matrix_perspective (VALUE self, VALUE fov_y, VALUE aspect,
                            VALUE z_near, VALUE z_far)
{
  CoglMatrix *matrix;

  Data_Get_Struct (self, CoglMatrix, matrix);

  cogl_matrix_perspective (matrix, NUM2DBL (fov_y), NUM2DBL (aspect),
                           NUM2DBL (z_near), NUM2DBL (z_far));

  return self;
}

static VALUE
rb_cogl_matrix_ortho (VALUE self,
                      VALUE left, VALUE right,
                      VALUE bottom, VALUE top,
                      VALUE z_near, VALUE z_far)
{
  CoglMatrix *matrix;

  Data_Get_Struct (self, CoglMatrix, matrix);

  cogl_matrix_ortho (matrix,
                     NUM2DBL (left), NUM2DBL (right),
                     NUM2DBL (bottom), NUM2DBL (top),
                     NUM2DBL (z_near), NUM2DBL (z_far));

  return self;
}

static VALUE
rb_cogl_matrix_get_inverse (VALUE self)
{
  VALUE result_value;
  CoglMatrix *matrix, *result;

  Data_Get_Struct (self, CoglMatrix, matrix);
  result_value = rb_cogl_matrix_alloc ();
  Data_Get_Struct (result_value, CoglMatrix, result);
  cogl_matrix_get_inverse (matrix, result);

  return result_value;
}

static VALUE
rb_cogl_matrix_transform_point (VALUE self, VALUE x, VALUE y, VALUE z, VALUE w)
{
  CoglMatrix *matrix;
  float x_out = NUM2DBL (x);
  float y_out = NUM2DBL (y);
  float z_out = NUM2DBL (z);
  float w_out = NUM2DBL (w);

  Data_Get_Struct (self, CoglMatrix, matrix);

  cogl_matrix_transform_point (matrix, &x_out, &y_out, &z_out, &w_out);

  return rb_ary_new3 (4,
                      rb_float_new (x_out),
                      rb_float_new (y_out),
                      rb_float_new (z_out),
                      rb_float_new (w_out));
}

#define RB_COGL_DEFINE_ACCESSOR_FUNC(member)          \
  static VALUE                                  \
  rb_cogl_matrix_get_ ## member (VALUE self)     \
  {                                             \
   CoglMatrix *matrix;                          \
   Data_Get_Struct (self, CoglMatrix, matrix);  \
   return rb_float_new (matrix->member);        \
  }

RB_COGL_DEFINE_ACCESSOR_FUNC (xx)
RB_COGL_DEFINE_ACCESSOR_FUNC (yx)
RB_COGL_DEFINE_ACCESSOR_FUNC (zx)
RB_COGL_DEFINE_ACCESSOR_FUNC (wx)
RB_COGL_DEFINE_ACCESSOR_FUNC (xy)
RB_COGL_DEFINE_ACCESSOR_FUNC (yy)
RB_COGL_DEFINE_ACCESSOR_FUNC (zy)
RB_COGL_DEFINE_ACCESSOR_FUNC (wy)
RB_COGL_DEFINE_ACCESSOR_FUNC (xz)
RB_COGL_DEFINE_ACCESSOR_FUNC (yz)
RB_COGL_DEFINE_ACCESSOR_FUNC (zz)
RB_COGL_DEFINE_ACCESSOR_FUNC (wz)
RB_COGL_DEFINE_ACCESSOR_FUNC (xw)
RB_COGL_DEFINE_ACCESSOR_FUNC (yw)
RB_COGL_DEFINE_ACCESSOR_FUNC (zw)
RB_COGL_DEFINE_ACCESSOR_FUNC (ww)

#define RB_COGL_DEFINE_ACCESSOR(member)                 \
  rb_define_method (klass, G_STRINGIFY (member),        \
                    rb_cogl_matrix_get_ ## member, 0)

void
rb_cogl_matrix_init ()
{
  VALUE klass;

  klass = rb_define_class_under (rbclt_c_cogl, "Matrix", rb_cObject);
  rb_cogl_c_matrix = klass;

  rb_define_alloc_func (klass, rb_cogl_matrix_alloc_with_class);

  rb_define_method (klass, "initialize", rb_cogl_matrix_initialize, -1);
  rb_define_method (klass, "initialize_copy",
                    rb_cogl_matrix_initialize_copy, 1);
  rb_define_method (klass, "to_a", rb_cogl_matrix_to_a, 0);
  rb_define_method (klass, "[]", rb_cogl_matrix_aref, 1);

  rb_define_method (klass, "multiply", rb_cogl_matrix_multiply, 1);
  rb_define_alias (klass, "*", "multiply");

  rb_define_method (klass, "rotate!", rb_cogl_matrix_rotate, 4);
  rb_define_method (klass, "translate!", rb_cogl_matrix_translate, 3);
  rb_define_method (klass, "scale!", rb_cogl_matrix_scale, 3);
  rb_define_method (klass, "frustum!", rb_cogl_matrix_frustum, 6);
  rb_define_method (klass, "perspective!", rb_cogl_matrix_perspective, 4);
  rb_define_method (klass, "ortho!", rb_cogl_matrix_ortho, 6);

  rb_define_method (klass, "inverse", rb_cogl_matrix_get_inverse, 0);
  rb_define_method (klass, "transform_point",
                    rb_cogl_matrix_transform_point, 4);

  RB_COGL_DEFINE_ACCESSOR (xx);
  RB_COGL_DEFINE_ACCESSOR (yx);
  RB_COGL_DEFINE_ACCESSOR (zx);
  RB_COGL_DEFINE_ACCESSOR (wx);
  RB_COGL_DEFINE_ACCESSOR (xy);
  RB_COGL_DEFINE_ACCESSOR (yy);
  RB_COGL_DEFINE_ACCESSOR (zy);
  RB_COGL_DEFINE_ACCESSOR (wy);
  RB_COGL_DEFINE_ACCESSOR (xz);
  RB_COGL_DEFINE_ACCESSOR (yz);
  RB_COGL_DEFINE_ACCESSOR (zz);
  RB_COGL_DEFINE_ACCESSOR (wz);
  RB_COGL_DEFINE_ACCESSOR (xw);
  RB_COGL_DEFINE_ACCESSOR (yw);
  RB_COGL_DEFINE_ACCESSOR (zw);
  RB_COGL_DEFINE_ACCESSOR (ww);
}
