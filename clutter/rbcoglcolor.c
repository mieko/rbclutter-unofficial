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
#include "rbcoglcolor.h"

static VALUE rb_cogl_c_color;

static void
rb_cogl_color_free (void *data)
{
  g_slice_free (CoglColor, data);
}

static VALUE
rb_cogl_color_alloc_with_class (VALUE klass)
{
  CoglColor *color = g_slice_new (CoglColor);

  return Data_Wrap_Struct (klass, 0, rb_cogl_color_free, color);
}

VALUE
rb_cogl_color_alloc (void)
{
  return rb_cogl_color_alloc_with_class (rb_cogl_c_color);
}

CoglColor *
rb_cogl_color_get_pointer (VALUE self)
{
  CoglColor *color;

  Data_Get_Struct (self, CoglColor, color);

  return color;
}

static VALUE
rb_cogl_color_initialize (int argc, VALUE *argv, VALUE self)
{
  CoglColor *color;
  VALUE red, green, blue, alpha;

  Data_Get_Struct (self, CoglColor, color);

  /* This takes anywhere between 0 and 4 arguments as byte values for
     the components. */

  rb_scan_args (argc, argv, "04", &red, &green, &blue, &alpha);

  cogl_color_init_from_4f (color,
                           NIL_P (red) ? 0.0f : NUM2DBL (red),
                           NIL_P (green) ? 0.0f : NUM2DBL (green),
                           NIL_P (blue) ? 0.0f : NUM2DBL (blue),
                           NIL_P (alpha) ? 1.0f : NUM2DBL (alpha));

  return Qnil;
}

gboolean
rb_cogl_is_kind_of_color (VALUE self)
{
  return TYPE (self) == T_DATA && RDATA (self)->dfree == rb_cogl_color_free;
}

void
rb_cogl_assert_is_kind_of_color (VALUE arg)
{
  if (!rb_cogl_is_kind_of_color (arg))
    rb_raise (rb_eTypeError, "wrong argument type");
}

static VALUE
rb_cogl_color_initialize_copy (VALUE self, VALUE orig)
{
  CoglColor *color_self, *color_orig;

  rb_cogl_assert_is_kind_of_color (orig);

  Data_Get_Struct (self, CoglColor, color_self);
  Data_Get_Struct (orig, CoglColor, color_orig);

  memcpy (color_self, color_orig, sizeof (CoglColor));

  return Qnil;
}

static float (* rb_cogl_color_float_accessors[]) (const CoglColor *) =
{
  cogl_color_get_red_float,
  cogl_color_get_green_float,
  cogl_color_get_blue_float,
  cogl_color_get_alpha_float
};

static VALUE
rb_cogl_color_to_a (VALUE self)
{
  CoglColor *color;
  VALUE array_values[G_N_ELEMENTS (rb_cogl_color_float_accessors)];
  size_t i;

  Data_Get_Struct (self, CoglColor, color);

  for (i = 0; i < G_N_ELEMENTS (rb_cogl_color_float_accessors); i++)
    array_values[i] = rb_float_new (rb_cogl_color_float_accessors[i] (color));

  return rb_ary_new4 (G_N_ELEMENTS (rb_cogl_color_float_accessors),
                      array_values);
}

static VALUE
rb_cogl_color_aref (VALUE self, VALUE index_value)
{
  CoglColor *color;
  int index_num = NUM2INT (index_value);

  Data_Get_Struct (self, CoglColor, color);

  if (index_num < 0
      || index_num >= G_N_ELEMENTS (rb_cogl_color_float_accessors))
    rb_raise (rb_eArgError, "index out of range");

  return rb_float_new (rb_cogl_color_float_accessors[index_num] (color));
}

static VALUE
rb_cogl_color_set_byte (int argc, VALUE *argv, VALUE self)
{
  CoglColor *color;
  VALUE red, green, blue, alpha;

  Data_Get_Struct (self, CoglColor, color);

  /* This takes anywhere between 0 and 4 arguments as byte values for
     the components. */

  rb_scan_args (argc, argv, "04", &red, &green, &blue, &alpha);

  cogl_color_set_from_4ub (color,
                           NIL_P (red) ? 0 : rbclt_num_to_guint8 (red),
                           NIL_P (green) ? 0 : rbclt_num_to_guint8 (green),
                           NIL_P (blue) ? 0 : rbclt_num_to_guint8 (blue),
                           NIL_P (alpha) ? 255 : rbclt_num_to_guint8 (alpha));

  return self;
}

static VALUE
rb_cogl_color_set_float (int argc, VALUE *argv, VALUE self)
{
  rb_cogl_color_initialize (argc, argv, self);

  return self;
}

static VALUE
rb_cogl_color_premultiply_bang (VALUE self)
{
  CoglColor *color;

  Data_Get_Struct (self, CoglColor, color);

  cogl_color_premultiply (color);

  return self;
}

static VALUE
rb_cogl_color_premultiply (VALUE self)
{
  VALUE copy = rb_cogl_color_alloc ();

  rb_cogl_color_initialize_copy (copy, self);

  return rb_cogl_color_premultiply_bang (copy);
}

static VALUE
rb_cogl_color_unpremultiply_bang (VALUE self)
{
  CoglColor *color;

  Data_Get_Struct (self, CoglColor, color);

  cogl_color_unpremultiply (color);

  return self;
}

static VALUE
rb_cogl_color_unpremultiply (VALUE self)
{
  VALUE copy = rb_cogl_color_alloc ();

  rb_cogl_color_initialize_copy (copy, self);

  return rb_cogl_color_unpremultiply_bang (copy);
}

static VALUE
rb_cogl_color_equal (VALUE self, VALUE other)
{
  CoglColor *self_color, *other_color;

  if (!rb_cogl_is_kind_of_color (other))
    return Qfalse;

  Data_Get_Struct (self, CoglColor, self_color);
  Data_Get_Struct (other, CoglColor, other_color);

  return cogl_color_equal (self_color, other_color) ? Qtrue : Qfalse;
}

#define RB_COGL_COMPONENT_ACCESSOR_FUNC(comp)                           \
  static VALUE                                                          \
  rb_cogl_color_get_ ## comp ## _byte (VALUE self)                      \
  {                                                                     \
    CoglColor *color;                                                   \
    Data_Get_Struct (self, CoglColor, color);                           \
    return UINT2NUM (cogl_color_get_ ## comp ## _byte (color));         \
  }                                                                     \
                                                                        \
  static VALUE                                                          \
  rb_cogl_color_get_ ## comp ## _float (VALUE self)                     \
  {                                                                     \
    CoglColor *color;                                                   \
    Data_Get_Struct (self, CoglColor, color);                           \
    return rb_float_new (cogl_color_get_ ## comp ## _float (color));    \
  }

RB_COGL_COMPONENT_ACCESSOR_FUNC (red)
RB_COGL_COMPONENT_ACCESSOR_FUNC (green)
RB_COGL_COMPONENT_ACCESSOR_FUNC (blue)
RB_COGL_COMPONENT_ACCESSOR_FUNC (alpha)

#define RB_COGL_COMPONENT_ACCESSOR(comp)                        \
  G_STMT_START {                                                \
    rb_define_method (klass, G_STRINGIFY (comp) "_byte",        \
                      rb_cogl_color_get_ ## comp ## _byte, 0);  \
    rb_define_method (klass, G_STRINGIFY (comp) "_float",       \
                      rb_cogl_color_get_ ## comp ## _float, 0); \
    rb_define_alias (klass, G_STRINGIFY (comp),                 \
                     G_STRINGIFY (comp) "_float");              \
  } G_STMT_END

void
rb_cogl_color_init ()
{
  VALUE klass;

  klass = rb_define_class_under (rbclt_c_cogl, "Color", rb_cObject);
  rb_cogl_c_color = klass;

  rb_define_alloc_func (klass, rb_cogl_color_alloc_with_class);

  rb_define_method (klass, "initialize", rb_cogl_color_initialize, -1);
  rb_define_method (klass, "initialize_copy",
                    rb_cogl_color_initialize_copy, 1);

  rb_define_method (klass, "to_a", rb_cogl_color_to_a, 0);
  rb_define_method (klass, "[]", rb_cogl_color_aref, 1);

  rb_define_method (klass, "set_byte", rb_cogl_color_set_byte, -1);
  rb_define_alias (klass, "set_from_4ub", "set_byte");
  rb_define_method (klass, "set_float", rb_cogl_color_set_float, -1);
  rb_define_alias (klass, "set_from_4f", "set_float");
  rb_define_alias (klass, "set", "set_float");

  rb_define_method (klass, "premultiply!", rb_cogl_color_premultiply_bang, 0);
  rb_define_method (klass, "premultiply", rb_cogl_color_premultiply, 0);
  rb_define_method (klass, "unpremultiply!",
                    rb_cogl_color_unpremultiply_bang, 0);
  rb_define_method (klass, "unpremultiply", rb_cogl_color_unpremultiply, 0);
  rb_define_method (klass, "==", rb_cogl_color_equal, 1);

  RB_COGL_COMPONENT_ACCESSOR (red);
  RB_COGL_COMPONENT_ACCESSOR (green);
  RB_COGL_COMPONENT_ACCESSOR (blue);
  RB_COGL_COMPONENT_ACCESSOR (alpha);
}
