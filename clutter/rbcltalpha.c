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

#include <rbgobject.h>
#include <clutter/clutter.h>

#include "rbclutter.h"

static VALUE
rbclt_alpha_initialize (int argc, VALUE *argv, VALUE self)
{
  VALUE timeline, mode;
  ClutterAlpha *alpha;

  rb_scan_args (argc, argv, "02", &timeline, &mode);

  alpha = clutter_alpha_new ();
  rbclt_initialize_unowned (self, alpha);

  if (timeline != Qnil)
    clutter_alpha_set_timeline (alpha, RVAL2GOBJ (timeline));
  if (mode != Qnil)
    clutter_alpha_set_mode (alpha, NUM2UINT (mode));

  return Qnil;
}

void
rbclt_alpha_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_TYPE_ALPHA, "Alpha", rbclt_c_clutter);

  rb_define_method (klass, "initialize", rbclt_alpha_initialize, -1);
}
