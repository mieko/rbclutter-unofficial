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

#define COGL_ENABLE_EXPERIMENTAL_API

#include <cogl/cogl.h>

#include "rbclutter.h"

VALUE rb_c_cogl_vec3 = Qnil;

/* Welcome to your accessor */
static CoglVector3 *
vec3_ptr (VALUE self)
{
  CoglVector3 *v;

  Data_Get_Struct (self, CoglVector3, v);
  return v;
}

static VALUE
rb_cogl_vector3_free (void* inst)
{
  g_slice_free (CoglVector3, inst);
}

/* A "normal protocol" ruby alloc.  _bare below tends to be more
   concise. */
static VALUE
rb_cogl_vector3_alloc (VALUE klass)
{
  CoglVector3 *vector = g_slice_new (CoglVector3);

  return Data_Wrap_Struct (klass, 0, rb_cogl_vector3_free, vector);
}

/* allocate with implied class */
static VALUE
rb_cogl_vector3_alloc_bare ()
{
  return rb_cogl_vector3_alloc (rb_c_cogl_vec3);
}

static VALUE
rb_cogl_vector3_initialize (int argc, VALUE *argv, VALUE self)
{
  VALUE vx, vy, vz;

  switch(rb_scan_args (argc, argv, "03", &vx, &vy, &vz))
    {
      case 0: vx = DBL2NUM (0.0);
      case 1: vy = DBL2NUM (0.0);
      case 2: vz = DBL2NUM (0.0);
    }

  /* Though controversal, nil.to_f is alive and well. */
  cogl_vector3_init (vec3_ptr(self), NUM2DBL(vx), NUM2DBL(vy), NUM2DBL(vz));
  return self;
}

static VALUE
rb_cogl_vector3_eq (VALUE self, VALUE other)
{
  return cogl_vector3_equal (vec3_ptr (self),
                             vec3_ptr (other)) ? Qtrue : Qfalse;
}

static VALUE
rb_cogl_vector3_equal_with_epsilon (VALUE self, VALUE other, VALUE epsilon)
{
  return cogl_vector3_equal_with_epsilon (vec3_ptr (self),
                                          vec3_ptr (other),
                                          NUM2DBL (epsilon)) ? Qtrue :Qfalse;
}

static VALUE
rb_cogl_vector3_to_a (VALUE self)
{
  CoglVector3 v3 = *(vec3_ptr (self));

  return rb_ary_new3 (3, DBL2NUM (v3.x), DBL2NUM (v3.y), DBL2NUM (v3.z));
}

static VALUE
rb_cogl_vector3_invert (VALUE self)
{
  VALUE result = rb_cogl_vector3_alloc_bare ();

  *vec3_ptr(result) = *vec3_ptr (self);
  cogl_vector3_invert (vec3_ptr (result));
  return result;
}

static VALUE
rb_cogl_vector3_add (VALUE self, VALUE other)
{
  VALUE result = rb_cogl_vector3_alloc_bare ();

  cogl_vector3_add (vec3_ptr (result), vec3_ptr (self), vec3_ptr (other));
  return result;
}

static VALUE
rb_cogl_vector3_subtract (VALUE self, VALUE other)
{
  VALUE result = rb_cogl_vector3_alloc_bare ();

  cogl_vector3_subtract (vec3_ptr (result), vec3_ptr (self), vec3_ptr (other));
  return result;
}

static VALUE
rb_cogl_vector3_multiply_scalar (VALUE self, VALUE other)
{
  VALUE result = rb_cogl_vector3_alloc_bare ();

  *vec3_ptr (result) = *vec3_ptr (self);
  cogl_vector3_multiply_scalar (vec3_ptr (result), NUM2DBL (other));
  return result;
}

static VALUE
rb_cogl_vector3_divide_scalar (VALUE self, VALUE other)
{
  VALUE result = rb_cogl_vector3_alloc_bare ();

  *vec3_ptr (result) = *vec3_ptr (self);
  cogl_vector3_divide_scalar (vec3_ptr (result), NUM2DBL (other));
  return result;
}

static VALUE
rb_cogl_vector3_normalize (VALUE self)
{
  VALUE result = rb_cogl_vector3_alloc_bare ();

  *vec3_ptr (result) = *vec3_ptr (self);
  cogl_vector3_normalize (vec3_ptr (result));
  return result;
}

static VALUE
rb_cogl_vector3_magnitude (VALUE self)
{
  return DBL2NUM (cogl_vector3_magnitude (vec3_ptr (self)));
}

static VALUE
rb_cogl_vector3_cross_product (VALUE self, VALUE other)
{
  VALUE result = rb_cogl_vector3_alloc_bare ();

  cogl_vector3_cross_product (vec3_ptr (result), vec3_ptr (self),
                                                 vec3_ptr (other));
  return result;
}

static VALUE
rb_cogl_vector3_dot_product (VALUE self, VALUE other)
{
  return DBL2NUM (cogl_vector3_dot_product (vec3_ptr (self),
                                            vec3_ptr (other)));
}

static VALUE
rb_cogl_vector3_distance (VALUE self, VALUE other)
{
  return DBL2NUM (cogl_vector3_distance (vec3_ptr (self), vec3_ptr (other)));
}

void rb_cogl_vector3_init ()
{
  VALUE klass;

  klass = rb_define_class_under (rbclt_c_cogl, "Vector3", rb_cObject);
  rb_define_alloc_func (klass, rb_cogl_vector3_alloc);

  rb_define_method (klass, "initialize", rb_cogl_vector3_initialize, -1);

  rb_define_method (klass, "==", rb_cogl_vector3_eq, 1);
  rb_define_method (klass, "equal_with_epsilon",
                    rb_cogl_vector3_equal_with_epsilon, 2);
  rb_define_method (klass, "invert", rb_cogl_vector3_invert, 0);
  rb_define_method (klass, "add", rb_cogl_vector3_add, 1);
  rb_define_method (klass, "subtract", rb_cogl_vector3_subtract, 1);
  rb_define_method (klass, "multiply_scalar",
                    rb_cogl_vector3_multiply_scalar, 1);
  rb_define_method (klass, "divide_scalar", rb_cogl_vector3_divide_scalar, 1);
  rb_define_method (klass, "normalize", rb_cogl_vector3_normalize, 0);
  rb_define_method (klass, "magnitude", rb_cogl_vector3_magnitude, 0);
  rb_define_method (klass, "cross_product", rb_cogl_vector3_cross_product, 1);
  rb_define_method (klass, "dot_product", rb_cogl_vector3_dot_product, 1);
  rb_define_method (klass, "distance", rb_cogl_vector3_distance, 1);

  rb_define_alias (klass, "+",  "add");
  rb_define_alias (klass, "-",  "subtract");
  rb_define_alias (klass, "-@", "invert");

  rb_define_alias (klass, "*", "multiply_scalar");
  rb_define_alias (klass, "/", "divide_scalar");

  rb_define_method (klass, "to_a", rb_cogl_vector3_to_a, 0);
  rb_define_alias (klass, "to_s", "inspect");

  rb_c_cogl_vec3 = klass;
}