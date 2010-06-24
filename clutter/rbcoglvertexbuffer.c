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

static ID id_to_add_array;
static ID id_to_delete_array;
static ID id_pack;

#define N_VERTEX_ATTRIBUTE_ARGS 7

static VALUE
rb_cogl_vertex_buffer_initialize (VALUE self, VALUE n_vertices)
{
  CoglHandle vertex_buffer = cogl_vertex_buffer_new (NUM2UINT (n_vertices));

  rb_cogl_handle_initialize (self, vertex_buffer);

  return Qnil;
}

static VALUE
rb_cogl_vertex_buffer_get_n_vertices (VALUE self)
{
  CoglHandle vertex_buffer = rb_cogl_handle_get_handle (self);

  return UINT2NUM (cogl_vertex_buffer_get_n_vertices (vertex_buffer));
}

static VALUE
rb_cogl_vertex_buffer_add (int argc, VALUE *argv, VALUE self)
{
  VALUE values[N_VERTEX_ATTRIBUTE_ARGS];
  VALUE value_array;
  VALUE to_add_array;
  VALUE to_delete_array;

  /* We will collect all of the arguments into an array and store them
     in an array of attributes to add when submit is called */
  rb_scan_args (argc, argv, "61",
                &values[0], &values[1], &values[2], &values[3],
                &values[4], &values[5], &values[6]);

  value_array = rb_ary_new4 (N_VERTEX_ATTRIBUTE_ARGS, values);

  if (TYPE (to_add_array = rb_ivar_get (self, id_to_add_array)) != T_ARRAY)
    rb_ivar_set (self, id_to_add_array, to_add_array = rb_ary_new ());

  rb_ary_push (to_add_array, value_array);

  /* Make sure the attribute is no longer in the array of attributes
     to delete */
  to_delete_array = rb_ivar_get (self, id_to_delete_array);
  if (TYPE (to_delete_array) == T_ARRAY)
    rb_ary_delete (to_delete_array, values[0]);

  return self;
}

static VALUE
rb_cogl_vertex_buffer_delete (VALUE self, VALUE attribute_name)
{
  VALUE to_delete_array;

  /* We will collect the list of attributes to delete into an array */
  to_delete_array = rb_ivar_get (self, id_to_delete_array);
  if (TYPE (to_delete_array) != T_ARRAY)
    rb_ivar_set (self, id_to_delete_array, to_delete_array = rb_ary_new ());

  rb_ary_push (to_delete_array, attribute_name);

  /* We don't need to remove the attribute from the array of
     attributes to add because the deletions are done afterwards so it
     will just work out */

  return self;
}

typedef struct
{
  VALUE attribute_name;
  guint8 n_components;
  CoglAttributeType type;
  gboolean normalized;
  guint16 stride;
  VALUE data;
  guint offset;
} AddData;

static int
attribute_type_size (CoglAttributeType type)
{
  switch (type)
    {
    case COGL_ATTRIBUTE_TYPE_BYTE:
    case COGL_ATTRIBUTE_TYPE_UNSIGNED_BYTE:
      return sizeof (guint8);

    case COGL_ATTRIBUTE_TYPE_SHORT:
    case COGL_ATTRIBUTE_TYPE_UNSIGNED_SHORT:
      return sizeof (guint16);

    case COGL_ATTRIBUTE_TYPE_FLOAT:
      return sizeof (gfloat);
    }

  rb_raise (rb_eRuntimeError, "Invalid CoglAttributeType");

  return 1;
}

static VALUE
convert_to_string (VALUE arg,
                   CoglAttributeType type,
                   guint8 n_components,
                   int stride)
{
  int type_size = attribute_type_size (type);
  VALUE str;

  /* We can accept an array or something that can be converted into a
     string */
  if (TYPE (arg) == T_ARRAY)
    {
      VALUE pack_type = Qnil;

      /* If it's an array then it doesn't make sense to pass a stride
         that doesn't exactly match the packed size */
      if (stride != 0 && stride != n_components * type_size)
        rb_raise (rb_eArgError, "A strided buffer can not be used when "
                  "the data is an array");

      switch (type)
        {
        case COGL_ATTRIBUTE_TYPE_BYTE:
          pack_type = rb_str_new_cstr ("c*");
          break;

        case COGL_ATTRIBUTE_TYPE_UNSIGNED_BYTE:
          pack_type = rb_str_new_cstr ("C*");
          break;

        case COGL_ATTRIBUTE_TYPE_SHORT:
          pack_type = rb_str_new_cstr ("s*");
          break;

        case COGL_ATTRIBUTE_TYPE_UNSIGNED_SHORT:
          pack_type = rb_str_new_cstr ("S*");
          break;

        case COGL_ATTRIBUTE_TYPE_FLOAT:
          pack_type = rb_str_new_cstr ("f*");
          break;
        }

      str = rb_funcall (arg, id_pack, 1, pack_type);

      if (TYPE (str) != T_STRING)
        rb_raise (rb_eRuntimeError, "Internal error");
    }
  else
    {
      str = arg;
      StringValue (str);
    }

  return str;
}

static VALUE
rb_cogl_vertex_buffer_submit (VALUE self)
{
  CoglHandle vertex_buffer = rb_cogl_handle_get_handle (self);
  VALUE to_add_array;
  VALUE to_delete_array;
  AddData *add_data = NULL;
  int n_to_add = 0;
  VALUE *delete_data = NULL;
  int n_to_delete = 0;
  int n_verts = cogl_vertex_buffer_get_n_vertices (vertex_buffer);
  int i;

  if (TYPE (to_add_array = rb_ivar_get (self, id_to_add_array)) == T_ARRAY)
    {
      n_to_add = RARRAY_LEN (to_add_array);

      add_data = alloca (sizeof (AddData) * n_to_add);

      /* Convert all of the values in the array to the right type
         first because converting will call back into Ruby so anything
         could happen. This tries to protect against the size of the
         array changing */
      for (i = 0; i < n_to_add && i < RARRAY_LEN (to_add_array); i++)
        {
          VALUE entry = RARRAY_PTR (to_add_array)[i];
          VALUE entry_values[N_VERTEX_ATTRIBUTE_ARGS];

          if (TYPE (entry) != T_ARRAY
              || RARRAY_LEN (entry) != N_VERTEX_ATTRIBUTE_ARGS)
            rb_raise (rb_eRuntimeError, "Internal error");

          memcpy (entry_values, RARRAY_PTR (entry),
                  sizeof (VALUE) * N_VERTEX_ATTRIBUTE_ARGS);

          add_data[i].attribute_name = entry_values[0];
          StringValue (add_data[i].attribute_name);
          add_data[i].n_components = rbclt_num_to_guint8 (entry_values[1]);
          add_data[i].type = RVAL2GENUM (entry_values[2],
                                         COGL_TYPE_ATTRIBUTE_TYPE);
          add_data[i].normalized = RTEST (entry_values[3]);
          add_data[i].stride = rbclt_num_to_guint16 (entry_values[4]);
          add_data[i].data = convert_to_string (entry_values[5],
                                                add_data[i].type,
                                                add_data[i].stride,
                                                add_data[i].n_components);
          add_data[i].offset = (NIL_P (entry_values[6]) ? 0
                                : NUM2UINT (entry_values[6]));
        }

      if (i != n_to_add)
        rb_raise (rb_eRuntimeError, "Internal error");

      rb_ivar_set (self, id_to_add_array, Qnil);
    }

  to_delete_array = rb_ivar_get (self, id_to_delete_array);
  if (TYPE (to_delete_array) == T_ARRAY)
    {
      n_to_delete = RARRAY_LEN (to_delete_array);

      delete_data = alloca (sizeof (VALUE) * n_to_delete);

      /* Convert all of the values in the array to the right type
         first because converting will call back into Ruby so anything
         could happen. This tries to protect against the size of the
         array changing */
      for (i = 0; i < n_to_delete && i < RARRAY_LEN (to_delete_array); i++)
        {
          delete_data[i] = RARRAY_PTR (to_delete_array)[i];
          StringValue (delete_data[i]);
        }

      if (i != n_to_delete)
        rb_raise (rb_eRuntimeError, "Internal error");

      rb_ivar_set (self, id_to_delete_array, Qnil);
    }

  /* The rest of this function should happen without calling back into
     Ruby */

  /* Verify that all of the strings are big enough */
  for (i = 0; i < n_to_add; i++)
    {
      int type_size = attribute_type_size (add_data[i].type);

      if (add_data[i].stride == 0)
        add_data[i].stride = add_data[i].n_components * type_size;
      else if (add_data[i].stride < add_data[i].n_components * type_size)
        rb_raise (rb_eArgError, "Stride is too short for %s",
                  StringValuePtr (add_data[i].attribute_name));

      if (n_verts > 0
          && (RSTRING_LEN (add_data[i].data) - add_data[i].offset
              < (add_data[i].stride * (n_verts - 1)
                 + add_data[i].n_components * type_size)))
        rb_raise (rb_eArgError, "Data is too short for %s",
                  StringValuePtr (add_data[i].attribute_name));
    }

  /* We are now past the point of no return. Nothing should raise an
     error or call back into Ruby past this point */

  for (i = 0; i < n_to_add; i++)
    cogl_vertex_buffer_add (vertex_buffer,
                            StringValuePtr (add_data[i].attribute_name),
                            add_data[i].n_components,
                            add_data[i].type,
                            add_data[i].normalized,
                            add_data[i].stride,
                            StringValuePtr (add_data[i].data)
                            + add_data[i].offset);

  for (i = 0; i < n_to_delete; i++)
    cogl_vertex_buffer_delete (vertex_buffer,
                               StringValuePtr (delete_data[i]));

  cogl_vertex_buffer_submit (vertex_buffer);

  return self;
}

static VALUE
rb_cogl_vertex_buffer_enable (VALUE self, VALUE attribute_name)
{
  CoglHandle vertex_buffer = rb_cogl_handle_get_handle (self);

  cogl_vertex_buffer_enable (vertex_buffer, StringValuePtr (attribute_name));

  return self;
}

static VALUE
rb_cogl_vertex_buffer_disable (VALUE self, VALUE attribute_name)
{
  CoglHandle vertex_buffer = rb_cogl_handle_get_handle (self);

  cogl_vertex_buffer_disable (vertex_buffer, StringValuePtr (attribute_name));

  return self;
}

static VALUE
rb_cogl_vertex_buffer_draw (int argc, VALUE *argv, VALUE self)
{
  CoglHandle vertex_buffer = rb_cogl_handle_get_handle (self);
  VALUE mode, first_arg, count_arg;
  guint first, count;
  int n_verts = cogl_vertex_buffer_get_n_vertices (vertex_buffer);

  rb_scan_args (argc, argv, "12", &mode, &first_arg, &count_arg);

  if (NIL_P (first_arg))
    first = 0;
  else
    first = NUM2UINT (first_arg);

  if (first >= n_verts)
    rb_raise (rb_eArgError, "The 'first' argument is too high (%u >= %u)",
              first, n_verts);

  if (NIL_P (count_arg))
    count = n_verts - first;
  else
    count = NUM2UINT (count_arg);

  if (count + first > n_verts)
    rb_raise (rb_eArgError, "The 'count' argument is too large (%u > %u)",
              count, n_verts - first);

  /* cogl_vertex_buffer_draw calls submit automatically. However the
     calls from ruby to add and remove are delayed so we need to call
     the ruby version of submit here to simulate that behaviour */
  rb_cogl_vertex_buffer_submit (self);

  cogl_vertex_buffer_draw (vertex_buffer,
                           RVAL2GENUM (mode, COGL_TYPE_VERTICES_MODE),
                           first, count);

  return self;
}

void
rb_cogl_vertex_buffer_init ()
{
  VALUE klass = rb_cogl_define_handle (cogl_is_vertex_buffer, "VertexBuffer");

  G_DEF_CLASS (COGL_TYPE_VERTICES_MODE, "VerticesMode", rbclt_c_cogl);
  G_DEF_CLASS (COGL_TYPE_ATTRIBUTE_TYPE, "AttributeType", rbclt_c_cogl);
  G_DEF_CLASS (COGL_TYPE_INDICES_TYPE, "IndicesType", rbclt_c_cogl);

  id_to_add_array = rb_intern ("_prv_to_add");
  id_to_delete_array = rb_intern ("_prv_to_delete");
  id_pack = rb_intern ("pack");

  rb_define_method (klass, "initialize", rb_cogl_vertex_buffer_initialize, 1);
  rb_define_method (klass, "n_vertices",
                    rb_cogl_vertex_buffer_get_n_vertices, 0);
  rb_define_method (klass, "add", rb_cogl_vertex_buffer_add, -1);
  rb_define_method (klass, "delete", rb_cogl_vertex_buffer_delete, 1);
  rb_define_method (klass, "submit", rb_cogl_vertex_buffer_submit, 0);
  rb_define_method (klass, "enable", rb_cogl_vertex_buffer_enable, 1);
  rb_define_method (klass, "disable", rb_cogl_vertex_buffer_disable, 1);
  rb_define_method (klass, "draw", rb_cogl_vertex_buffer_draw, -1);
}
