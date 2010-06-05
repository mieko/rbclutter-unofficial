/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2007  Neil Roberts
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
#include "rbcltpath.h"

static ClutterPath *
create_path_from_knots (int len, VALUE *values)
{
  int i;
  ClutterPath *path = clutter_path_new ();

  for (i = 0; i < len; i++)
    {
      ClutterKnot *knot = RVAL2BOXED (values[i], CLUTTER_TYPE_KNOT);

      if (i == 0)
        clutter_path_add_move_to (path, knot->x, knot->y);
      else
        clutter_path_add_line_to (path, knot->x, knot->y);
    }

  return path;
}

static VALUE
rbclt_behaviour_path_initialize (int argc, VALUE *argv, VALUE self)
{
  ClutterBehaviour *behaviour = clutter_behaviour_path_new (NULL, NULL);
  VALUE alpha, path;

  rb_scan_args (argc, argv, "02", &alpha, &path);

  G_INITIALIZE (self, behaviour);

  if (!NIL_P (alpha))
    clutter_behaviour_set_alpha (behaviour, RVAL2GOBJ (alpha));
  if (!NIL_P (path))
    {
      if (RTEST (rb_obj_is_kind_of (path, rbclt_c_path)))
        clutter_behaviour_path_set_path (CLUTTER_BEHAVIOUR_PATH (behaviour),
                                         RVAL2GOBJ (path));
      else
        {
          ClutterPath *path_obj;

          if (TYPE (path) == T_ARRAY)
            path_obj = create_path_from_knots (RARRAY_LEN (path),
                                               RARRAY_PTR (path));
          else
            path_obj
              = clutter_path_new_with_description (StringValuePtr (path));

          clutter_behaviour_path_set_path (CLUTTER_BEHAVIOUR_PATH (behaviour),
                                           path_obj);
        }
    }

  return Qnil;
}

void
rbclt_behaviour_path_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_TYPE_BEHAVIOUR_PATH, "BehaviourPath", rbclt_c_clutter);

  rb_define_method (klass, "initialize", rbclt_behaviour_path_initialize, -1);
}
