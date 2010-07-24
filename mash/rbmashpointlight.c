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
#include <mash/mash.h>

#include "rbclutter.h"
#include "rbmash.h"

static VALUE
rbmash_point_light_initialize (VALUE self)
{
  ClutterActor *box = mash_point_light_new ();

  rbclt_initialize_unowned (self, box);

  return Qnil;
}

void
rbmash_point_light_init ()
{
  VALUE klass = G_DEF_CLASS (MASH_TYPE_POINT_LIGHT, "PointLight",
                             rbmash_c_mash);

  rb_define_method (klass, "initialize", rbmash_point_light_initialize, 0);
}
