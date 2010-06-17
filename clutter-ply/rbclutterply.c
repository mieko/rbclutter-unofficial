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
#include <clutter-ply/clutter-ply.h>

#include "rbclutter.h"
#include "rbclutterply.h"

VALUE rbcltply_c_clutter_ply = Qnil;

extern void rbcltply_data_init ();
extern void rbcltply_model_init ();

void
Init_clutter_ply ()
{
  rb_require ("glib2");
  rb_require ("clutter");

  rbcltply_c_clutter_ply = rb_define_module_under (rbclt_c_clutter, "Ply");

  rbcltply_data_init ();
  rbcltply_model_init ();
}
