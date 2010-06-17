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
#include <clutter-ply/clutter-ply.h>

#include "rbclutter.h"
#include "rbclutterply.h"

static VALUE
rbcltply_model_initialize (int argc, VALUE *argv, VALUE self)
{
  ClutterActor *model;
  VALUE filename;

  rb_scan_args (argc, argv, "01", &filename);

  if (NIL_P (filename))
    model = clutter_ply_model_new ();
  else
    {
      GError *error = NULL;
      model = clutter_ply_model_new_from_file (StringValuePtr (filename),
                                               &error);
      if (model == NULL)
        RAISE_GERROR (error);
    }

  rbclt_initialize_unowned (self, model);

  return Qnil;
}

void
rbcltply_model_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_PLY_TYPE_MODEL, "Model",
                             rbcltply_c_clutter_ply);

  rb_define_method (klass, "initialize", rbcltply_model_initialize, -1);
}
