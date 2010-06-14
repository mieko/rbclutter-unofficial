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
#include <clutter/clutter.h>

#include "rbclutter.h"
#include "rbcoglhandle.h"
#include "rbcoglcolor.h"
#include "rbcogltexture.h"
#include "rbcoglmatrix.h"

static VALUE rb_c_cogl_material_layer;

static VALUE
rb_cogl_material_initialize (VALUE self)
{
  CoglHandle material = cogl_material_new ();

  rb_cogl_handle_initialize (self, material);

  return Qnil;
}

static VALUE
rb_cogl_material_dup (VALUE self)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  return rb_cogl_handle_to_value_unref (cogl_material_copy (material));
}

static VALUE
rb_cogl_material_set_color4ub (VALUE self,
                               VALUE red,
                               VALUE green,
                               VALUE blue,
                               VALUE alpha)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);

  cogl_material_set_color4ub (material,
                              rbclt_num_to_guint8 (red),
                              rbclt_num_to_guint8 (green),
                              rbclt_num_to_guint8 (blue),
                              rbclt_num_to_guint8 (alpha));

  return self;
}

static VALUE
rb_cogl_material_set_color4f (VALUE self,
                              VALUE red,
                              VALUE green,
                              VALUE blue,
                              VALUE alpha)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);

  cogl_material_set_color4f (material,
                             NUM2DBL (red),
                             NUM2DBL (green),
                             NUM2DBL (blue),
                             NUM2DBL (alpha));

  return self;
}

static VALUE
rb_cogl_material_set_shininess (VALUE self, VALUE shininess)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);

  cogl_material_set_shininess (material, NUM2DBL (shininess));

  return self;
}

static VALUE
rb_cogl_material_get_shininess (VALUE self)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);

  return rb_float_new (cogl_material_get_shininess (material));
}

static VALUE
rb_cogl_material_set_alpha_test_function (VALUE self,
                                          VALUE func_arg,
                                          VALUE reference)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  CoglMaterialAlphaFunc func = RVAL2GENUM (func_arg,
                                           COGL_TYPE_MATERIAL_ALPHA_FUNC);

  cogl_material_set_alpha_test_function (material, func, NUM2DBL (reference));

  return self;
}

static VALUE
rb_cogl_material_set_blend (VALUE self, VALUE blend)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  GError *error = NULL;

  cogl_material_set_blend (material, StringValuePtr (blend), &error);

  if (error)
    RAISE_GERROR (error);

  return self;
}

static VALUE
rb_cogl_material_set_layer (VALUE self, VALUE layer_index, VALUE texture_arg)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  CoglHandle texture;

  if (!rb_obj_is_kind_of (texture_arg, rb_c_cogl_texture))
    rb_raise (rb_eArgError, "Cogl::Texture instance expected");

  texture = rb_cogl_handle_get_handle (texture_arg);

  cogl_material_set_layer (material, NUM2INT (layer_index), texture);

  return self;
}

static VALUE
rb_cogl_material_remove_layer (VALUE self, VALUE layer_index)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);

  cogl_material_remove_layer (material, NUM2INT (layer_index));

  return self;
}

static VALUE
rb_cogl_material_set_layer_combine (VALUE self,
                                    VALUE layer_index,
                                    VALUE combine)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  GError *error = NULL;

  if (!cogl_material_set_layer_combine (material,
                                        NUM2INT (layer_index),
                                        StringValuePtr (combine),
                                        &error))
    {
      if (error)
        RAISE_GERROR (error);
      else
        rb_raise (rb_eRuntimeError, "Failed to set layer combine");
    }

  return self;
}

static VALUE
rb_cogl_material_set_layer_combine_constant (VALUE self,
                                             VALUE layer_num,
                                             VALUE color_arg)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  CoglColor *color;

  rb_cogl_assert_is_kind_of_color (color_arg);

  color = rb_cogl_color_get_pointer (color_arg);

  cogl_material_set_layer_combine_constant (material,
                                            NUM2INT (layer_num),
                                            color);

  return self;
}

static VALUE
rb_cogl_material_set_layer_matrix (VALUE self,
                                   VALUE layer_num,
                                   VALUE matrix_arg)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  CoglMatrix *matrix;

  rb_cogl_assert_is_kind_of_matrix (matrix_arg);

  matrix = rb_cogl_matrix_get_pointer (matrix_arg);

  cogl_material_set_layer_matrix (material,
                                  NUM2INT (layer_num),
                                  matrix);

  return self;
}

static VALUE
rb_cogl_material_get_layers (VALUE self)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  const GList *l;
  VALUE ret = rb_ary_new ();

  for (l = cogl_material_get_layers (material); l; l = l->next)
    {
      VALUE layer = rb_cogl_handle_to_value (l->data);
      /* There's no ruby handle conversion function registered for
         CoglMaterialLayer so we have to manually force the klass */
      RBASIC (layer)->klass = rb_c_cogl_material_layer;
      rb_ary_push (ret, layer);
    }

  return ret;
}

static VALUE
rb_cogl_material_get_n_layers (VALUE self)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  return INT2NUM (cogl_material_get_n_layers (material));
}

static VALUE
rb_cogl_material_set_layer_filters (VALUE self,
                                    VALUE layer_num,
                                    VALUE min_filter,
                                    VALUE mag_filter)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);

  cogl_material_set_layer_filters (material,
                                   NUM2INT (layer_num),
                                   RVAL2GENUM (min_filter,
                                               COGL_TYPE_MATERIAL_FILTER),
                                   RVAL2GENUM (mag_filter,
                                               COGL_TYPE_MATERIAL_FILTER));

  return self;
}

#if CLUTTER_CHECK_VERSION(1, 3, 2)

static VALUE
rb_cogl_material_set_layer_wrap_mode (VALUE self,
                                      VALUE layer_num,
                                      VALUE mode_arg)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  CoglMaterialWrapMode mode
    = RVAL2GENUM (mode_arg, COGL_TYPE_MATERIAL_WRAP_MODE);

  cogl_material_set_layer_wrap_mode (material, NUM2INT (layer_num), mode);

  return self;
}

static VALUE
rb_cogl_material_set_layer_wrap_mode_s (VALUE self,
                                        VALUE layer_num,
                                        VALUE mode_arg)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  CoglMaterialWrapMode mode
    = RVAL2GENUM (mode_arg, COGL_TYPE_MATERIAL_WRAP_MODE);

  cogl_material_set_layer_wrap_mode_s (material, NUM2INT (layer_num), mode);

  return self;
}

static VALUE
rb_cogl_material_set_layer_wrap_mode_t (VALUE self,
                                        VALUE layer_num,
                                        VALUE mode_arg)
{
  CoglHandle material = rb_cogl_handle_get_handle (self);
  CoglMaterialWrapMode mode
    = RVAL2GENUM (mode_arg, COGL_TYPE_MATERIAL_WRAP_MODE);

  cogl_material_set_layer_wrap_mode_t (material, NUM2INT (layer_num), mode);

  return self;
}

static VALUE
rb_cogl_material_layer_get_wrap_mode_s (VALUE self)
{
  CoglHandle layer = rb_cogl_handle_get_handle (self);
  return GENUM2RVAL (cogl_material_layer_get_wrap_mode_s (layer),
                     COGL_TYPE_MATERIAL_WRAP_MODE);
}

static VALUE
rb_cogl_material_layer_get_wrap_mode_t (VALUE self)
{
  CoglHandle layer = rb_cogl_handle_get_handle (self);
  return GENUM2RVAL (cogl_material_layer_get_wrap_mode_t (layer),
                     COGL_TYPE_MATERIAL_WRAP_MODE);
}

#endif /* CLUTTER_CHECK_VERSION(1, 3, 2) */

static VALUE
rb_cogl_material_layer_get_type (VALUE self)
{
  CoglHandle layer = rb_cogl_handle_get_handle (self);
  return GENUM2RVAL (cogl_material_layer_get_type (layer),
                     COGL_TYPE_MATERIAL_LAYER_TYPE);
}

static VALUE
rb_cogl_material_layer_get_texture (VALUE self)
{
  CoglHandle layer = rb_cogl_handle_get_handle (self);
  return rb_cogl_handle_to_value (cogl_material_layer_get_texture (layer));
}

static VALUE
rb_cogl_material_layer_get_min_filter (VALUE self)
{
  CoglHandle layer = rb_cogl_handle_get_handle (self);
  return GENUM2RVAL (cogl_material_layer_get_min_filter (layer),
                     COGL_TYPE_MATERIAL_FILTER);
}

static VALUE
rb_cogl_material_layer_get_mag_filter (VALUE self)
{
  CoglHandle layer = rb_cogl_handle_get_handle (self);
  return GENUM2RVAL (cogl_material_layer_get_mag_filter (layer),
                     COGL_TYPE_MATERIAL_FILTER);
}

#define RB_COGL_MATERIAL_COLOR_WRITER_FUNC(prop)                \
  static VALUE                                                  \
  rb_cogl_material_set_ ## prop (VALUE self, VALUE color_arg)   \
  {                                                             \
    CoglHandle material = rb_cogl_handle_get_handle (self);     \
    CoglColor *color;                                           \
                                                                \
    rb_cogl_assert_is_kind_of_color (color_arg);                \
                                                                \
    color = rb_cogl_color_get_pointer (color_arg);              \
                                                                \
    cogl_material_set_ ## prop (material, color);               \
                                                                \
    return self;                                                \
  }                                                             \

#define RB_COGL_MATERIAL_COLOR_READER_FUNC(prop)                \
  static VALUE                                                  \
  rb_cogl_material_get_ ## prop (VALUE self)                    \
  {                                                             \
    CoglHandle material = rb_cogl_handle_get_handle (self);     \
    VALUE ret = rb_cogl_color_alloc ();                         \
    CoglColor *color = rb_cogl_color_get_pointer (ret);         \
                                                                \
    cogl_material_get_ ## prop (material, color);               \
                                                                \
    return ret;                                                 \
  }

#define RB_COGL_MATERIAL_COLOR_ACCESSOR_FUNC(prop)      \
  RB_COGL_MATERIAL_COLOR_WRITER_FUNC (prop)             \
  RB_COGL_MATERIAL_COLOR_READER_FUNC (prop)             \

RB_COGL_MATERIAL_COLOR_ACCESSOR_FUNC (color)
RB_COGL_MATERIAL_COLOR_ACCESSOR_FUNC (ambient)
RB_COGL_MATERIAL_COLOR_ACCESSOR_FUNC (diffuse)
RB_COGL_MATERIAL_COLOR_WRITER_FUNC (ambient_and_diffuse)
RB_COGL_MATERIAL_COLOR_ACCESSOR_FUNC (specular)
RB_COGL_MATERIAL_COLOR_ACCESSOR_FUNC (emission)
RB_COGL_MATERIAL_COLOR_WRITER_FUNC (blend_constant)

#define RB_COGL_MATERIAL_COLOR_READER(prop)             \
  rb_define_method (klass, G_STRINGIFY (prop),          \
                    rb_cogl_material_get_ ## prop, 0)
#define RB_COGL_MATERIAL_COLOR_WRITER(prop)             \
  rb_define_method (klass, "set_" G_STRINGIFY (prop),   \
                    rb_cogl_material_set_ ## prop, 1)
#define RB_COGL_MATERIAL_COLOR_ACCESSOR(prop)   \
  G_STMT_START {                                \
    RB_COGL_MATERIAL_COLOR_WRITER (prop);       \
    RB_COGL_MATERIAL_COLOR_READER (prop);       \
  } G_STMT_END

void
rb_cogl_material_init ()
{
  VALUE klass = rb_cogl_define_handle (cogl_is_material, "Material");

  G_DEF_CLASS (COGL_TYPE_MATERIAL_FILTER, "Filter", klass);
  G_DEF_CONSTANTS (klass, COGL_TYPE_MATERIAL_FILTER, "COGL_MATERIAL_");

  G_DEF_CLASS (COGL_TYPE_MATERIAL_ALPHA_FUNC, "AlphaFunc", klass);
  G_DEF_CONSTANTS (klass, COGL_TYPE_MATERIAL_ALPHA_FUNC, "COGL_MATERIAL_");

  rb_define_method (klass, "initialize", rb_cogl_material_initialize, 0);
  rb_define_method (klass, "dup", rb_cogl_material_dup, 0);

  rb_define_method (klass, "set_color4ub", rb_cogl_material_set_color4ub, 4);
  rb_define_method (klass, "set_color4f", rb_cogl_material_set_color4f, 4);

  rb_define_method (klass, "shininess", rb_cogl_material_get_shininess, 0);
  rb_define_method (klass, "set_shininess", rb_cogl_material_set_shininess, 1);

  rb_define_method (klass, "set_alpha_test_function",
                    rb_cogl_material_set_alpha_test_function, 2);

  rb_define_method (klass, "set_blend", rb_cogl_material_set_blend, 1);

  rb_define_method (klass, "set_layer", rb_cogl_material_set_layer, 2);
  rb_define_method (klass, "remove_layer", rb_cogl_material_remove_layer, 1);
  rb_define_method (klass, "set_layer_combine",
                    rb_cogl_material_set_layer_combine, 2);
  rb_define_method (klass, "set_layer_combine_constant",
                    rb_cogl_material_set_layer_combine_constant, 2);
  rb_define_method (klass, "set_layer_matrix",
                    rb_cogl_material_set_layer_matrix, 2);

  rb_define_method (klass, "layers", rb_cogl_material_get_layers, 0);
  rb_define_method (klass, "n_layers", rb_cogl_material_get_n_layers, 0);

  rb_define_method (klass, "set_layer_filters",
                    rb_cogl_material_set_layer_filters, 3);


  RB_COGL_MATERIAL_COLOR_ACCESSOR (color);
  RB_COGL_MATERIAL_COLOR_ACCESSOR (ambient);
  RB_COGL_MATERIAL_COLOR_ACCESSOR (diffuse);
  RB_COGL_MATERIAL_COLOR_WRITER (ambient_and_diffuse);
  RB_COGL_MATERIAL_COLOR_ACCESSOR (specular);
  RB_COGL_MATERIAL_COLOR_ACCESSOR (emission);
  RB_COGL_MATERIAL_COLOR_WRITER (blend_constant);

  G_DEF_SETTERS (klass);

  /* There's no cogl_is_material_layer function so we can't use
     rb_cogl_define_handle */
  klass = rb_define_class_under (klass, "Layer", rb_c_cogl_handle);
  rb_c_cogl_material_layer = klass;

  rb_define_method (klass, "type", rb_cogl_material_layer_get_type, 0);
  rb_define_method (klass, "texture", rb_cogl_material_layer_get_texture, 0);
  rb_define_method (klass, "min_filter",
                    rb_cogl_material_layer_get_min_filter, 0);
  rb_define_method (klass, "mag_filter",
                    rb_cogl_material_layer_get_mag_filter, 0);

#if CLUTTER_CHECK_VERSION(1, 3, 2)

  G_DEF_CLASS (COGL_TYPE_MATERIAL_WRAP_MODE, "WrapMode", klass);
  G_DEF_CONSTANTS (klass, COGL_TYPE_MATERIAL_WRAP_MODE, "COGL_MATERIAL_");

  rb_define_method (klass, "set_layer_wrap_mode",
                    rb_cogl_material_set_layer_wrap_mode, 2);
  rb_define_method (klass, "set_layer_wrap_mode_s",
                    rb_cogl_material_set_layer_wrap_mode_s, 2);
  rb_define_method (klass, "set_layer_wrap_mode_t",
                    rb_cogl_material_set_layer_wrap_mode_t, 2);

  rb_define_method (klass, "wrap_mode_s",
                    rb_cogl_material_layer_get_wrap_mode_s, 0);
  rb_define_method (klass, "wrap_mode_t",
                    rb_cogl_material_layer_get_wrap_mode_t, 0);

#endif /* CLUTTER_CHECK_VERSION(1, 3, 2) */

  G_DEF_CLASS (COGL_TYPE_MATERIAL_LAYER_TYPE, "Type", klass);
  G_DEF_CONSTANTS (klass, COGL_TYPE_MATERIAL_LAYER_TYPE,
                   "COGL_MATERIAL_LAYER_TYPE_");

}
