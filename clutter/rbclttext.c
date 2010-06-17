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
#include <pango/pango.h>

#include "rbclutter.h"

static ID id_begin;
static ID id_end;
static ID id_exclude_end_p;

static VALUE
rbclt_text_initialize (int argc, VALUE *argv, VALUE self)
{
  VALUE font_name, text, color;
  gchar *font_name_s = NULL, *text_s = NULL;
  ClutterColor *color_s = NULL;
  ClutterActor *actor;

  rb_scan_args (argc, argv, "03", &font_name, &text, &color);

  if (!NIL_P (font_name))
    font_name_s = StringValuePtr (font_name);
  if (!NIL_P (text))
    text_s = StringValuePtr (text);
  if (!NIL_P (color))
    color_s = (ClutterColor *) RVAL2BOXED (color, CLUTTER_TYPE_COLOR);

  actor = clutter_text_new ();

  if (font_name_s)
    clutter_text_set_font_name (CLUTTER_TEXT (actor), font_name_s);
  if (text_s)
    clutter_text_set_text (CLUTTER_TEXT (actor), text_s);
  if (color_s)
    clutter_text_set_color (CLUTTER_TEXT (actor), color_s);

  rbclt_initialize_unowned (self, actor);

  return Qnil;
}

static VALUE
rbclt_text_get_layout (VALUE self)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));
  return GOBJ2RVAL (clutter_text_get_layout (text));
}

static VALUE
rbclt_text_set_markup (VALUE self, VALUE markup)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));

  clutter_text_set_markup (text, StringValuePtr (markup));

  return self;
}

static VALUE
rbclt_text_insert_unichar (VALUE self, VALUE ch)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));

  clutter_text_insert_unichar (text, rbclt_num_to_gunichar (ch));

  return self;
}

static VALUE
rbclt_text_delete_chars (VALUE self, VALUE n_chars)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));

  clutter_text_delete_chars (text, NUM2UINT (n_chars));

  return self;
}

static VALUE
rbclt_text_insert_text (int argc, VALUE *argv, VALUE self)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));
  VALUE str, pos;

  rb_scan_args (argc, argv, "11", &str, &pos);

  clutter_text_insert_text (text, StringValuePtr (str),
                            NIL_P (pos) ? -1 : NUM2INT (pos));

  return self;
}

static VALUE
rbclt_text_delete_text (VALUE self, VALUE start_pos, VALUE end_pos)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));

  clutter_text_delete_text (text, NUM2INT (start_pos), NUM2INT (end_pos));

  return self;
}

static VALUE
rbclt_text_get_chars (int argc, VALUE *argv, VALUE self)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));
  VALUE start_pos_arg, end_pos_arg, ret;
  gssize start_pos, end_pos;
  gchar *str;

  rb_scan_args (argc, argv, "11", &start_pos_arg, &end_pos_arg);

  /* If the first argument looks like a range then we'll use the
     values from that instead */
  if (rb_respond_to (start_pos_arg, id_exclude_end_p)
      && rb_respond_to (start_pos_arg, id_begin)
      && rb_respond_to (start_pos_arg, id_end))
    {
      start_pos = NUM2INT (rb_funcall (start_pos_arg, id_begin, 0));
      end_pos = NUM2INT (rb_funcall (start_pos_arg, id_end, 0));
      if (!RTEST (rb_funcall (start_pos_arg, id_exclude_end_p, 0)))
        end_pos++;
    }
  else
    {
      start_pos = NUM2INT (start_pos_arg);
      end_pos = NIL_P (end_pos_arg) ? -1 : NUM2INT (end_pos_arg);
    }

  str = clutter_text_get_chars (text, start_pos, end_pos);

  ret = rb_str_new2 (str);

  g_free (str);

  return ret;
}

static VALUE
rbclt_text_delete_selection (VALUE self)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));

  clutter_text_delete_selection (text);

  return self;
}

static VALUE
rbclt_text_activate (VALUE self)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));

  clutter_text_activate (text);

  return self;
}

static VALUE
rbclt_text_position_to_coords (VALUE self, VALUE position)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));
  gfloat x, y, line_height;

  if (!clutter_text_position_to_coords (text, NUM2INT (position),
                                        &x, &y, &line_height))
    return Qnil;
  else
    return rb_ary_new3 (3, rb_float_new (x), rb_float_new (y),
                        rb_float_new (line_height));
}

static VALUE
rbclt_text_set_preedit_string (VALUE self,
                               VALUE preedit_str,
                               VALUE attrs,
                               VALUE cursor_pos)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));

  clutter_text_set_preedit_string (text,
                                   NIL_P (preedit_str) ? NULL
                                   : StringValuePtr (preedit_str),
                                   NIL_P (attrs) ? NULL
                                   : RVAL2BOXED (attrs, PANGO_TYPE_ATTR_LIST),
                                   NUM2UINT (cursor_pos));

  return self;
}

static VALUE
rbclt_text_get_selection (VALUE self)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));
  gchar *str;
  VALUE ret;

  str = clutter_text_get_selection (text);

  if (str)
    ret = rb_str_new2 (str);
  else
    ret = Qnil;

  g_free (str);

  return ret;
}

static VALUE
rbclt_text_set_selection (VALUE self, VALUE start_pos, VALUE end_pos)
{
  ClutterText *text = CLUTTER_TEXT (RVAL2GOBJ (self));

  clutter_text_set_selection (text, NUM2INT (start_pos), NUM2INT (end_pos));

  return self;
}

void
rbclt_text_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_TYPE_TEXT, "Text", rbclt_c_clutter);

  id_begin = rb_intern ("begin");
  id_end = rb_intern ("end");
  id_exclude_end_p = rb_intern ("exclude_end?");

  rb_define_method (klass, "initialize", rbclt_text_initialize, -1);
  rb_define_method (klass, "layout", rbclt_text_get_layout, 0);
  rb_define_method (klass, "set_markup", rbclt_text_set_markup, 1);
  rb_define_method (klass, "insert_unichar", rbclt_text_insert_unichar, 1);
  rb_define_method (klass, "delete_chars", rbclt_text_delete_chars, 1);
  rb_define_method (klass, "insert_text", rbclt_text_insert_text, -1);
  rb_define_method (klass, "delete_text", rbclt_text_delete_text, 2);
  rb_define_method (klass, "get_chars", rbclt_text_get_chars, -1);
  rb_define_method (klass, "delete_selection", rbclt_text_delete_selection, 0);
  rb_define_method (klass, "activate", rbclt_text_activate, 0);
  rb_define_method (klass, "position_to_coords",
                    rbclt_text_position_to_coords, 1);
  rb_define_method (klass, "set_preedit_string",
                    rbclt_text_set_preedit_string, 3);
  rb_define_method (klass, "selection", rbclt_text_get_selection, 0);
  rb_define_method (klass, "set_selection", rbclt_text_set_selection, 2);
  rb_define_alias (klass, "<<", "insert_text");

  rb_define_alias (klass, "to_s", "text");

  G_DEF_SETTERS (klass);
}
