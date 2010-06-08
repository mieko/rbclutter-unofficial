/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright 2010  Intel Corporation
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

#ifndef _RB_COGL_COLOR_H
#define _RB_COGL_COLOR_H

#include <ruby.h>
#include <cogl/cogl.h>

/* Allocates an uninitalized CoglColor wrapped in a VALUE */
VALUE rb_cogl_color_alloc (void);

CoglColor *rb_cogl_color_get_pointer (VALUE self);

gboolean rb_cogl_is_kind_of_color (VALUE self);
void rb_cogl_assert_is_kind_of_color (VALUE arg);

#endif /* _RB_COGL_COLOR_H */
