/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2008  Neil Roberts
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
#include <cogl/cogl.h>

#include "rbclutter.h"

typedef struct _PolyData PolyData;

struct _PolyData
{
  int argc;
  VALUE *argv;
  float *points;
  void (* func) (const float *coords, gint num_points);
};

static VALUE
rb_cogl_set_source_color (int argc, VALUE *argv, VALUE self)
{
  ClutterColor color;

  /* Accept either a single ClutterColor argument or up to four
     components */
  if (argc == 1
      && RTEST (rb_obj_is_kind_of (argv[0], GTYPE2CLASS (CLUTTER_TYPE_COLOR))))
    color = *(const ClutterColor *) RVAL2BOXED (argv[0], CLUTTER_TYPE_COLOR);
  else
    {
      VALUE r, g, b, a;

      rb_scan_args (argc, argv, "04", &r, &g, &b, &a);

      color.red = NIL_P (r) ? 0 : rbclt_num_to_guint8 (r);
      color.green = NIL_P (g) ? 0 : rbclt_num_to_guint8 (g);
      color.blue = NIL_P (b) ? 0 : rbclt_num_to_guint8 (b);
      color.alpha = NIL_P (a) ? 255 : rbclt_num_to_guint8 (a);
    }

  cogl_set_source_color4ub (color.red,
                            color.green,
                            color.blue,
                            color.alpha);
  return Qnil;
}

static VALUE
rb_cogl_rectangle (VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2)
{
  cogl_rectangle (NUM2DBL (x1), NUM2DBL (y1),
                  NUM2DBL (x2), NUM2DBL (y2));

  return Qnil;
}

static VALUE
rb_cogl_path_fill (VALUE self)
{
  cogl_path_fill ();

  return Qnil;
}

static VALUE
rb_cogl_path_stroke (VALUE self)
{
  cogl_path_stroke ();

  return Qnil;
}

static VALUE
rb_cogl_path_move_to (VALUE self, VALUE x, VALUE y)
{
  cogl_path_move_to (NUM2DBL (x),
                     NUM2DBL (y));

  return Qnil;
}

static VALUE
rb_cogl_path_rel_move_to (VALUE self, VALUE x, VALUE y)
{
  cogl_path_rel_move_to (NUM2DBL (x),
                         NUM2DBL (y));

  return Qnil;
}

static VALUE
rb_cogl_path_line_to (VALUE self, VALUE x, VALUE y)
{
  cogl_path_line_to (NUM2DBL (x),
                     NUM2DBL (y));

  return Qnil;
}

static VALUE
rb_cogl_path_rel_line_to (VALUE self, VALUE x, VALUE y)
{
  cogl_path_rel_line_to (NUM2DBL (x),
                         NUM2DBL (y));

  return Qnil;
}

static VALUE
rb_cogl_path_arc (VALUE self,
                  VALUE center_x,
                  VALUE center_y,
                  VALUE radius_x,
                  VALUE radius_y,
                  VALUE angle_1,
                  VALUE angle_2)
{
  cogl_path_arc (NUM2DBL (center_x),
                 NUM2DBL (center_y),
                 NUM2DBL (radius_x),
                 NUM2DBL (radius_y),
                 NUM2DBL (angle_1),
                 NUM2DBL (angle_2));

  return Qnil;
}

static VALUE
rb_cogl_path_curve_to (VALUE self,
                       VALUE x1,
                       VALUE y1,
                       VALUE x2,
                       VALUE y2,
                       VALUE x3,
                       VALUE y3)
{
  cogl_path_curve_to (NUM2DBL (x1),
                      NUM2DBL (y1),
                      NUM2DBL (x2),
                      NUM2DBL (y2),
                      NUM2DBL (x3),
                      NUM2DBL (y3));

  return Qnil;
}

static VALUE
rb_cogl_path_rel_curve_to (VALUE self,
                           VALUE x1,
                           VALUE y1,
                           VALUE x2,
                           VALUE y2,
                           VALUE x3,
                           VALUE y3)
{
  cogl_path_rel_curve_to (NUM2DBL (x1),
                          NUM2DBL (y1),
                          NUM2DBL (x2),
                          NUM2DBL (y2),
                          NUM2DBL (x3),
                          NUM2DBL (y3));

  return Qnil;
}

static VALUE
rb_cogl_path_close (VALUE self)
{
  cogl_path_close ();

  return Qnil;
}

static VALUE
rb_cogl_path_line (VALUE self,
                   VALUE x1,
                   VALUE y1,
                   VALUE x2,
                   VALUE y2)
{
  cogl_path_line (NUM2DBL (x1),
                  NUM2DBL (y1),
                  NUM2DBL (x2),
                  NUM2DBL (y2));

  return Qnil;
}

static VALUE
rb_cogl_path_rectangle (VALUE self,
                        VALUE x,
                        VALUE y,
                        VALUE width,
                        VALUE height)
{
  cogl_path_rectangle (NUM2DBL (x),
                       NUM2DBL (y),
                       NUM2DBL (width),
                       NUM2DBL (height));

  return Qnil;
}

static VALUE
rb_cogl_path_ellipse (VALUE self,
                      VALUE center_x,
                      VALUE center_y,
                      VALUE radius_x,
                      VALUE radius_y)
{
  cogl_path_ellipse (NUM2DBL (center_x),
                     NUM2DBL (center_y),
                     NUM2DBL (radius_x),
                     NUM2DBL (radius_y));

  return Qnil;
}

static VALUE
rb_cogl_path_round_rectangle (VALUE self,
                              VALUE x,
                              VALUE y,
                              VALUE width,
                              VALUE height,
                              VALUE radius,
                              VALUE arc_step)
{
  cogl_path_round_rectangle (NUM2DBL (x),
                             NUM2DBL (y),
                             NUM2DBL (width),
                             NUM2DBL (height),
                             NUM2DBL (radius),
                             NUM2DBL (arc_step));

  return Qnil;
}

static VALUE
rb_cogl_do_poly_func (VALUE data_value)
{
  PolyData *data = (PolyData *) data_value;
  int i;

  for (i = 0; i < data->argc; i++)
    data->points[i] = NUM2DBL (data->argv[i]);

  (* data->func) (data->points, data->argc / 2);

  return Qnil;
}

static VALUE
rb_cogl_free_poly_data (VALUE data_value)
{
  free (((PolyData *) data_value)->points);

  return Qnil;
}

static VALUE
rb_cogl_poly_func (PolyData *data)
{
  if ((data->argc & 1))
    rb_raise (rb_eArgError, "wrong number of arguments (pairs required)");

  data->points = ALLOC_N (float, data->argc);

  return rb_ensure (rb_cogl_do_poly_func, (VALUE) data,
                    rb_cogl_free_poly_data, (VALUE) data);
}

static VALUE
rb_cogl_path_polyline (int argc, VALUE *argv, VALUE self)
{
  PolyData data;

  data.argc = argc;
  data.argv = argv;
  data.func = cogl_path_polyline;

  return rb_cogl_poly_func (&data);
}

static VALUE
rb_cogl_path_polygon (int argc, VALUE *argv, VALUE self)
{
  PolyData data;

  data.argc = argc;
  data.argv = argv;
  data.func = cogl_path_polygon;

  return rb_cogl_poly_func (&data);
}

static VALUE
rb_cogl_get_path (VALUE self)
{
  return rb_cogl_handle_to_value ((CoglHandle *) cogl_get_path ());
}

static VALUE
rb_cogl_set_path (VALUE self, VALUE path)
{
  cogl_set_path ((CoglPath *) rb_cogl_handle_get_handle (path));
  return Qnil;
}

static VALUE
rb_cogl_path_dup (VALUE self)
{
  CoglPath *src = (CoglPath *) rb_cogl_handle_get_handle (self);
  CoglPath *dst = cogl_path_copy (src);

  return rb_cogl_handle_get_handle (dst);
}

static void
rb_cogl_path_init ()
{
  VALUE klass = rb_cogl_define_handle (cogl_is_path, "Path");
  rb_define_method(klass, "dup", rb_cogl_path_dup, 0);
}

void
rb_cogl_primitives_init ()
{
  rb_define_singleton_method (rbclt_c_cogl, "set_source_color",
                              rb_cogl_set_source_color, -1);
  rb_define_singleton_method (rbclt_c_cogl, "rectangle", rb_cogl_rectangle, 4);
  rb_define_singleton_method (rbclt_c_cogl, "path_fill", rb_cogl_path_fill, 0);
  rb_define_singleton_method (rbclt_c_cogl, "path_stroke",
                              rb_cogl_path_stroke, 0);
  rb_define_singleton_method (rbclt_c_cogl, "path_move_to",
                              rb_cogl_path_move_to, 2);
  rb_define_singleton_method (rbclt_c_cogl, "path_rel_move_to",
                              rb_cogl_path_rel_move_to, 2);
  rb_define_singleton_method (rbclt_c_cogl, "path_line_to",
                              rb_cogl_path_line_to, 2);
  rb_define_singleton_method (rbclt_c_cogl, "path_rel_line_to",
                              rb_cogl_path_rel_line_to, 2);
  rb_define_singleton_method (rbclt_c_cogl, "path_arc",
                              rb_cogl_path_arc, 6);
  rb_define_singleton_method (rbclt_c_cogl, "path_curve_to",
                              rb_cogl_path_curve_to, 6);
  rb_define_singleton_method (rbclt_c_cogl, "path_rel_curve_to",
                              rb_cogl_path_rel_curve_to, 6);
  rb_define_singleton_method (rbclt_c_cogl, "path_close",
                              rb_cogl_path_close, 0);
  rb_define_singleton_method (rbclt_c_cogl, "path_line",
                              rb_cogl_path_line, 4);
  rb_define_singleton_method (rbclt_c_cogl, "path_rectangle",
                              rb_cogl_path_rectangle, 4);
  rb_define_singleton_method (rbclt_c_cogl, "path_ellipse",
                              rb_cogl_path_ellipse, 4);
  rb_define_singleton_method (rbclt_c_cogl, "path_round_rectangle",
                              rb_cogl_path_round_rectangle, 6);
  rb_define_singleton_method (rbclt_c_cogl, "path_polyline",
                              rb_cogl_path_polyline, -1);
  rb_define_singleton_method (rbclt_c_cogl, "path_polygon",
                              rb_cogl_path_polygon, -1);
  rb_define_singleton_method (rbclt_c_cogl, "get_path",
                              rb_cogl_get_path, 0);
  rb_define_singleton_method (rbclt_c_cogl, "set_path",
                              rb_cogl_set_path, 1);
  rb_cogl_path_init ();
}
