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
rbmash_model_initialize (int argc, VALUE *argv, VALUE self)
{
  ClutterActor *model;
  VALUE flags, filename;

  rb_scan_args (argc, argv, "02", &flags, &filename);

  if (NIL_P (filename))
    model = mash_model_new ();
  else
    {
      GError *error = NULL;
      model = mash_model_new_from_file (RVAL2GENUM (flags,
                                                    MASH_TYPE_DATA_FLAGS),
                                        StringValuePtr (filename),
                                        &error);
      if (model == NULL)
        RAISE_GERROR (error);
    }

  rbclt_initialize_unowned (self, model);

  return Qnil;
}

void
rbmash_model_init ()
{
  VALUE klass = G_DEF_CLASS (MASH_TYPE_MODEL, "Model",
                             rbmash_c_mash);

  rb_define_method (klass, "initialize", rbmash_model_initialize, -1);
}
