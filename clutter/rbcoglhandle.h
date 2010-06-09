/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2010  Neil Roberts
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

#ifndef _RBCOGL_HANDLE_H
#define _RBCOGL_HANDLE_H

#include <cogl/cogl.h>
#include <ruby.h>

extern VALUE rb_c_cogl_handle;

typedef gboolean (* RBCoglHandleTypeCheckFunc) (CoglHandle handle);

void rb_cogl_handle_initialize (VALUE self, CoglHandle handle);
VALUE rb_cogl_handle_to_value (CoglHandle handle);
VALUE rb_cogl_handle_to_value_unref (CoglHandle handle);

CoglHandle rb_cogl_handle_get_handle (VALUE obj);

VALUE rb_cogl_define_handle (RBCoglHandleTypeCheckFunc type_check,
                             const char *class_name);

#endif /* _RBCOGL_HANDLE_H */
