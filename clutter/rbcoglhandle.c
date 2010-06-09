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

VALUE rb_c_cogl_handle;

static GSList *rb_cogl_handle_types = NULL;

typedef struct _RBCoglHandleType RBCoglHandleType;

struct _RBCoglHandleType
{
  RBCoglHandleTypeCheckFunc type_check;
  VALUE klass;
};

void
rb_cogl_handle_initialize (VALUE self, CoglHandle handle)
{
  G_INITIALIZE (self, handle);
  cogl_handle_unref (handle);
}

VALUE
rb_cogl_handle_to_value (CoglHandle handle)
{
  VALUE ret;

  if (handle == COGL_INVALID_HANDLE)
    ret = Qnil;
  else
    {
      GSList *l;
      VALUE klass = rb_c_cogl_handle;;

      for (l = rb_cogl_handle_types; l; l = l->next)
        {
          RBCoglHandleType *type = l->data;

          if (type->type_check (handle))
            {
              klass = type->klass;
              break;
            }
        }

      ret = BOXED2RVAL (handle, COGL_TYPE_HANDLE);
      /* This is copied from rbgdkevent.c and where it is commented as a hack */
      RBASIC (ret)->klass = klass;
    }

  return ret;
}

VALUE
rb_cogl_handle_to_value_unref (CoglHandle handle)
{
  VALUE ret;

  if (handle == COGL_INVALID_HANDLE)
    return Qnil;

  ret = rb_cogl_handle_to_value (handle);

  cogl_handle_unref (handle);

  return ret;
}

CoglHandle
rb_cogl_handle_get_handle (VALUE obj)
{
  return (CoglHandle) RVAL2BOXED (obj, COGL_TYPE_HANDLE);
}

static VALUE
rb_cogl_handle_g2r (const GValue *value)
{
  return rb_cogl_handle_to_value (g_value_get_boxed (value));
}

void
rb_cogl_handle_init ()
{
  VALUE klass = G_DEF_CLASS (COGL_TYPE_HANDLE, "Handle",
                             rbclt_c_cogl);
  rb_c_cogl_handle = klass;

  rbgobj_register_g2r_func (COGL_TYPE_HANDLE, rb_cogl_handle_g2r);
}

VALUE
rb_cogl_define_handle (RBCoglHandleTypeCheckFunc type_check,
                       const char *class_name)
{
  RBCoglHandleType *type = g_slice_new (RBCoglHandleType);

  type->type_check = type_check;
  type->klass = rb_define_class_under (rbclt_c_cogl, class_name,
                                       rb_c_cogl_handle);

  rb_cogl_handle_types = g_slist_prepend (rb_cogl_handle_types, type);

  return type->klass;
}
