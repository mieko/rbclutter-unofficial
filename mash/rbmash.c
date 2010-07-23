/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2007  Neil Roberts
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

#include <ruby.h>
#include <rbgobject.h>
#include <mash/mash.h>

#include "rbclutter.h"
#include "rbmash.h"

VALUE rbmash_c_mash = Qnil;

extern void rbmash_data_init ();
extern void rbmash_model_init ();

void
Init_mash ()
{
  rb_require ("glib2");
  rb_require ("clutter");

  rbmash_c_mash = rb_define_module ("Mash");

  rbmash_data_init ();
  rbmash_model_init ();
}
