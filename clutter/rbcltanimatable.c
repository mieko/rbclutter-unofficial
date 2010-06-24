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
#include <clutter/clutter.h>

#include "rbclutter.h"
#include "rbcltactor.h"

static VALUE
rbclt_animatable_animate_property (VALUE self,
                                   VALUE animation,
                                   VALUE property_name,
                                   VALUE initial_value_arg,
                                   VALUE final_value_arg,
                                   VALUE progress)
{
  ClutterAnimatable *animatable = CLUTTER_ANIMATABLE (RVAL2GOBJ (self));
  GValue initial_value = { 0, }, final_value = { 0, }, result = { 0, };
  GType type;
  VALUE ret;

  type = RVAL2GTYPE (initial_value_arg);

  g_value_init (&initial_value, type);
  g_value_init (&final_value, type);
  g_value_init (&result, type);

  rbgobj_rvalue_to_gvalue (initial_value_arg, &initial_value);
  rbgobj_rvalue_to_gvalue (final_value_arg, &final_value);

  if (clutter_animatable_animate_property (animatable,
                                           RVAL2GOBJ (animation),
                                           StringValuePtr (property_name),
                                           &initial_value,
                                           &final_value,
                                           NUM2DBL (progress),
                                           &result))
    ret = GVAL2RVAL (&result);
  else
    ret = Qnil;

  g_value_unset (&result);
  g_value_unset (&final_value);
  g_value_unset (&initial_value);

  return ret;
}

void
rbclt_animatable_init ()
{
  VALUE klass = G_DEF_INTERFACE (CLUTTER_TYPE_ANIMATABLE,
                                 "Animatable", rbclt_c_clutter);

  rb_define_method (klass, "animate_property",
                    rbclt_animatable_animate_property, 5);
}
